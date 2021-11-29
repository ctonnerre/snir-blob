/*
  Lora Send And Receive
  This sketch demonstrates how to send and receive data with the MKR WAN 1300/1310 LoRa module.
  This example code is in the public domain.
*/

#include <MKRWAN.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
int sensorPin = A5;
Adafruit_BME280 bme;

LoRaModem modem;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independent of this setting, the modem will
  // not allow sending more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
}

//void loop() {
//  Serial.println();
//  Serial.println("Enter a message to send to network");
//  Serial.println("(make sure that end-of-line 'NL' is enabled)");

//  while (!Serial.available());
//  String msg = Serial.readStringUntil('\n');
//
//  Serial.println();
//  Serial.print("Sending: " + msg + " - ");
//  for (unsigned int i = 0; i < msg.length(); i++) {
//    Serial.print(msg[i] >> 4, HEX);
//    Serial.print(msg[i] & 0xF, HEX);
//    Serial.print(" ");
//  }
//  Serial.println();

void loop()
{
  // La valeur de la tension est mesurée
  int rawValue = analogRead(sensorPin);
  float voltage = rawValue * (3.3/1023) * 1000;
  float resitance = 10000 * ( voltage / ( 3300.0 - voltage) );
  float lux = 250000/resitance;


  int err;
  
  float valTemp = bme.readTemperature();
  String valTempString = String(valTemp);

  for (unsigned int i = 0; i < valTempString.length(); i++) {
      Serial.print(valTempString[i] >> 4, HEX);
      Serial.print(valTempString[i] & 0xF, HEX);
      Serial.print(" ");
    }
    
  String valLumString = String(lux);

  for (unsigned int i = 0; i < valLumString.length(); i++) {
      Serial.print(valLumString[i] >> 4, HEX);
      Serial.print(valLumString[i] & 0xF, HEX);
      Serial.print(" ");
  }

  float valHum = bme.readHumidity();
  String valHumString = String(valHum);

  for (unsigned int i = 0; i < valHumString.length(); i++) {
    Serial.print(valHumString[i] >> 4, HEX);
    Serial.print(valHumString[i] & 0xF, HEX);
    Serial.print(" ");
  }
  
  modem.beginPacket();
  modem.print(valTempString);
  modem.print(valHumString);
  modem.print(valLumString);
  
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
    Serial.println(valTempString);
    Serial.println(valHumString);
    Serial.println(valLumString);
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  delay(60000);
  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    return;
  }
  char rcv[64];
  int i = 0;
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.print("Received: ");
  for (unsigned int j = 0; j < i; j++) {
    Serial.print(rcv[j] >> 4, HEX);
    Serial.print(rcv[j] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
}
