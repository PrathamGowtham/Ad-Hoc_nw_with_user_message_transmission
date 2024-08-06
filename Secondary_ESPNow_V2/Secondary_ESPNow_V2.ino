/* Example sketch for ESP32 that uses the ESP-Now communication protocol to create a dynamic wireless network
 * This sketch was used in a YouTube video entitled: Using ESP-Now to Create a Dynamic Wireless ESP32 Device Network Part 3
 * This sketch was created by ForceTronics and is free to use and modify at your own risk
 * ESP Now documentation: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html#_CPPv416esp_now_peer_num
 */
#include "Secondary_Comm.h"
#include "Seconadry_MAC.h"
#include "definitions.h"

Seconadry_MAC mhandler; //declare object for mac address class
Secondary_Comm sComm; //declare object for ESPNow Comm class
static uint8_t pMAddr[MAC_BYTE_LENGTH];

void setup() {
  //function initializes ESPNow comm
  initializeESPNowComm();
  
  //initialize mac address class, get my mac as a string print, convert to byte array and print
  initializeMACAddress();
  Serial.begin(115200);

}

void loop() {
  //loop until ESPNow data receive flag goes high
  if(sComm.getRecDataFlag()) {
   secondaryHandleRecAndSendingData(); //handle received data reply as needed
   sComm.clearRecDataFlag(); //since rec flag was set to true, need to reset
 }

  if (Serial.available()) {
        String message = Serial.readStringUntil('\n'); // Read the message until newline character
        message.trim(); // Remove any trailing newlines or spaces

        if (message.length() > 0) {
            // Convert the message to a byte array
            uint8_t payload[message.length() + 1];
            message.getBytes(payload, message.length() + 1);

            // Send the message to the primary device
            esp_now_send(pMAddr, payload, sizeof(payload));

            Serial.println("Message sent: " + message);
        }
    }
    delay(100); // Add a small delay to avoid overwhelming the serial buffer
}

//handles functions and logic for initializing ESP-Now communication
void initializeESPNowComm() {
  bool espInit = sComm.initSecondary_Comm(); //init esp now comm and ensure it started correctly
  delay(5000); //delay to allow user to open serial monitor
  _SERIAL_BEGIN(115200); //now can start serial commm
  if(!espInit) { //if true issue initializing ESP Now comm
    _SERIAL_PRINTLN("ESP Now communication failed to initialize, check your setup and restart program");
    while(1); //loop forever
  }
  else {
    _SERIAL_PRINTLN("ESP Now communication initialized");
  }
}

//Inits mac address handling class, stores primary mac, prints out mac address
void initializeMACAddress() {
  _SERIAL_PRINTLN("Getting my MAC Address as String:"); 
  _SERIAL_PRINTLN(mhandler.initGetMAC()); //call function to return ESP32 MAC address and print as string
  uint8_t mTemp[MAC_BYTE_LENGTH]; //create byte array to store mac address
  mhandler.getMyMACAddress(mTemp); //convert MAC address from string to array of bytes and store in array
  _SERIAL_PRINTLN();
  _SERIAL_PRINTLN("Printing my MAC address in bytes:"); //print MAC address out as bytes of data in hex format
  serialPrintMAC(mTemp); //print mac address as HEX bytes
}

//function handles received data from primary node, determines what request is, 
//and sends data back to primary as applicable
void secondaryHandleRecAndSendingData() {
  if(!sComm.getNetworkState()) { //if true we have not joined network and expecting primary node mac address
      _SERIAL_PRINTLN("Received MAC Address from Primary Node, here it is:");
       //temp array to print primary node MAC address
      sComm.copyPrimaryMACAddress(pMAddr); //copy primary node MAC Address
      serialPrintMAC(pMAddr); //print mac address to serial monitor
      _SERIAL_PRINTLN("Attempting to add primary peer...");
      if(sComm.createPrimaryPeer()) { //add primary node as peer using mac address
         _SERIAL_PRINTLN("Successfully added primary peer!");
         _SERIAL_PRINTLN();
         sComm.setNetworkState(true); //set on network flag to true now that we added primary node
      }
      else { //if true was not able to add primary node as esp now peer, try resending primary mac address
         _SERIAL_PRINTLN("Failed to add peer, so not sending confirmation");
      }
   }
   else { //if true we are expecting data request
     delay((sComm.getMyNodeNumber() - 1)*SEND_DATA_DELAY); //transmit delay to prevent collisions, delay based off node number
     _SERIAL_PRINT("Received data request from Primary Node, sending data from node # "); _SERIAL_PRINTLN(sComm.getMyNodeNumber());
     sComm.sendData2Primary(); //send node data to primary
   }
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

