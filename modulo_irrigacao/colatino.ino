/*
  Automatizador de rega usando arduino, vávula solenóide de máquina de lavar.
	Autor chicosimoes
	contribuição Vinicius Violetti
 
 */
#include <LiquidCrystal.h> 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 
int ledUmi = 13;
// the setup routine runs once when you press reset:
void setup() {   
lcd.begin(16, 2); //Inicia o LCD com dimensões 16x2(Colunas x Linhas)
lcd.setCursor(0, 0); //Posiciona o cursor na primeira coluna(0) e na primeira linha(0) do LCD
lcd.print("Umidade:"); //Escreve no LCD "Olá Garagista!"
lcd.setCursor(0, 1); //Posiciona o cursor na primeira coluna(0) e na segunda linha(1) do LCD
lcd.print("Luz:"); //Escreve no LCD "Luz"
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(ledUmi, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorUmi = analogRead(A0);
  // print out the value you read:
  Serial.println("A umidade e");
  Serial.println(sensorUmi);
  lcd.setCursor(13, 0);
  lcd.println(sensorUmi);
  
       // delay in between reads for stability

  int sensorLuz = analogRead(A1);
  // print out the value you read:
  Serial.println("A luminosidade e");
  Serial.println(sensorLuz);
  lcd.setCursor(13, 1);
  lcd.println(sensorLuz);
  
  if (sensorUmi <= 400){
digitalWrite(ledUmi, HIGH);
}
else{
digitalWrite(ledUmi, LOW);
}
 
  delay(1000); 

}
