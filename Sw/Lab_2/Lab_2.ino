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


#define I2C_SDA 21
#define I2C_SCL 22


LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD: 0x27, 16 colunas, 2 linhas


int LED1 = 13;            // Led Verde
int LED2 = 14;            // Led Amarelo
int LED3 = 26;            // Led Vermelho
int LED4 = 2;             // Led Azul placa


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

}

void loop() {

    digitalWrite(LED1, HIGH);     // Acende Led Verde
    delay(1000);
    digitalWrite(LED2, HIGH);     // Acende Led Amarelo
    delay(1000);
    digitalWrite(LED3, HIGH);     // Acende Led Vermelho
    delay(1000);
    digitalWrite(LED4, HIGH);     // Acende Led Azul
    delay(1000);
    digitalWrite(LED1, LOW);      // Apaga Led Verde
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    delay(1000);
    Serial.println("Grupo - X ");

    lcd.setCursor(3, 0);            // Posiciona curso na linha 0, terceira coluna
    lcd.print("IMT 2025");
    lcd.setCursor(2, 1);
    lcd.print("Grupo - X");

}
