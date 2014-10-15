#include <SPI.h>
#include <Ethernet.h>
#include <stdlib.h>

//#include <OneWire.h>

// Enabe debug tracing to Serial port.
#define DEBUG

// Here we define a maximum framelength to 64 bytes. Default is 256.
#define MAX_FRAME_LENGTH 64

#include <WebSocket.h>

// Endereco MAC e IP do Ethernet shield conectado no Arduino
byte mac[] = { 0x52, 0x4F, 0x43, 0x4B, 0x45, 0x54 };
byte ip[] = { 192, 168, 0 , 42 };

// String auxiliar para converter de float para string
char s[32];

int metanoPino = A0;   // escolha o pino que recebe o sinal do sensor de metano
int calorPino = 10;  // escolha o pino que recebe o sinal do sensor de temperatura
int umidadePino = A2; // escolha o pino que recebe o sinal do sensor de umidade
//OneWire ds(calorPino);
int ledMetano = 13;      // escolha o pino que acendera o ledMetano
float valorMetano = 0;  //  guarda o valor medido no sensor MQ-4(metano)
float limiteMetano = 600; // valor definido como limiar maximo de metano

int ledcalormeso = 10;   //ascende pino na temperatura boa para bact mesofilicas
int ledcalortermo = 11;  //ascende pino na temperatura boa para bact termofilicas
int ledcalorfrito = 12;  //ascende pino quando temperatura  muito alta
float valorcalor = 0;    //guarda valor medido sensor LM35(temperatura)
float limitecalor = 70;  //Limite maximo de temperatura

int ledseco = 8;  //ascende pino quando umidade esta baixa. Precisa molhar composteira.
int ledmolhado = 9;  //ascende pino quando umidade esta muito alta. 
float valorumidade = 0; //guarda valor do sensor caseiro (umidade)
float umidademin = 300;  //limite minimo da faixa ideal de umidade
float umidademax = 370;   //limite maximo da faixa ideal de umidade


// Create a Websocket server
WebSocket wsServer;

void onConnect(WebSocket &socket) {
  Serial.println("onConnect called");
}


// You must have at least one function with the following signature.
// It will be called by the server when a data frame is received.
void onData(WebSocket &socket, char* dataString, byte frameLength) {
  
#ifdef DEBUG
  Serial.print("Got data: ");
  Serial.write((unsigned char*)dataString, frameLength);
  Serial.println();
#endif
  
  // Just echo back data for fun.
  socket.send(dataString, strlen(dataString));
}

void onDisconnect(WebSocket &socket) {
  Serial.println("onDisconnect called");
}

void setup() {
#ifdef DEBUG  
  Serial.begin(57600);
#endif
  Ethernet.begin(mac, ip);
  
  // declarando o ledMetano como OUTPUT:
  pinMode(ledMetano, OUTPUT);

  // declarando os ledcalor como OUTPUT:
  pinMode(ledcalormeso, OUTPUT);
  pinMode(ledcalortermo, OUTPUT);
  pinMode(ledcalorfrito, OUTPUT);

  // declarando os ledumidade como OUTPUT:
  pinMode(ledseco, OUTPUT);
  pinMode(ledmolhado, OUTPUT);

  wsServer.registerConnectCallback(&onConnect);
  wsServer.registerDataCallback(&onData);
  wsServer.registerDisconnectCallback(&onDisconnect);  
  wsServer.begin();
  
  delay(100); // Give Ethernet time to get ready
}

void loop() {
  // Should be called for each loop.
  wsServer.listen();
  
  // Do other stuff here, but don't hang or cause long delays.
  delay(100);
  if (wsServer.isConnected()) {
     // funçao Metano

     // lendo o valor do sensor:
     valorMetano =analogRead(metanoPino);   
     // liga o ledMetano

    // imprime o resultado no monitor serial:
  //Serial.print("Metano igual a: " );                       
  //Serial.println(valorMetano); 

  // dependendo do valorMetado acende ou apaga led limiteMetano
  if (valorMetano >= limiteMetano){
    digitalWrite(ledMetano, HIGH);
  }
  else{

    digitalWrite(ledMetano, LOW);
  }
  dtostrf(valorMetano, 2, 3, s);
  wsServer.send(s, 32);
  }
}
