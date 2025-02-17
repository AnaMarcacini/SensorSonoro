//******************************************************************************************************************//
//                                                                                                                  //
//      Instituto Mauá de Tecnologia                                                                                // 
//      Engenharia de Computação - 2025                                                                             //
//                                                                                                                  //
//      Coordenação:                                                                                                //
//      Prof. Dr. Ângelo Sebastião Zanini                                                                           //
//                                                                                                                  //
//      Professores:                                                                                                //
//      Prof. MSc Nuncio Perrella                                                                                   //
//      Prof. MSc Everson Denis                                                                                     //
//                                                                                                                  //
//      Corpo Técnico:                                                                                              //
//      Eng. Gabriel Godoy                                                                                          //
//      Enga. Ana Helena                                                                                            // 
//      Eng. Matheus Coelho                                                                                         //
//                                                                                                                  //
//******************************************************************************************************************//                               

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include "soc/i2s_reg.h"
#include <math.h>
#include "ThingSpeak.h"

#define BUFLEN 256
#define I2C_SDA 21
#define I2C_SCL 22


LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD: 0x27, 16 colunas, 2 linhas


int LED1 = 13;            // Led Verde
int LED2 = 14;            // Led Amarelo
int LED3 = 26;            // Led Vermelho
int LED4 = 2;             // Led Azul placa
int cont = 0;
boolean pisca = false;


const char* ssid     = "ZTE_E5864";          //  Roteador celular
const char* password = "0803211";            //  Senha

                                              // Configurações do ThingSpeak
const char* apiKey = "GSA6IINXZV96XRN";      // Substitua pela API Key do seu canal ThingSpeak
const long channelID = 284164;               // Substitua pelo ID do seu canal ThingSpeak

                                              // Variáveis para controle do tempo
unsigned long lastTime = 0;                   // Tempo da última execução
const unsigned long interval = 30000;         // Intervalo de 30 segundos (em milissegundos)

WiFiClient client;                            // Wifi simples



float dB;
static const i2s_port_t i2s_num = I2S_NUM_0;  // i2s port number
static const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 22050,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
};

// Configuração placa ESP32
static const i2s_pin_config_t pin_config = {
    .bck_io_num = 23,  // BCKL  23   Amarelo
    .ws_io_num = 33,   // LRCL  33   Vermelho
    .data_out_num = I2S_PIN_NO_CHANGE, // not used (only for speakers)
    .data_in_num = 32  // DOUT  32    Laranja
};



void setup() {

  Serial.begin(115200);
  Serial.println("IMT - Engenharia da Computação - 2025");
     
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);  

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);

  Wire.begin(I2C_SDA, I2C_SCL);   // Inicializa a comunicação I2C com os pinos definidos
  lcd.begin(16, 2);               // Inicializa o LCD com 16 colunas e 2 linhas
  lcd.backlight();                // Liga a luz de fundo
  lcd.setCursor(3, 0);            // Posiciona curso na linha 0, terceira coluna
  lcd.print("IMT 2025");
  lcd.setCursor(2, 1);
  lcd.print("Prof. Nuncio");
  delay(3000);
  lcd.clear();


  i2s_driver_install(i2s_num, &i2s_config, 0, NULL); // install and start i2s driver
  REG_SET_BIT(I2S_TIMING_REG(i2s_num), BIT(9));
  REG_SET_BIT(I2S_CONF_REG(i2s_num), I2S_RX_MSB_SHIFT);
  i2s_set_pin(i2s_num, &pin_config);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  cont = 0;
  Serial.print("Setup - Iniciando a conexão WiFi");
  while (WiFi.status() != WL_CONNECTED) {
        pisca = !pisca;
        digitalWrite(LED4, pisca); // Led Azul

        Serial.print("Setup - Tentando Conexão Wifi: "); // Depuração
        cont = cont + 1;                                 // Depuração
        Serial.println(cont);                            // Depuração

        lcd.clear();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Conectando ");
        lcd.setCursor(0, 1);
        lcd.print("Tentativa: ");
        lcd.setCursor(12, 1);
        lcd.print(cont);
        delay(1000); 
    }

    WiFi.setAutoReconnect(true);      // Habilitar reconexão automática
    WiFi.persistent(true);            // Salvar configuração na memória
    Serial.println();
    Serial.println("WiFi conectado. ");
    Serial.print("WiFi SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Modulo: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Conectado");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(3000);
    digitalWrite(LED4, LOW);

   
    ThingSpeak.begin(client);     // Inicializa o ThingSpeak

}


int32_t audio_buf[BUFLEN];

void loop() {

    unsigned long currentTime = millis();
    size_t bytes_read;
    i2s_read(i2s_num, audio_buf, sizeof(audio_buf), &bytes_read, portMAX_DELAY);
    int32_t cleanBuf[BUFLEN / 2] = {0};
    int cleanBufIdx = 0;
    for (int i = 0; i < BUFLEN; i++) {
        if (audio_buf[i] != 0) { // Exclude values from other channel
            cleanBuf[cleanBufIdx] = audio_buf[i] >> 14;
            cleanBufIdx++;
        }
    }
    float meanval = 0;
    int volCount = 0;
    for (int i = 0; i < BUFLEN / 2; i++) {
        if (cleanBuf[i] != 0) {
            meanval += cleanBuf[i];
            volCount++;
        }
    }
    meanval /= volCount;

    // Normaliza os dados
    for (int i = 0; i < volCount; i++) {
        cleanBuf[i] -= meanval;
    }

    // Encontra o valor máximo e mínimo
    float maxsample = -100000, minsample = 100000;
    for (int i = 0; i < volCount; i++) {
        minsample = fmin(minsample, cleanBuf[i]);
        maxsample = fmax(maxsample, cleanBuf[i]);
    }

    dB = maxsample - minsample;
    dB = 10 * log(dB); // Coletor parabólico adicionar 12 dB  --> 10*log(dB) + 12
    Serial.print(dB);
    Serial.println();

    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("IMT Sonometro");
    lcd.setCursor(0, 1);
    lcd.print(dB);
    lcd.setCursor(7, 1);
    lcd.print("dB");

    if (dB < 60) {
        Serial.println("Menor que 60");
        digitalWrite(LED1, HIGH); // Verde
        digitalWrite(LED2, LOW);  // Amarelo
        digitalWrite(LED3, LOW);  // Vermelho
    } else if (dB >= 60 && dB < 80) {
        Serial.println("Entre 60 e 80");
        digitalWrite(LED1, LOW);  // Verde
        digitalWrite(LED2, HIGH); // Amarelo
        digitalWrite(LED3, LOW);  // Vermelho
    } else if (dB >= 80) {
        Serial.println("Maior que 80");
        digitalWrite(LED1, LOW);  // Verde
        digitalWrite(LED2, LOW);  // Amarelo
        digitalWrite(LED3, HIGH); // Vermelho
    }
    delay(500);

                                                  // Verifica se o intervalo de 30 segundos passou
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime;                   // Atualiza o tempo da última execução

        int status = ThingSpeak.writeField(channelID, 1, dB, apiKey);

        if (status == 200) {
            Serial.println("Dado enviado com sucesso!");
        } else {
            Serial.print("Erro ao enviar dado: ");
            Serial.println(status);
        }
    }
}
