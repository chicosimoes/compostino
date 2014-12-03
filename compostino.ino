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

Arduino Requirements:
* Arduino with Ethernet Shield or Arduino Ethernet
* Arduino 1.0 IDE
Network Requirements:
* Ethernet port on Router
* DHCP enabled on Router
* Unique MAC Address for Arduino
* SDCARD
Created: november 22, 2014 by Manuel Mejia Perales (https://github.com/ruralkan)
Additional Credits:
Example sketches from Arduino team, Ethernet by Adrian McEwen
Example sketches from thingspeak team by Hans Scharler (http://www.iamshadowlord.com)
Example sketches from Arduino team, SDcard by Tom Igoe
created 18 Dec 2009
 by David A. Mellis
 
 modified 19 Mar 2012
 by Sam Fentress

 modified 14 Oct 2014
 by Chico Simões & Vilson Vieira

 modified 23 Nov - 6 Dec 2014 for compostino Proyect by Manuel Perales
 https://raw.githubusercontent.com/ruralkan/Compostino/master/
*/
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <SD.h>
float j=0;
int tenTot1;
int tenTot2;
int tenTot3;
String dataString;
//////////////////////////////////////////////////////////////////Internet communications//////////////////////////////////////////////////////////////////////////
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;
// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network
// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "FM637ENN77ZMTW4C";
const int updateThingSpeakInterval = 20*1000; // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)
// Variable communication Setup
long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter = 0;
// Initialize Arduino Ethernet Client
EthernetClient client;
//////////////////////////////////////////////////////////////////Internet communications//////////////////////////////////////////////////////////////////////////
/////////////////////////////////Recordar///////////////////////////////////
/*SD card datalogger
This example shows how to log data from three analog sensors
to an SD card using the SD library.
The circuit:
* analog sensors on analog ins 0, 1, and 2
* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 4
** Record - pin 10
*/
/////////////////////////////////////////////////////////////////Measure Variables/////////////////////////////////////////////////////////////////////////////////
int metanoPino = A0; // escolha o pino que recebe o sinal do sensor de metano
int calorPino = 31; // escolha o pino que recebe o sinal do sensor de temperatura
int umidadePino = A2; // escolha o pino que recebe o sinal do sensor de umidade
OneWire ds(calorPino);
/////////////////////////////////////////////////////////////////Parameters to metano indicator////////////////////////////////////////////////////////////////////////////////
int ledMetano = 33; // escolha o pino que acendera o ledMetano
int ledMetanook =32; //ausencia metano
int valorMetano = 0; // guarda o valor medido no sensor MQ-4(metano)
int ADCvalorMetano=0;
int valorMetanom=0;
int limiteMetano = 30; // valor definido como limiar maximo de metano
/////////////////////////////////////////////////////////////////Parameters to metano indicator////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Parameters to temperature indicator////////////////////////////////////////////////////////////////////////////////
int ledcalormeso = 46; //ascende pino na temperatura boa para bact mesofilicas
int ledcalortermo = 47; //ascende pino na temperatura boa para bact termofilicas
int ledcalorfrito = 48; //ascende pino quando temperatura muito alta
int valorcalor = 0; //guarda valor medido sensor LM35(temperatura)
int valorcalorm=0;
int limitecalor = 70; //Limite maximo de temperatura
/////////////////////////////////////////////////////////////////Parameters to temperature indicator////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Parameters to humidity indicator////////////////////////////////////////////////////////////////////////////////
int ledseco= 42;
int ledok = 43; //ascende pino quando umidade esta baixa. Precisa molhar composteira.
int ledmolhado = 45; //ascende pino quando umidade esta muito alta.
int valorumidade = 0; //guarda valor do sensor caseiro (umidade)
int ADCvalorumidade=0;
int valorumidadem=0;
int umidademin = 40; //limite minimo da faixa ideal de umidade
int umidademax = 60; //limite maximo da faixa ideal de umidade
/////////////////////////////////////////////////////////////////Parameters to humidity indicator////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Measure Variables/////////////////////////////////////////////////////////////////////////////////
void setup()
{
/////////////////////////////////////////////////////////////////Output Indicator declarations///////////////////////////////////////////////////////////////////////////////
// declarando o ledMetano como OUTPUT:
pinMode(ledMetano, OUTPUT);
pinMode(ledMetanook,OUTPUT);
// declarando os ledcalor como OUTPUT:
pinMode(ledcalormeso, OUTPUT);
pinMode(ledcalortermo, OUTPUT);
pinMode(ledcalorfrito, OUTPUT);
// declarando os ledumidade como OUTPUT:
pinMode(ledseco, OUTPUT);
pinMode(ledok, OUTPUT);
pinMode(ledmolhado, OUTPUT);
/////////////////////////////////////////////////////////////////Output Indicator declarations///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// Comunications parameters///////////////////////////////////////////////////////////////////////////////
// Start Serial for debugging on the Serial Monitor
Serial.begin(9600);
Serial.print("Initializing SD card...");
// make sure that the default chip select pin is set to
// output, even if you don't use it:
pinMode(10, OUTPUT);
digitalWrite(10, HIGH); // Add this line
// see if the card is present and can be initialized:
if (!SD.begin(chipSelect)) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
Serial.println("card initialized.");
// Start Ethernet on Arduino
// startEthernet();
///////////////////////////////////////////////////////////////// Comunications parameters///////////////////////////////////////////////////////////////////////////////
}
void loop()
{
/////////////////////////////////////////////////////////////////Inputs///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Metano Inputs///////////////////////////////////////////////////////////////////////////////
// lendo o valor do sensor:
ADCvalorMetano =analogRead(metanoPino);
/////////////////////////////////////////////////////////////////Temperature///////////////////////////////////////////////////////////////////////////////
// funçao temperatura
//valorcalor =(5.0 * analogRead(calorPino) * 100.0) / 1024;
valorcalor =getTemp();
/////////////////////////////////////////////////////////////////Humidity ///////////////////////////////////////////////////////////////////////////////
// lendo o valor do sensor:
ADCvalorumidade =analogRead(umidadePino);
/////////////////////////////////////////////////////////////////Inputs///////////////////////////////////////////////////////////////////////////////
//Conversion ADC a Escala y sampling
media(ADCvalorumidade,valorcalor,ADCvalorMetano);
valorumidade=map(ADCvalorumidade, 0, 1023, 0, 100);
valorMetano=map(ADCvalorMetano, 0, 1023, 0, 100);
valorcalor=valorcalorm;
////////////////////////////////////////////////Create messages to send and indicators////////////////////
//Convert valorumidade to string for Internet transmition
String svalorumidade = String(valorumidade,DEC);
//Convert valorcalor to string for Internet transmition
String svalorcalor = String(valorcalor,DEC);
//Convert valorMetano to string for Internet transmition
String svalorMetano = String(valorMetano, DEC);
//Call humedity Indicator
indicatorUmidade(valorumidade);
//Call Temperature Indicator
indicatorTemperature(valorcalor);
//Call Metano Indicator
indicatorMetano(valorMetano);
////////////////////////////////////////////////Create messages to send////////////////////
//String para para escrever no cartão SD
dataString = svalorumidade+","+svalorcalor+","+svalorMetano;
Serial.print("valor de la umidade:");
Serial.println(svalorumidade);
Serial.print("valor de la temperatura:");
Serial.println(svalorcalor);
Serial.print("valor de la metano:");
Serial.println(svalorMetano);
/////função para carregar data en sd card
sdcardload(dataString);
/////função para carregar data en thingspeak
thingSpeakLoad(svalorumidade,svalorcalor,svalorMetano);
}
////////////////////////////////iniciar sessão internet////////////////////////////////////
void startEthernet()
{
client.stop();
Serial.println("Connecting Arduino to network...");
Serial.println();
delay(1000);
// Connect to network amd obtain an IP address using DHCP
if (Ethernet.begin(mac) == 0)
{
Serial.println("DHCP Failed, reset Arduino to try again");
Serial.println();
}
else
{
Serial.println("Arduino connected to network using DHCP");
Serial.println();
}
delay(1000);
}
/////////////////////////////////////////////////////////////////////Metano Indicator//////////////////////////////////////////////////////////////
void indicatorMetano(float valorMetano1){
// imprime o resultado no monitor serial:
// Serial.print("Metano igual a: " );
// Serial.println(valorMetano1);
// dependendo do valorMetado acende ou apaga led limiteMetano
if (valorMetano1 >= limiteMetano){
digitalWrite(ledMetano, HIGH);
digitalWrite(ledMetanook, LOW);
}
else{
digitalWrite(ledMetano, LOW);
digitalWrite(ledMetanook, HIGH);
}
}
/////////////////////////////////////////////////Get temperature values//////////////////////////////////////////////////////////////
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
int Temperature = int(TRead / 16);
return Temperature;
}
//////////////////////////////////////////////////Temperature Indicator//////////////////////////////////////////////////////////////
void indicatorTemperature(float valorcalor1){
// acender o primeiro ledCalor, se temperatura maior que 35
if((valorcalor1>35) && (valorcalor1<50)){
digitalWrite(ledcalormeso, HIGH);
digitalWrite(ledcalortermo, LOW);
digitalWrite(ledcalorfrito, LOW);
}
// acender primeiro e segundo ledCalor, se temperatura maior que 50
else if((valorcalor1>=50)&&(valorcalor1<70)){
digitalWrite(ledcalormeso, LOW);
digitalWrite(ledcalortermo, HIGH);
digitalWrite(ledcalorfrito, LOW);
}
// acende primeiro, segundo e terceiro ledCalor, se temperatura maior que 70
else if(valorcalor1>70){
digitalWrite(ledcalormeso, LOW);
digitalWrite(ledcalortermo, LOW);
digitalWrite(ledcalorfrito, HIGH);
}
// se temperatura menor que 35, primeiro, segundo e terceiro ledcalor desligado
else if(valorcalor1<35){
digitalWrite(ledcalormeso, LOW);
digitalWrite(ledcalortermo, LOW);
digitalWrite(ledcalorfrito, LOW);
}
// imprime resultados no monitor serial:
//Serial.print("A temperatura igual a:");
//Serial.println(valorcalor1);
}
/////////////////////////////////////////Umidade Indicator//////////////////////////////////////////////////////////////
void indicatorUmidade(float valorumidade1){
// umidade menor que 40, significa que esta muito seco, ledseco ligado:
if (valorumidade1 < umidademin) {
digitalWrite(ledseco, HIGH);
digitalWrite(ledmolhado, LOW);
digitalWrite(ledok, LOW);
}
// umidade maior que 60, significa que esta muito molhado, ledmolhado ligado:
if (valorumidade1 > umidademax){
digitalWrite(ledseco, LOW);
digitalWrite(ledok, LOW);
digitalWrite(ledmolhado, HIGH);
}
// umidade entre 40 e 60, todos os leds desligado:
if (valorumidade1 > umidademin && valorumidade < umidademax) {
digitalWrite(ledok, HIGH);
digitalWrite(ledmolhado, LOW);
digitalWrite(ledok, LOW);
}
}
///////////////////////////////////Enviar valores a Thingspeak.com////////////////
void thingSpeakLoad(String svalorumidade,String svalorcalor,String svalorMetano){
// Print Update Response to Serial Monitor
if (client.available())
{
char c = client.read();
Serial.print(c);
}
// Disconnect from ThingSpeak
if (!client.connected() && lastConnected)
{
Serial.println("...disconnected");
Serial.println();
client.stop();
}
// Update ThingSpeak
if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
{
updateThingSpeak("field1="+svalorumidade+"&field2="+svalorcalor+"&field3="+svalorMetano);
}
// Check if Arduino Ethernet needs to be restarted
if (failedCounter > 3 ) {startEthernet();}
lastConnected = client.connected();
}
void updateThingSpeak(String tsData)
{
if (client.connect(thingSpeakAddress, 80))
{
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(tsData.length());
client.print("\n\n");
client.print(tsData);
lastConnectionTime = millis();
if (client.connected())
{
Serial.println("Connecting to ThingSpeak...");
Serial.println();
failedCounter = 0;
}
else
{
failedCounter++;
Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");
Serial.println();
}
}
else
{
failedCounter++;
Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");
Serial.println();
lastConnectionTime = millis();
}
}
//////////////////////////////////////Enviar data a SDCARD////////////////////////
void sdcardload(String dataString1){
// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
File dataFile = SD.open("datalog.txt", FILE_WRITE);
// if the file is available, write to it:
if (dataFile) {
dataFile.println(dataString1);
dataFile.close();
// print to the serial port too:
Serial.println(dataString1);
// imprime o resultado no monitor serial:
Serial.print("Metano igual a: " );
Serial.println(valorMetano);
// imprime resultados no monitor serial:
Serial.print("A temperatura igual a:");
Serial.println(valorcalor);
//Serial Print humedad values
Serial.print("Umidade do composto igual a: " );
Serial.println(valorumidade);
}
// if the file isn't open, pop up an error:
else {
Serial.println("error opening datalog.txt");
}
}
void media(int ADCvalorumidade, int valorcalor1, int ADCvalorMetano){
if(millis() - lastConnectionTime < updateThingSpeakInterval){
j+=1;
tenTot1 = tenTot1 + ADCvalorumidade; //add up readings
tenTot2 = tenTot2 + valorcalor1; //add up readings
tenTot3 = tenTot3 + ADCvalorMetano;
valorumidadem= (tenTot1 /j ); //divide the total
valorcalorm= (tenTot2 /j ); //divide the total
valorMetanom= (tenTot3/j ); //divide the total
}
if(millis() - lastConnectionTime > updateThingSpeakInterval){
tenTot1 = 0; //reset total variable
tenTot2 = 0; //reset total variable
tenTot3 = 0; //reset total variable
j=0;
}
}
