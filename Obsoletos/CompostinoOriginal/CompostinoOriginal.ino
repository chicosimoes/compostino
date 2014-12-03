/*
Compostino = Unidade de monitoramento de composteiras.
 inicio em 7/7/2013 durante oficina aberta no veracidade Sao Carlos SP brasil copyleft
 
 Partimos de:
 
 metano in arduino.cc
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe
 
 lm35 in arduino.cc
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe
 
 */

#include <dht11.h>
#include <OneWire.h>

 
dht11 DHT11;


int dht11pino = 2; // pino digital do sensor dht11
int metanoPino = A0;   // escolha o pino que recebe o sinal do sensor de metano
int calorPino = 3;  // escolha o pino que recebe o sinal do sensor de temperatura
int umidadePino = A2; // escolha o pino que recebe o sinal do sensor de umidade
OneWire ds(calorPino);
int ledMetano = 13;      // escolha o pino que acendera o ledMetano
float valorMetano = 0;  //  guarda o valor medido no sensor MQ-4(metano)
float limiteMetano = 300; // valor definido como limiar maximo de metano

int ledcalormeso = 10;   //ascende pino na temperatura boa para bact mesofilicas
int ledcalortermo = 11;  //ascende pino na temperatura boa para bact termofilicas
int ledcalorfrito = 12;  //ascende pino quando temperatura  muito alta
float valorcalor = 0;    //guarda valor medido sensor LM35(temperatura)
float limitecalor = 70;  //Limite maximo de temperatura

int ledseco = 8;  //ascende pino quando umidade esta baixa. Precisa molhar composteira.
int ledmolhado = 9;  //ascende pino quando umidade esta muito alta. 
float valorumidade = 0; //guarda valor do sensor caseiro (umidade)
float umidademin = 400;  //limite minimo da faixa ideal de umidade
float umidademax = 600;   //limite maximo da faixa ideal de umidade


void setup() {
  // declarando o ledMetano como OUTPUT:
  pinMode(ledMetano, OUTPUT);

  // declarando os ledcalor como OUTPUT:
  pinMode(ledcalormeso, OUTPUT);
  pinMode(ledcalortermo, OUTPUT);
  pinMode(ledcalorfrito, OUTPUT);

  // declarando os ledumidade como OUTPUT:
  pinMode(ledseco, OUTPUT);
  pinMode(ledmolhado, OUTPUT);

  Serial.begin(9600);  
}

void loop() {

  // funçao Metano

  // lendo o valor do sensor:
  valorMetano =analogRead(metanoPino);   
  // liga o ledMetano

    // imprime o resultado no monitor serial:
  Serial.print("Metano igual a: " );                       
  Serial.println(valorMetano); 

  // dependendo do valorMetado acende ou apaga led limiteMetano
  if (valorMetano >= limiteMetano){
    digitalWrite(ledMetano, HIGH);
  }
  else{

    digitalWrite(ledMetano, LOW);
  }



  // funçao temperatura

  //valorcalor =(5.0 * analogRead(calorPino) * 100.0) / 1024;
 valorcalor =getTemp();

  // acender o primeiro ledCalor, se temperatura maior que 35
  if (valorcalor > 35) {
    digitalWrite(ledcalormeso, HIGH);  
    digitalWrite(ledcalortermo, LOW);  
    digitalWrite(ledcalorfrito, LOW);  
  }

  // acender primeiro e segundo ledCalor, se temperatura maior que 50 
  if (valorcalor > 50) {              
    digitalWrite(ledcalormeso, HIGH);  
    digitalWrite(ledcalortermo, HIGH);  
    digitalWrite(ledcalorfrito, LOW);  

  }
  // acende primeiro, segundo e terceiro ledCalor, se temperatura maior que 70  
  if (valorcalor > 70) {              
    digitalWrite(ledcalormeso, HIGH);  
    digitalWrite(ledcalortermo, HIGH);  
    digitalWrite(ledcalorfrito, HIGH);  

  }

  // se temperatura menor que 35, primeiro, segundo e terceiro ledcalor desligado
  else {
    digitalWrite(ledcalormeso, LOW);  
    digitalWrite(ledcalortermo, LOW);  
    digitalWrite(ledcalorfrito, LOW); 

  }

  // imprime resultados no monitor serial:
  Serial.print("A temperatura igual a: " );                       
  Serial.println(valorcalor);      

  //funçao umidade

  // lendo o valor do sensor:
  valorumidade =1024-analogRead(umidadePino);

  // umidade menor que 40%, significa que esta muito seco, ledseco ligado:
  if (valorumidade < umidademin) {
    digitalWrite(ledseco, HIGH);  
    digitalWrite(ledmolhado, LOW);  
  }

  // umidade maior que 60%, significa que esta muito molhado, ledmolhado ligado:
  if (valorumidade > umidademax){
    digitalWrite(ledseco, LOW);  
    digitalWrite(ledmolhado, HIGH);

  }   
  // umidade entre 40% e 60%, todos os leds desligado:
  if (valorumidade > umidademin && valorumidade < umidademax) {
    digitalWrite(ledseco, LOW);  
    digitalWrite(ledmolhado, LOW);
  }

  Serial.print("Umidade do composto igual a: " );                       
  Serial.println(valorumidade); 

  // funçao umidade do ar: 

  int chk = DHT11.read(dht11pino);
  Serial.print("umidade do ar: ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Temperatura do ar: ");
  Serial.println((float)DHT11.temperature, 2);

  Serial.println ( );
  delay(3000); 

}

float getTemp(){

byte data[12];
byte addr[8];

if ( !ds.search(addr)) {
//no more sensors on chain, reset search
ds.reset_search();
return -1000;
}

if ( OneWire::crc8( addr, 7) != addr[7]) {
Serial.println("CRC is not valid!");
return -1000;
}

if ( addr[0] != 0x10 && addr[0] != 0x28) {
Serial.print("Device is not recognized");
return -1000;
}

ds.reset();
ds.select(addr);
ds.write(0x44,1); 

byte present = ds.reset();
ds.select(addr); 
ds.write(0xBE); 


for (int i = 0; i < 9; i++) { 
data[i] = ds.read();
}

ds.reset_search();

byte MSB = data[1];
byte LSB = data[0];

float TRead = ((MSB << 8) | LSB); 
float Temperature = TRead / 16;

return Temperature;

}




