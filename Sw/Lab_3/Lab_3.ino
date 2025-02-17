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


#define I2C_SDA 21
#define I2C_SCL 22


LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD: 0x27, 16 colunas, 2 linhas


int LED1 = 13;            // Led Verde
int LED2 = 14;            // Led Amarelo
int LED3 = 26;            // Led Vermelho
int LED4 = 2;             // Led Azul placa
int cont = 0;
boolean pisca = false;


const char* ssid     = "ZTE_E58644";          //  Roteador celular
const char* password = "08032114";            //  Senha

WiFiClient client;        // Wifi simples


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

}

void loop() {

    digitalWrite(LED1, HIGH);     // Acende Led Verde
    delay(1000);
    digitalWrite(LED2, HIGH);     // Acende Led Amarelo
    delay(1000);
    digitalWrite(LED3, HIGH);     // Acende Led Vermelho
    delay(1000);
    digitalWrite(LED1, LOW);      // Apaga Led Verde
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    delay(1000);

}
