#include "Primary_Comm.h"
#include <esp_now.h>
#include <WiFi.h>

volatile bool rFlag = false; //flag that tracks when we get data
volatile bool sFlag = false; //flag that tracks successful data send
volatile bool sCompleteFlag = false; //flag for tracking when send call back has executed
uint8_t recAddr[MAC_BYTE_LENGTH]; //array to hold address from secondary node, gets address from rec data call back
secondaryData sData; //struct variable to capture data from secondary node

//use constuctor to set mac address
Primary_Comm::Primary_Comm() { }

// callback or interupt function that executes after sending data
void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status == ESP_NOW_SEND_SUCCESS) {
    sFlag = true; //set flag to true for successful data send
  }
  sCompleteFlag = true; //tracks when send callback has completed
}

// callback function that will be executed when data is received
void dataReceived(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&sData, incomingData, sizeof(sData)); //copy rec data to variable
  memcpy(recAddr,mac,MAC_BYTE_LENGTH); //copy mac address of data sender
  rFlag = true; //set rec data flag to true
}

//this function init the espnow communication and registers the callback functions
//if esp now init fails function returns false, otherwise it returns true
bool Primary_Comm::initPrimary_Comm() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) { 
    return false;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to
  esp_now_register_recv_cb(dataReceived);
  esp_now_register_send_cb(dataSent); //get the status of Trasnmitted packet
 
  return true;
}

//creates ESPNow peer for adding secondary, input is array of bytes of secondary mac address
//returns true if peer was added successfully or false if not
bool Primary_Comm::createSecondaryPeer(uint8_t* sAddr) {
  esp_now_peer_info_t secondayPeer; //create peer variable to add new secondary
  memset(&secondayPeer, 0, sizeof(secondayPeer)); //apparently you have to initialize peer info struct???
  memcpy(secondayPeer.peer_addr,sAddr,MAC_BYTE_LENGTH);
  secondayPeer.channel = 0;  //sets channel to talk to peer 
  secondayPeer.encrypt = false; //not using encryption
   
  bool worked = false;
  esp_err_t resp = esp_now_add_peer(&secondayPeer);
  switch (resp) {
    case ESP_OK:
        worked = true;
      break;
    case ESP_ERR_ESPNOW_NOT_INIT :
      _SERIAL_PRINTLN("Error: ESP-Now not initialized");
      break;
    case ESP_ERR_ESPNOW_ARG:
      _SERIAL_PRINTLN("Error: Invalid argument");
      break;
    case ESP_ERR_ESPNOW_FULL:
      _SERIAL_PRINTLN("Error: Network is full");
      break;
    case ESP_ERR_ESPNOW_NO_MEM:
      _SERIAL_PRINTLN("Error: Out of memory");
      break;
    case ESP_ERR_ESPNOW_EXIST:
      _SERIAL_PRINTLN("Error: Peer does not exist");
      break;
    default:
      _SERIAL_PRINTLN("Unknown error");
      break;
  }
  return worked;
}

//deletes ESP Now secondary node peer, input argument is array of bytes
//made up of secondary peer mac address, returns true if succussfully deletec
//otherwise it returns false
bool Primary_Comm::deleteSecondaryPeer(uint8_t* sAddr) {
  esp_now_peer_info_t secondayPeer; //create peer variable to add new secondary
  memset(&secondayPeer, 0, sizeof(secondayPeer)); //apparently you have to initialize peer info struct???
  memcpy(secondayPeer.peer_addr,sAddr,MAC_BYTE_LENGTH);
   
  bool worked = false;
  esp_err_t resp = esp_now_del_peer(sAddr);
  switch (resp) {
    case ESP_OK:
        worked = true;
      break;
    case ESP_ERR_ESPNOW_NOT_INIT :
      _SERIAL_PRINTLN("Error: ESP-Now not initialized");
      break;
    case ESP_ERR_ESPNOW_ARG:
      _SERIAL_PRINTLN("Error: Invalid argument");
      break;
    case ESP_ERR_ESPNOW_NOT_FOUND:
      _SERIAL_PRINTLN("Error: Peer not found");
      break;
    default:
      _SERIAL_PRINTLN("Unknown error");
      break;
  }
  return worked;
}

//returns number of peers, if error condition returns -1
int Primary_Comm::getPeerNumber() {
  esp_now_peer_num peerCnt;
  if(esp_now_get_peer_num(&peerCnt) != ESP_OK) return -1; //return negative number to signal error
  return  peerCnt.total_num;
}

//sends mac address to secondary node so they can join network and communicate
//Inputs are secondary max and primary max in byte form
bool Primary_Comm::sendPrimaryMACAddress(uint8_t* sMAC, uint8_t* pMAC, uint8_t nNum) {
  struct primaryData { //create struct to hold address and secondary node number
    uint8_t pAddr[MAC_BYTE_LENGTH];
    uint8_t nodeNumber;
  } primData;
  primData.nodeNumber = nNum; //set secondary node number
  memcpy(primData.pAddr,pMAC,MAC_BYTE_LENGTH); //copy master mac address to send to secondary node
  esp_err_t errState = esp_now_send(sMAC, (uint8_t *)&primData, sizeof(primData));

  return espNowSendErrorStatus(errState); //if there is an error evaluate and print message
}

//sends mac address to secondary node so they can join network and communicate
//Inputs are secondary max and primary max in byte form
bool Primary_Comm::sendPrimaryDataRequest(uint8_t* sMAC) {
  uint8_t dRequest = 1;
  esp_err_t errState = esp_now_send(sMAC, &dRequest, sizeof(dRequest));
    
  return espNowSendErrorStatus(errState); //if there is an error evaluate and print message
}

//evaluates whether there was an error or not for ESPNow send function
//if there is an error this function will print out the error type
//returns true if no error and false if there is an error
//input argument is espnow error variable
bool Primary_Comm::espNowSendErrorStatus(esp_err_t errType) {
  bool worked = false; //return variable
  switch ( errType) {
    case ESP_OK:
        worked = true;
      break;
    case ESP_ERR_ESPNOW_NOT_INIT :
      _SERIAL_PRINTLN("Error: ESP-Now not initialized");
      break;
    case ESP_ERR_ESPNOW_ARG:
      _SERIAL_PRINTLN("Error: Invalid argument");
      break;
    case ESP_ERR_ESPNOW_INTERNAL:
      _SERIAL_PRINTLN("Error: Internal issue");
      break;
    case ESP_ERR_ESPNOW_NO_MEM:
      _SERIAL_PRINTLN("Error: Out of memory");
      break;
    case ESP_ERR_ESPNOW_NOT_FOUND:
      _SERIAL_PRINTLN("Error: Peer not found");
      break;
    case ESP_ERR_ESPNOW_IF:
      _SERIAL_PRINTLN("Error: WiFi interface doesn’t match that of peer");
      break;
    default:
      _SERIAL_PRINTLN("Unknown error");
      break;
  }
  return worked;
}

//returns rec'd data flag. flag is set to true when new data has been rec'd
bool Primary_Comm::getRecDataFlag() { return rFlag; }

//clears or sets to false rec'd data flag
void Primary_Comm::clearRecDataFlag() { rFlag = false; }

//returns send data flag. flag is set to true on successful data send
bool Primary_Comm::getSendDataFlag() { return sFlag; }

//blocks until send call back function has completed
void Primary_Comm::waitForSend2Complete() {
  while(!sCompleteFlag);
}

//clears send successful flag and send complete flag
void Primary_Comm::clearSendFlags() { 
  sFlag = false; 
  sCompleteFlag = false;
}

//returns byte of data rec from secondary node
secondaryData Primary_Comm::getSecondaryData() { return sData; }

//copies mac address of secondary node that just sent data, note address is captured in rec callback function
//Input is byte array to copy address to
void Primary_Comm::recSecondaryNodeMACAddress(uint8_t* rAddr) { memcpy(rAddr,recAddr,MAC_BYTE_LENGTH); } //copy mac address of data sender
