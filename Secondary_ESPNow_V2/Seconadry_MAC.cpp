#include "Seconadry_MAC.h"
#include <WiFi.h>
#include "definitions.h"

Seconadry_MAC::Seconadry_MAC() { } //class constructor

//get mac address of the ESP32 code is running on and stores it
//returns MAC address in string format
String Seconadry_MAC::initGetMAC() {
  String s = WiFi.macAddress(); //get mac address of the ESP32 as string
  char mAddr[MAC_LENGTH]; //char array to hold mac address
  s.toCharArray(mAddr,MAC_LENGTH); //convert string to char array
  convertMACString2MACAddress(mAddr,myMAC); //convert char array to array of bytes and store in global
  return s;
}

//gets mac address in byte form from global private byte array and copies MAC address to another byte array
//input argument is byte array or byte pointer where MAC address will be copied to
void Seconadry_MAC::getMyMACAddress(uint8_t* mAddr) { 
  memcpy (mAddr,myMAC,MAC_BYTE_LENGTH);
}

//function converts MAC address string to mac address in ints
void Seconadry_MAC::convertMACString2MACAddress(char* mChar, uint8_t* macAddr) {
  uint8_t mArr[MAC_LENGTH];
  uint8_t cnt = 0;
  for(uint8_t i=0; i<18; i++) { //convert char array to byte array
    if(i != 2 || i != 5 || i != 8 || i != 11 || i != 14) { //only convert the actual mac address bytes, skip the ':"
      mArr[i] = (uint8_t)mChar[i];
      if(mArr[i] < 65) { //we are a dec number
        mArr[i] = mArr[i] - 48; //number from 0 to 9
      }
      else { //we are an A,B,C,D,E,F hex number
        mArr[i] = mArr[i] - 55; 
      }
    }
    if(i == 1 || i == 4 || i == 7 || i == 10 || i == 13 || i == 16) { //add up the individual digits into bytes
      macAddr[cnt] = (mArr[i-1]*16) + mArr[i];
      cnt++;
    }
  }
}
