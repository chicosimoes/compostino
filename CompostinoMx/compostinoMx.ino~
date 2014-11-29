/*
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
Modify for compostino Proyect by Manuel Perales
https://raw.githubusercontent.com/ruralkan/Compostino/master/
*/
#include <SPI.h>
#include <Ethernet.h>
#include <dht11.h>
#include <OneWire.h>
#include <SD.h>
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
const int updateThingSpeakInterval = 30 * 1000; // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)
// Variable communication Setup
long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter = 0;
// Initialize Arduino Ethernet Client
EthernetClient client;
//////////////////////////////////////////////////////////////////Internet communications//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Measure Variables/////////////////////////////////////////////////////////////////////////////////
//dht11 DHT11;
//int dht11pino = 2; // pino digital do sensor dht11
int metanoPino = A0; // escolha o pino que recebe o sinal do sensor de metano
int calorPino = 3; // escolha o pino que recebe o sinal do sensor de temperatura
int umidadePino = A2; // escolha o pino que recebe o sinal do sensor de umidade
OneWire ds(calorPino);
/////////////////////////////////////////////////////////////////Parameters to metano indicator////////////////////////////////////////////////////////////////////////////////
int ledMetano = 13; // escolha o pino que acendera o ledMetano
float valorMetano = 0; // guarda o valor medido no sensor MQ-4(metano)
float limiteMetano = 600; // valor definido como limiar maximo de metano
/////////////////////////////////////////////////////////////////Parameters to metano indicator////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Parameters to temperature indicator////////////////////////////////////////////////////////////////////////////////
int ledcalormeso = 10; //ascende pino na temperatura boa para bact mesofilicas
int ledcalortermo = 11; //ascende pino na temperatura boa para bact termofilicas
int ledcalorfrito = 12; //ascende pino quando temperatura muito alta
int valorcalor = 0; //guarda valor medido sensor LM35(temperatura)
float limitecalor = 70; //Limite maximo de temperatura
/////////////////////////////////////////////////////////////////Parameters to temperature indicator////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Parameters to humidity indicator////////////////////////////////////////////////////////////////////////////////
int ledseco = 8; //ascende pino quando umidade esta baixa. Precisa molhar composteira.
int ledmolhado = 9; //ascende pino quando umidade esta muito alta.
float valorumidade = 0; //guarda valor do sensor caseiro (umidade)
float umidademin = 300; //limite minimo da faixa ideal de umidade
float umidademax = 370; //limite maximo da faixa ideal de umidade
/////////////////////////////////////////////////////////////////Parameters to humidity indicator////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Measure Variables/////////////////////////////////////////////////////////////////////////////////
void setup()
{
/////////////////////////////////////////////////////////////////Output Indicator declarations///////////////////////////////////////////////////////////////////////////////
// declarando o ledMetano como OUTPUT:
pinMode(ledMetano, OUTPUT);
// declarando os ledcalor como OUTPUT:
pinMode(ledcalormeso, OUTPUT);
pinMode(ledcalortermo, OUTPUT);
pinMode(ledcalorfrito, OUTPUT);
// declarando os ledumidade como OUTPUT:
pinMode(ledseco, OUTPUT);
pinMode(ledmolhado, OUTPUT);
/////////////////////////////////////////////////////////////////Output Indicator declarations///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// Comunications parameters///////////////////////////////////////////////////////////////////////////////
// Start Serial for debugging on the Serial Monitor
Serial.begin(9600);
Serial.print("Initializing SD card...");
// make sure that the default chip select pin is set to
// output, even if you don't use it:
pinMode(53, OUTPUT);
// see if the card is present and can be initialized:
if (!SD.begin(chipSelect)) {
Serial.println("Card failed, or not present");
// don't do anything more:
return;
}
Serial.println("card initialized.");
// Start Ethernet on Arduino
startEthernet();
///////////////////////////////////////////////////////////////// Comunications parameters///////////////////////////////////////////////////////////////////////////////
}
void loop()
{
/////////////////////////////////////////////////////////////////Metano Inputs///////////////////////////////////////////////////////////////////////////////
// lendo o valor do sensor:
valorMetano =analogRead(metanoPino);
//Call Metano Indicator
indicatorMetano(valorMetano);
//Convert valorMetano to string for Internet transmition
String svalorMetano = String(analogRead(metanoPino), DEC);
/////////////////////////////////////////////////////////////////Metano Inputs///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Temperature Inputs///////////////////////////////////////////////////////////////////////////////
// funçao temperatura
//valorcalor =(5.0 * analogRead(calorPino) * 100.0) / 1024;
valorcalor =getTemp();
//Call Temperature Indicator
indicatorTemperature(valorcalor);
//Convert valorcalor to string for Internet transmition
String svalorcalor = String(valorcalor,DEC);
/////////////////////////////////////////////////////////////////Temperature Inputs///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////Humidity Inputs///////////////////////////////////////////////////////////////////////////////
// lendo o valor do sensor:
valorumidade =analogRead(umidadePino);
//Call humedity Indicator
indicatorUmidade(umidadePino);
String svalorumidade = String(analogRead(umidadePino),DEC);
/////////////////////////////////////////////////////////////////Humidity Inputs///////////////////////////////////////////////////////////////////////////////
//String para para escrever no cartão SD
String dataString = svalorMetano+","+svalorcalor+","+svalorumidade;
/////função para carregar data en sd card
/// sdcardload(dataString);
/////função para carregar data en thingspeak
thingSpeakLoad(svalorMetano,svalorcalor,svalorumidade);
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
}
else{
digitalWrite(ledMetano, LOW);
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
digitalWrite(ledcalormeso, HIGH);
digitalWrite(ledcalortermo, HIGH);
digitalWrite(ledcalorfrito, LOW);
}
// acende primeiro, segundo e terceiro ledCalor, se temperatura maior que 70
else if(valorcalor1>70){
digitalWrite(ledcalormeso, HIGH);
digitalWrite(ledcalortermo, HIGH);
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
}
// umidade maior que 370, significa que esta muito molhado, ledmolhado ligado:
if (valorumidade1 > umidademax){
digitalWrite(ledseco, LOW);
digitalWrite(ledmolhado, HIGH);
}
// umidade entre 40 e 370, todos os leds desligado:
if (valorumidade1 > umidademin && valorumidade < umidademax) {
digitalWrite(ledseco, LOW);
digitalWrite(ledmolhado, LOW);
}
//Serial Print humedad values
// Serial.print("Umidade do composto igual a: " );
// Serial.println(valorumidade1);
// funçao umidade do ar:
// int chk = DHT11.read(dht11pino);
// Serial.print("umidade do ar: ");
// Serial.println((float)DHT11.humidity, 2);
// Serial.print("Temperatura do ar: ");
// Serial.println((float)DHT11.temperature, 2);
// Serial.println ( );
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
Serial.print("valor de la temperatura:");
Serial.println(svalorcalor);
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
void sdcardload(String dataString){
// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
File dataFile = SD.open("datalog.txt", FILE_WRITE);
// if the file is available, write to it:
if (dataFile) {
dataFile.println(dataString);
dataFile.close();
// print to the serial port too:
Serial.println(dataString);
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

