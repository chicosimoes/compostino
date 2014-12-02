/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  24 Nov 2010
 updated 2 Dec 2010
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <SD.h>


// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;


int calorPino = 1;  // escolha o pino que recebe o sinal do sensor de temperatura
OneWire ds(calorPino);

void setup()
{
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
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";

  // read three sensors and append to the string:
  int metano = analogRead(A0);
  int umidade= analogRead(A2);
  //int valorumidade=map(umidade, 1023, 0, 0, 100);
  //int valormetano=map(metano, 0, 1023, 0, 100);

  // funçao temperatura

  //valorcalor =(5.0 * analogRead(calorPino) * 100.0) / 1024;
  int valorcalor =getTemp();



  //Convert valorumidade to string for Internet transmition
  String svalorumidade = String(umidade,DEC); 
  //Convert valorcalor to string for Internet transmition
  String svalorcalor = String(valorcalor,DEC);
  //Convert valorMetano to string for Internet transmition
  String svalormetano = String(metano, DEC);




  //String para para escrever no cartão SD
  dataString = svalorumidade+","+svalorcalor+","+svalormetano;  

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    Serial.println(dataString);
  } 
}


/////////////////////////////////////////////////Get temperature values//////////////////////////////////////////////////////////////
int getTemp(){

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









