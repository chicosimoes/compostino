/*

 Compostino
 ==========
 Unidade de monitoramento de composteiras

 inicio em 7/7/2013 durante oficina aberta no 
 veracidade Sao Carlos SP brasil copyleft

 metano in arduino.cc
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe
 
 lm35 in arduino.cc
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe

 created 18 Dec 2009
 by David A. Mellis
 
 modified 19 Mar 2012
 by Sam Fentress

 modified 14 Oct 2014
 by Chico Simões & Vilson Vieira
 
 runs with: http://jsbin.com/romenu/3/edit

 */

//#include <OneWire.h> 

#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <SD.h>
 
// Enderecos MAC e IP do Arduino
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(172, 16, 3, 31);
char callback[27] = "arduinoEthernetComCallback";

//int dht11pino = 2; // pino digital do sensor dht11
int metanoPino = A0;   // escolha o pino que recebe o sinal do sensor de metano
int calorPino = 3;  // escolha o pino que recebe o sinal do sensor de temperatura
int umidadePino = A2; // escolha o pino que recebe o sinal do sensor de umidade
OneWire ds(calorPino);
float valorMetano = 0;  //  guarda o valor medido no sensor MQ-4(metano)
float limiteMetano = 600; // valor definido como limiar maximo de metano

int ledcalormeso = 10;   //ascende pino na temperatura boa para bact mesofilicas
int ledcalortermo = 11;  //ascende pino na temperatura boa para bact termofilicas
int ledcalorfrito = 12;  //ascende pino quando temperatura  muito alta
int ledMetano = 13;      // escolha o pino que acendera o ledMetano
float valorcalor = 0;    //guarda valor medido sensor LM35(temperatura)
float limitecalor = 70;  //Limite maximo de temperatura

int ledseco = 8;  //ascende pino quando umidade esta baixa. Precisa molhar composteira.
int ledmolhado = 9;  //ascende pino quando umidade esta muito alta. 

const int chipSelect = 4; // pino do cartao SD

float valorumidade = 0; //guarda valor do sensor caseiro (umidade)
float umidademin = 300;  //limite minimo da faixa ideal de umidade
float umidademax = 370;   //limite maximo da faixa ideal de umidade

// Porta do servidor (padrao: 80) 
EthernetServer server(80);
 
void setup() {
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  // declarando o ledMetano como OUTPUT:
  pinMode(ledMetano, OUTPUT);

  // declarando os ledcalor como OUTPUT:
  pinMode(ledcalormeso, OUTPUT);
  pinMode(ledcalortermo, OUTPUT);
  pinMode(ledcalorfrito, OUTPUT);

  // declarando os ledumidade como OUTPUT:
  pinMode(ledseco, OUTPUT);
  pinMode(ledmolhado, OUTPUT);
  
  // inicializa o cartao SD
  if (!SD.begin(chipSelect)) {
    // se der erro, nao faz nada
    return;
  }
}
 
void loop() {
  // string para armazenar dados para cartao SD
  char dataString[255];
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println();
 
          // Comeca a escrever o JSON
          client.print(callback);
          client.print("('{");

          // METANO

          valorMetano = analogRead(metanoPino);

          if (valorMetano >= limiteMetano){
            digitalWrite(ledMetano, HIGH);
          } else {
            digitalWrite(ledMetano, LOW);
          }

          // Escreve JSON
          client.print("\"Metano\": ");
          client.print(valorMetano);
          client.print(",");

          // Escreve string para cartao SD
          //char tmp[255] = "";
         // sprintf(dataString, "%f", valorMetano);
          //dtostrf(valorMetano, 0, 1, dataString);
          //dataString += String((float)valorMetano);
         // dataString += ",";

          // TEMPERATURA

          valorcalor = getTemp(); 

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

          client.print("\"Temperatura\": ");
          client.print(valorcalor);
          client.print(",");

          // lendo o valor do sensor:
          valorumidade = analogRead(umidadePino);

          // umidade menor que 40, significa que esta muito seco, ledseco ligado:
          if (valorumidade < umidademin) {
            digitalWrite(ledseco, HIGH);  
            digitalWrite(ledmolhado, LOW);  
          }

          // umidade maior que 370, significa que esta muito molhado, ledmolhado ligado:
          if (valorumidade > umidademax){
            digitalWrite(ledseco, LOW);  
            digitalWrite(ledmolhado, HIGH);

          }   
          // umidade entre 40 e 370, todos os leds desligado:
          if (valorumidade > umidademin && valorumidade < umidademax) {
            digitalWrite(ledseco, LOW);  
            digitalWrite(ledmolhado, LOW);
          }

          client.print("\"Umidade\": ");
          client.print(valorumidade);
          // client.print(",");

          // funçao umidade do ar: 

          // int chk = DHT11.read(dht11pino);
          // Serial.print("umidade do ar: ");
          // Serial.println((float)DHT11.humidity, 2);

          // Serial.print("Temperatura do ar: ");
          // Serial.println((float)DHT11.temperature, 2);

          client.println("}')");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        File dataFile = SD.open("log.txt", FILE_WRITE);
        // if the file is available, write to it:
        if (dataFile) {
          dataFile.println(dataString);
          dataFile.close();
        }  else {
          // erro, nao consegue abrir o arquivo
        } 
      }
    }
    // give the web browser time to receive the data
    delay(1000);
    // close the connection:
    client.stop();
  }
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

   float TRead = ((MSB <<  8) | LSB); 
   float Temperature = TRead / 16;

   return Temperature;
 }
