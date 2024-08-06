/* Example sketch for ESP32 that uses the ESP-Now communication protocol to create a dynamic wireless network
 * This sketch was used in a YouTube video entitled: Using ESP-Now to Create a Dynamic Wireless ESP32 Device Network Part 3
 * This sketch was created by ForceTronics and is free to use and modify at your own risk
 * ESP Now documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html#_CPPv416esp_now_peer_num
 */
#include "Primary_MAC.h"
#include "definitions.h"
#include "Primary_Comm.h"

Primary_MAC mHandler; //mac handler class object
Primary_Comm nowComm; //primary esp-now comm class object
secondaryData nodeData; //struct variable to hold secondary node data
uint32_t dataTimer = 0; //timer variable used for collecting data from secondary nodes

void setup() {
  //call function to init ESPNow comm and make sure no error condition, also contains 5 sec delay to start serial term
  initializeESPNowComm(); 
  //call function to init mac handler class, get and store primary mac address, optionally print mac address
  initializeMACAddress();
  //print out instructions for the menu to add or delete secondary nodes from wireless network
  _SERIAL_PRINTLN("Enter an 'A' to add a secondary peer to the network and enter a 'D' to delete a secondary peer from the network");
  _SERIAL_PRINTLN("You can enter these menu items to add or delete nodes at any time");
      Serial.begin(115200);
    // ... existing setup code
    // Register receive callback
    esp_now_register_recv_cb(OnDataRecv);
  
  dataTimer = millis(); //initiate data collection timer variable
}

void loop() {
  //function handels menu to add or delete secondary nodes from network, function blocks until complete
  handleSecondaryNodeAddDelete();

  //function that is timer based that requests data from all secondary nodes online
  dataRequestTimer4SecondaryNodes();
  
  //function executes when new data from secondary node has beer received, prints out the received data and node it came from
  handleDataFromSecondaryNode();
}

//handles functions and logic for initializing ESP-Now communication
void initializeESPNowComm() {
  bool espInit = nowComm.initPrimary_Comm(); //start ESP now communication
  delay(5000); //delay to get serial monitor open
  _SERIAL_BEGIN(115200);
  if(!espInit) {
    _SERIAL_PRINTLN("ESP Now communication failed to initialize, check your setup and restart program");
    while(1); //loop forever
  }
  else { //ESP Now intialized successfully
    _SERIAL_PRINTLN("Primary node ESP-Now communication initialized");
  }
}

//Inits mac address handling class, stores primary mac, prints out mac address (optional)
void initializeMACAddress() {
  pinMode(MAC_PIN,INPUT_PULLUP); //setup digital pin to read to print MAC address
  String s = mHandler.initGetMAC(); //get mac address of this ESP32 as string
  uint8_t pTemp[MAC_BYTE_LENGTH]; //byte array to store MAC address locally
  mHandler.getMyMACAddress(pTemp); //copies stored MAC address to local array
    
  if(!digitalRead(MAC_PIN)) { //if digital pin is actively pulled low, get and print MAC Address
    _SERIAL_PRINTLN("Printing my MAC address:"); //print MAC address out as bytes of data in hex format
    serialPrintMAC(pTemp); //function that prints mac address in bytes to serial terminal
  } 
}

//this function handles adding a deleting secondary nodes from the network
//once you enter menu using 'A' or 'D' this function blocks until you finish 
//adding or deleting node. function resets data collection timer
void handleSecondaryNodeAddDelete() {
  if(_SERIAL_AVAILABLE()) { //if serial comm is available
    String s = _SERIAL_READSTRING();  //using string to read all data in serial buffer, include \n or \r
    s.trim();                        // remove any \r \n whitespace at the end of the String
    if(s == "a" || s == "A") {
      _SERIAL_PRINTLN("To add a secondary node to your ESP-Now dynamic network by entering its MAC Address as a string using HEX numbering.");
      _SERIAL_PRINTLN("For each of the 6 bytes of data in the MAC Address, ensure you use two digits");
      _SERIAL_PRINTLN("If on byte is '8' or 'A' enter it in the serial monitor as '08' or '0A'");  
      while(!_SERIAL_AVAILABLE()); //loop until we get serial data
      String aStr = _SERIAL_READSTRING();  //read until timeout
      aStr.trim();     // remove any \r \n whitespace at the end of the String
      uint8_t aTemp[MAC_BYTE_LENGTH]; //array to store the HEX values of the MAC address
      mHandler.getEnteredMACByteArray(aStr,aTemp); //convert mac string to byte array
      if(nowComm.createSecondaryPeer(aTemp)) { //add secondary node peer to esp now communication
           _SERIAL_PRINTLN("Added ESP Now peer");
           _SERIAL_PRINTLN("Printing entered secondary MAC address in bytes:"); //print out entered address
           serialPrintMAC(aTemp); //function that prints mac address in bytes to serial terminal
           uint8_t nodeNum = mHandler.getNextSecondaryNodeNumber(); //get next available node number, whatever is the lowest node number available
           if(sendData2SecondaryNode(aTemp,PRIMARY_MAC,nodeNum) && nowComm.getSendDataFlag()) { //if true mac address was sent and ack came back
             mHandler.storeSecondaryMAC(aStr,(nodeNum-1)); //store mac address in secondary array
             _SERIAL_PRINT("Sent primary mac address successfully to new Secondary Node #: "); _SERIAL_PRINTLN(nodeNum);
             _SERIAL_PRINTLN(); 
           }
           else { //if true sending primary mac failed
             nowComm.deleteSecondaryPeer(aTemp); //failed to send primary mac so delete
             _SERIAL_PRINTLN("Sending primary node MAC address failed, verify secondary MAC address and try again");
           }
           nowComm.clearSendFlags(); //clear send flags
      }
      else {
          _SERIAL_PRINTLN("Failed to add peer, check MAC address and try again");
      }
    }
    else if(s == "d" || s == "D") { //if true entering menu to delete peer
      _SERIAL_PRINTLN("To delete a secondary node from your ESP-Now dynamic network enter its MAC Address as a string using HEX numbering.");
      _SERIAL_PRINTLN("For each of the 6 bytes of data in the MAC Address, ensure you use two digits");
      _SERIAL_PRINTLN("If on byte is '8' or 'A' enter it in the serial monitor as '08' or '0A'");
      while(!_SERIAL_AVAILABLE()); //loop forever until serial data is available 
      String dStr = _SERIAL_READSTRING();  //read until timeout
      dStr.trim();                        // remove any \r \n whitespace at the end of the String
      _SERIAL_PRINTLN(dStr); //print entered string
      uint8_t mTemp[MAC_BYTE_LENGTH]; //create local array to hold mac address
      uint8_t addrSlot = mHandler.deleteSecondaryMAC(dStr,mTemp); //delete mac address from secondary array, but also get a copy before deleting
      if(addrSlot <= MAX_SECONDARY_NODES) { //if true we were able to find and delete address
        if(nowComm.deleteSecondaryPeer(mTemp)) { //if true we deleted the peer of the entered mac address
           _SERIAL_PRINTLN("Deleted ESP Now peer");
           _SERIAL_PRINTLN("Printing deleted secondary MAC address in bytes:"); //print out entered address
           serialPrintMAC(mTemp); //function that prints mac address in bytes to serial terminal
           _SERIAL_PRINT("Deleted Secondary Node #: "); _SERIAL_PRINTLN(addrSlot);
           _SERIAL_PRINTLN(); 
        }
        else { //if true failed to delete peer
          _SERIAL_PRINTLN("Failed to delete peer, with mac address");
          serialPrintMAC(mTemp); //function that prints mac address in bytes to serial terminal
        }
      }
      else { //secondary node to delete is out of node count range, this should not be true ever
        _SERIAL_PRINTLN("Something went wrong trying to delete secondary node"); 
      }
    }
    else { //incorrect serial command entered
      _SERIAL_PRINTLN("Serial command not recognized, no action taken");
    }
    dataTimer = millis(); //reset timer after adding node
  }
}

//timer based function that sends data requests to all the secondary nodes that are online
//only executes if there is at least one secondary node online (one peer)
void dataRequestTimer4SecondaryNodes() {
  if(millis() > (dataTimer + DATA_COLLECTION_TIMER) && nowComm.getPeerNumber()) { //if true time to request data from secondary nodes
    _SERIAL_PRINTLN("Preparing to send data request to all secondary nodes on the network...");
    for(uint8_t j=0; j<MAX_SECONDARY_NODES; j++) { //loop to run through all possible node numbers
      if(mHandler.secondaryNodeIsOnline(j)) { //if true secondary node exists at array location
        uint8_t sAddr[MAC_BYTE_LENGTH] = {0}; //intialize array to store mac address
        mHandler.getSecondaryMACAddress(sAddr,j); //get secondary node address at array location
        if(sendData2SecondaryNode(sAddr,DATA_REQUEST,false) && nowComm.getSendDataFlag()) { //send data request to secondary node
          _SERIAL_PRINT("Data request to node "); _SERIAL_PRINT(j+1); _SERIAL_PRINTLN(" was sent successfully.");
        }
        else { //atempt to send data to secondary node who should be online failed
          _SERIAL_PRINTLN("Data request send failed.");
        }
      }
    }
    nowComm.clearSendFlags(); //clear or reset esp now send flag and send complete flag
    dataTimer = millis(); //reset timer 
  }
}

//function executes when new data has been recieved from secondary nodes. Function then reports
//node that sent data and prints out data
void handleDataFromSecondaryNode() {
  if(nowComm.getRecDataFlag()) { //if we rec data this will be true
    uint8_t recAddr[MAC_BYTE_LENGTH]; //create byte array to hold secondary node address
    nowComm.recSecondaryNodeMACAddress(recAddr); //get secondary node address from rec callback
    uint8_t sNodeNum = mHandler.convertSecondaryAddr2NodeNumber(recAddr); //get secondary node number
    secondaryData sD = nowComm.getSecondaryData(); //get rec data struct from secondary node
    nowComm.clearRecDataFlag(); //clear rec data flag
    _SERIAL_PRINT("Received the following data from Secondary Node #: "); _SERIAL_PRINTLN(sNodeNum);
    _SERIAL_PRINT("Secondary node measured voltage: "); _SERIAL_PRINTLN(sD.vReading); //print node voltage from struct
    _SERIAL_PRINT("Secondary node measured logic state: "); _SERIAL_PRINTLN(sD.digState); //print node voltage from struct
  }
}

//this function sends data to a secondary node, it can either send a data request or primary mac address
//input arguments are mac address of secondary node and type
bool sendData2SecondaryNode(uint8_t* secMAC, uint8_t dataType, uint8_t nodeNum) {
  bool sendSuccess;
  if(dataType == DATA_REQUEST) { //sending request for data
    sendSuccess = nowComm.sendPrimaryDataRequest(secMAC); //sending data request to secondary node
  }
  else { //sending primary mac address
    uint8_t pMAC[6]; //create array to hold mac address
    mHandler.getMyMACAddress(pMAC);
    sendSuccess = nowComm.sendPrimaryMACAddress(secMAC,pMAC,nodeNum); //send primary mac address to secondary node
  }
  
  nowComm.waitForSend2Complete(); //functions block for very short time until send call back has completed
  return sendSuccess;
}

//prints out a mac address in hex format
//input argument is byte array of mac address
void serialPrintMAC(uint8_t* macAddr) {
  _SERIAL_PRINTHEX(macAddr[0],HEX);
  _SERIAL_PRINT(" ");
  _SERIAL_PRINTHEX(macAddr[1],HEX);
  _SERIAL_PRINT(" ");
  _SERIAL_PRINTHEX(macAddr[2],HEX);
  _SERIAL_PRINT(" ");
  _SERIAL_PRINTHEX(macAddr[3],HEX);
  _SERIAL_PRINT(" ");
  _SERIAL_PRINTHEX(macAddr[4],HEX);
  _SERIAL_PRINT(" ");
  _SERIAL_PRINTLNHEX(macAddr[5],HEX);
  _SERIAL_PRINTLN(); 
}


// Function to handle received data from secondary device
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    // Display the received message on the Serial Monitor
    Serial.print("Received message: ");
    for(int i = 0; i < len; i++) {
        Serial.print((char)incomingData[i]);
    }
    Serial.println();
}



