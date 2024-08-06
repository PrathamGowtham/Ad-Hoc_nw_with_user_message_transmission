#include "Arduino.h"
#include "definitions.h"
#include <esp_now.h>
#include <WiFi.h>
#include "Secondary_Comm.h"
#include "Node_Data.h"

volatile bool rFlag = false; //flag that tracks when we get data
volatile bool sFlag = false; //flag that tracks successful data send
volatile bool inNetwork = false; //flag tracks whether node is on network or waiting to join
secondaryData sData; //struct variable for secondary data
primaryData pData; //struct to store primary data
Node_Data nData; //object for node data class

Secondary_Comm::Secondary_Comm() { } //class constructor

// callback function that executes as interrupt when data is rec'd
void dataReceived(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if(!inNetwork) { //if true not on network and packet should be mac address
    memcpy((uint8_t*)&pData,incomingData,sizeof(pData));
  }
  rFlag = true; //set rec data flag to true
}

// callback or interupt function that executes after sending data
void dataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status == ESP_NOW_SEND_SUCCESS) {
    sFlag = true; //set flag to true for successful data send
  }
}

//this function init the espnow communication and registers the callback functions
//if esp now init fails function returns false, otherwise it returns true
bool Secondary_Comm::initSecondary_Comm() {
  nData.initNodeData(); //init node data class
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    return false;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to
  esp_now_register_recv_cb(dataReceived); //register rec callback function
  esp_now_register_send_cb(dataSent); //register send callback function

  return true;
}

//creates ESPNow peer of primary node. Input argument is primary node mac address
//function returns false if it failed to create peer and true if peer was created
bool Secondary_Comm::createPrimaryPeer() {
   memcpy(primaryPeer.peer_addr,pData.pAddr,MAC_BYTE_LENGTH);
   primaryPeer.channel = 0;  
   primaryPeer.encrypt = false;
   
  if (esp_now_add_peer(&primaryPeer) != ESP_OK){
    return false;
  }
  return true; //creating ESPNow peer was successful
}

//this functions copies stored primary node MAC address to local array
void Secondary_Comm::copyPrimaryMACAddress(uint8_t* temp) { memcpy(temp,pData.pAddr, MAC_BYTE_LENGTH); }

//function to send data to primary node using ESP Now
bool Secondary_Comm::sendData2Primary() {
  sData.vReading = nData.getvoltageMeas(); //get voltage reading
  sData.digState = nData.getDigitalReading(); //get digital state
  esp_err_t errState = esp_now_send(pData.pAddr,(uint8_t *)&sData,sizeof(sData));
  if (errState != ESP_OK) {
    return false;
  }
  return true;
}

//returns the node number of this secondary node, node number was assigned by primary node
uint8_t Secondary_Comm::getMyNodeNumber() { return pData.nodeNumber; }

//returns rec'd data flag. flag is set to true when new data has been rec'd
bool Secondary_Comm::getRecDataFlag() { return rFlag; }

//clears or sets to false rec'd data flag
void Secondary_Comm::clearRecDataFlag() { rFlag = false; }

//returns send data flag. flag is set to true on successful data send
bool Secondary_Comm::getSendDataFlag() { return sFlag; }

//clears or sets to false send data success flag
void Secondary_Comm::clearSendDataFlag() { sFlag = false; }

//returns true if this secondary node is part of a network and false if not
bool Secondary_Comm::getNetworkState() { return inNetwork; }

//sets flag whether we are on network
//set to true when we join network otherwise set it to flase
//input argument is bool setting network state
void Secondary_Comm::setNetworkState(bool netState) { inNetwork = netState; }
