//********************************************************************************************************
//
//        Sensor de Ruído 
//        Projeto IMT 2025
//        Prof. Eng. Nuncio Perrella
//        28/01/2025
//
//********************************************************************************************************

#include <Arduino.h>
#include <driver/i2s.h>
#include "soc/i2s_reg.h"
#include <math.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "ThingSpeak.h"

 

#if !(USING_DEFAULT_ARDUINO_LOOP_STACK_SIZE)
  //uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 16384;
  uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 8192;
#endif

#define BUFLEN 256

#define I2C_SDA 21
#define I2C_SCL 22

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int cont = 0;
int n = 0;
int LED1 = 13;            // Led Verde
int LED2 = 14;            // Led Amarelo
int LED3 = 26;            // Led Vermelho
int LED4 = 2;             // Led Azul placa
boolean pisca = false;


// Variáveis para controle do tempo
unsigned long lastTime = 0; // Tempo da última execução
const unsigned long interval = 30000; // Intervalo de 30 segundos (em milissegundos)


// Configurações do ThingSpeak
const char* apiKey = "SEMW4E1BY3H0A2OB";      // Substitua pela API Key do seu canal ThingSpeak
const long channelID = 2822718;   // Substitua pelo ID do seu canal ThingSpeak


//const char* ssid     = "SMG";
//const char* password = "12102020";


//const char* ssid     = "4G-MIFI-E88";
//const char* password = "1234567890";


//const char* ssid     = "ZTE_E58644";
//const char* password = "08032114";

const char* ssid     = "ana";
const char* password = "anahelena";

WiFiClient  client;           // Wifi simples



float dB;
static const i2s_port_t i2s_num = I2S_NUM_0; // i2s port number
static const i2s_config_t i2s_config = 
{
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
 
static const i2s_pin_config_t pin_config =
{
    .bck_io_num = 23,                                        // BCKL  23   Amarelo
    .ws_io_num = 33,                                         // LRCL  33   Vermelho
    .data_out_num = I2S_PIN_NO_CHANGE,                       // not used (only for speakers)
    .data_in_num = 32                                        // DOUT  32    Laranja
};




void setup() 
{ 
  Serial.begin(115200);
  Serial.println("Configuring I2S...");
  //SerialBT.begin("IMT_2025"); 


	lcd.init(I2C_SDA, I2C_SCL); // initialize the lcd to use user defined I2C pins
	lcd.backlight();
	lcd.setCursor(3,0);
	lcd.print("IMT 2025");
	lcd.setCursor(2,1);
	lcd.print("Prof. Nuncio");
  delay(3000);
  lcd.clear();


  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
  pinMode(LED4, OUTPUT);
  pinMode(32, INPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);


  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);          // install and start i2s driver
  REG_SET_BIT(  I2S_TIMING_REG(i2s_num),BIT(9));               
  REG_SET_BIT( I2S_CONF_REG(i2s_num), I2S_RX_MSB_SHIFT);
  i2s_set_pin(i2s_num, &pin_config);



 WiFi.mode(WIFI_STA); 
 WiFi.begin(ssid, password); 
                                                            
      cont = 0;
      Serial.print("Setup - Inciciando a conexão WiFi");
      while(WiFi.status() != WL_CONNECTED)
      {
        pisca = !pisca; 
        digitalWrite(LED4, pisca);                       //Led Azul
       
        Serial.print("Setup - Tentando Conexão Wifi: ");        // Depuração
        cont = cont +1;                                 // Depuração
        Serial.println(cont);                           // Depuração

        lcd.clear();
	      lcd.backlight();
	      lcd.setCursor(0,0);
	      lcd.print("Conectando ");
        lcd.setCursor(0,1);
	      lcd.print("Tentativa: ");
        lcd.setCursor(12,1);
	      lcd.print(cont);
          

        delay(1000);     
     } 
     
      WiFi.setAutoReconnect(true); // Habilitar reconexão automática
      WiFi.persistent(true);       // Salvar configuração na memória
      Serial.println();                                         
      Serial.println("WiFi conectado. ");
      Serial.print("WiFi SSID: ");
      Serial.println(WiFi.SSID()); 
      Serial.print("IP Modulo: "); 
      Serial.println(WiFi.localIP());
      Serial.println();

      lcd.clear();
	    lcd.backlight();
	    lcd.setCursor(0,0);
	    lcd.print("WiFi Conectado");
      delay(3000);
      digitalWrite(LED4, LOW);
    
  // Inicializa o ThingSpeak
  ThingSpeak.begin(client);

}

int32_t audio_buf[BUFLEN];

void loop() 
{
    unsigned long currentTime = millis();
    int bytes_read = i2s_read_bytes(i2s_num, audio_buf, sizeof(audio_buf), portMAX_DELAY);
    int32_t cleanBuf[BUFLEN / 2] {0};
    int cleanBufIdx = 0;
    for (int i = 0; i < BUFLEN; i++)
    {
      if (audio_buf[i] != 0)    // Exclude values from other channel
      {
          cleanBuf[cleanBufIdx] = audio_buf[i] >> 14;
          cleanBufIdx++;
      }
    }
    float meanval = 0;
    int volCount = 0;
    for (int i=0; i < BUFLEN / 2; i++) 
    {
         if (cleanBuf[i] != 0)
         {
          meanval += cleanBuf[i];
          volCount++;
         }
    }
    meanval /= volCount;

    // subtract it from all sapmles to get a 'normalized' output
    for (int i=0; i< volCount; i++) 
    {
        cleanBuf[i] -= meanval;
    }

    // find the 'peak to peak' max
    float maxsample, minsample;
    minsample = 100000;
    maxsample = -100000;
    for (int i=0; i<volCount; i++) {
      minsample = _min(minsample, cleanBuf[i]);
      maxsample = _max(maxsample, cleanBuf[i]);
    }
    //Serial.print("Volume SPH: ");
    //Serial.println(maxsample - minsample);
    dB = maxsample - minsample;
    dB = 10*log(dB);                                               // Coletor parabólico adicionar 12 dB  --> 10*log(dB) + 12
    Serial.print(dB);
    //SerialBT.print(dB);
    Serial.println();

    lcd.clear();
	  lcd.backlight();
	  lcd.setCursor(0,0);
	  lcd.print("IMT Sonometro");
	  lcd.setCursor(0,1);
	  lcd.print(dB);
    lcd.setCursor(7,1);
	  lcd.print("dB"); 
    

   if(dB < 60)
   {
    digitalWrite(LED1, HIGH);   // Verde
    digitalWrite(LED2, LOW);    // Amarelo
    digitalWrite(LED3, LOW);    // Vermelho
   }

   if(dB >= 60 && dB < 80)
   {
    digitalWrite(LED1, LOW);    // Verde
    digitalWrite(LED2, HIGH);   // Amarelo
    digitalWrite(LED3, LOW);    // Vermelho
   }

    if(dB >= 80)
   {
    digitalWrite(LED1, LOW);     // Verde
    digitalWrite(LED2, LOW);     // Amarelo
    digitalWrite(LED3, HIGH);    // Vermelho
   }

    delay(500);

    // Verifica se o intervalo de 30 segundos passou
  if (currentTime - lastTime >= interval) 
  {
    lastTime = currentTime; // Atualiza o tempo da última execução
      
    int status = ThingSpeak.writeField(channelID, 1, dB, apiKey);

    if (status == 200) 
    {
      Serial.println("Dado enviado com sucesso!");
    } 
    
    else 
    {
    Serial.print("Erro ao enviar dado: ");
    Serial.println(status);
    }
  }

}
