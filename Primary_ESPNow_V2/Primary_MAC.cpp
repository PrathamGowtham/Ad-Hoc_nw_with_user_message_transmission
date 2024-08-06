#include "Primary_MAC.h"
#include <WiFi.h>
#include "definitions.h"

Primary_MAC::Primary_MAC() { } //class constructor

//get mac address of the ESP32 code is running on and stores it
//returns MAC address in string format
String Primary_MAC::initGetMAC() {
  setMACArray2Zeros(); //set secondary mac address array to all zeros
  String s = WiFi.macAddress(); //get mac address of the ESP32 as string
  char mAddr[MAC_LENGTH]; //char array to hold mac address
  s.toCharArray(mAddr,MAC_LENGTH); //convert string to char array
  convertMACString2MACAddress(mAddr,myMAC); //convert char array to array of bytes and store in global
  return s;
}

//gets mac address in byte form from global private byte array and copies MAC address to another byte array
//input argument is byte array or byte pointer where MAC address will be copied to
void Primary_MAC::getMyMACAddress(uint8_t* mAddr) { 
  memcpy (mAddr,myMAC,MAC_BYTE_LENGTH);
}

//gets mac address in byte form of any secondary node. 
//Input argument is pointer to byte array and position in 2D of address
void Primary_MAC::getSecondaryMACAddress(uint8_t* mAddr, uint8_t sPos) {
  for(int j=0; j<MAC_BYTE_LENGTH; j++) { //get secondary node mac address
    mAddr[j] = secondaryMAC[sPos][j]; 
  }
}

//takes mac address string and converts it to a byte array of the mac address
//input arguments are string of mac address and bArray (pointer) to store mac
void Primary_MAC::getEnteredMACByteArray(String m, uint8_t* bArray) {
  char sMAddr[MAC_LENGTH]; //char array to hold mac address
  m.toCharArray(sMAddr,MAC_LENGTH); //convert string to char array
  convertMACString2MACAddress(sMAddr, bArray); //convert char array to array of bytes
}

//takes MAC address string of secondary node, converts to byte array, and stores in secondary node mac address array
//input argument is mac address as string, returns array slot MAC address was stored, if array is full returns MAX_SECONDARY_NODES + 1
void Primary_MAC::storeSecondaryMAC(String s, uint8_t arrPos) {
  char sMAddr[MAC_LENGTH]; //char array to hold mac address
  s.toCharArray(sMAddr,MAC_LENGTH); //convert string to char array
  uint8_t aTemp[MAC_BYTE_LENGTH]; //temp array to hold new mac address
  convertMACString2MACAddress(sMAddr, aTemp); //convert char array to array of bytes
  for(int j=0; j<MAC_BYTE_LENGTH; j++) { //transfer 1D array to 2D array
     secondaryMAC[arrPos][j] = aTemp[j]; 
  }
}

//takes MAC address string of secondary node, converts to byte array, and deletes from secondary array
//input argument is mac address as string, returns array slot MAC address was erased, if mac address not found returns MAX_SECONDARY_NODES + 1
uint8_t Primary_MAC::deleteSecondaryMAC(String s, uint8_t* gAddr) {
  char sMAddr[MAC_LENGTH]; //char array to hold mac address
  s.toCharArray(sMAddr,MAC_LENGTH); //convert string to char array
  //uint8_t aTemp[MAC_BYTE_LENGTH]; //temp array to hold new mac address
  convertMACString2MACAddress(sMAddr, gAddr); //convert char array to array of bytes
  //gAddr = aTemp; //copy mac address before we delete it
  return  deleteAddrSecondaryArray(gAddr); //take temp byte array and deletes it from secondary mac address array position 0
}

//copies 1D array of MAC address to 2D array of secondary node mac addresses
//input argument is mac address as array of bytes
uint8_t Primary_MAC::getNextSecondaryNodeNumber() {
  bool done = false; 
  uint8_t cnt = 0;
  while(!done && cnt < MAX_SECONDARY_NODES) { //purpose of loop is to find first open spot in secondary node mac array
    if(!secondaryMAC[cnt][0] && !secondaryMAC[cnt][1] && !secondaryMAC[cnt][2] && !secondaryMAC[cnt][3] && !secondaryMAC[cnt][4] && !secondaryMAC[cnt][5]) {
      done = true; //we found an open address slot so we are done
    }
    cnt++;
  }

  if(!done) return (MAX_SECONDARY_NODES + 1); //return number higher than array count if no empty slots
  else return cnt; //return array slot that mac address was stored in
}

//deletes input 1D array of MAC address from 2D array of secondary node mac addresses
//input argument is mac address as array of bytes, returns position was deleted or if mac address not found returns MAX_SECONDARY_NODES + 1
uint8_t Primary_MAC::deleteAddrSecondaryArray(uint8_t* sAddr) {
  bool done = false; 
  uint8_t cnt = 0;
  while(!done && cnt < MAX_SECONDARY_NODES) { //purpose of loop is to find first open spot in secondary node mac array
    if(secondaryMAC[cnt][0] == sAddr[0] && secondaryMAC[cnt][1] == sAddr[1] && secondaryMAC[cnt][2] == sAddr[2] && secondaryMAC[cnt][3] == sAddr[3] && secondaryMAC[cnt][4] == sAddr[4] && secondaryMAC[cnt][5] == sAddr[5]) {
      for(int j=0; j<MAC_BYTE_LENGTH; j++) { //transfer 1D array to 2D array
        secondaryMAC[cnt][j] = false; 
      }
      done = true; //we found an open address slot so we are done
    }
    cnt++;
  }

  if(!done) return (MAX_SECONDARY_NODES + 1); //return number higher than array count if no empty slots
  else return cnt; //return array slot that mac address was stored in
}

//function converts secondary mac address to its corresponding node number
//input argument is mac address as byte array, returns node number
uint8_t Primary_MAC::convertSecondaryAddr2NodeNumber(uint8_t* sAddr) {
  bool done = false; 
  uint8_t cnt = 0;
  while(!done && cnt < MAX_SECONDARY_NODES) { //purpose of loop is to find first open spot in secondary node mac array
    if(secondaryMAC[cnt][0] == sAddr[0] && secondaryMAC[cnt][1] == sAddr[1] && secondaryMAC[cnt][2] == sAddr[2] && secondaryMAC[cnt][3] == sAddr[3] && secondaryMAC[cnt][4] == sAddr[4] && secondaryMAC[cnt][5] == sAddr[5]) {
      done = true; //we found an open address slot so we are done
    }
    cnt++;
  }
  if(!done) return (MAX_SECONDARY_NODES + 1); //return number higher than array count if no empty slots
  else return cnt; //return array slot that mac address was stored in
}

//returns true if there is a secondary node online for a given node number, returns false otherwise
//input argument is the secondary node number
bool Primary_MAC::secondaryNodeIsOnline(uint8_t aNum) {
  uint8_t aPos = aNum; //convert node num to array spot
  if(!secondaryMAC[aPos][0] && !secondaryMAC[aPos][1] && !secondaryMAC[aPos][2] && !secondaryMAC[aPos][3] && !secondaryMAC[aPos][4] && !secondaryMAC[aPos][5]) return false;
  return true;
}

//function converts MAC address string to mac address in ints
void Primary_MAC::convertMACString2MACAddress(char* mChar, uint8_t* macAddr) {
  uint8_t mArr[MAC_LENGTH];
  uint8_t cnt = 0;
  for(uint8_t i=0; i<18; i++) { //convert char array to byte array
    if(i != 2 || i != 5 || i != 8 || i != 11 || i != 14) { //only convert the actual mac address bytes, skip the ':"
      mArr[i] = (uint8_t)mChar[i];
      if(mArr[i] < 65) { //we are a dec number
        mArr[i] = mArr[i] - 48; //number from 0 to 9
      }
      else { //we are an A,B,C,D,E,F hex number
        mArr[i] = mArr[i] - 55; //number from 0 to 9
      }
    }
    if(i == 1 || i == 4 || i == 7 || i == 10 || i == 13 || i == 16) { //add up the individual digits into bytes
      macAddr[cnt] = (mArr[i-1]*16) + mArr[i];
      cnt++;
    }
  }
}

//function intializes all secondary MAC address array slots to zero
void Primary_MAC::setMACArray2Zeros() {
  for(uint8_t n=0; n<MAX_SECONDARY_NODES; n++) {
    for(uint8_t j=0; j<MAC_BYTE_LENGTH; j++) {
      secondaryMAC[n][j] = 0; //initialize all address slots to 0
    }
  }
}
