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

}
