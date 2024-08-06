/* Secondary_Comm class --> wrapper for ESP Now communication library for
 * secondary nodes
 *
 * Created: 2/14/23
 *  Author: ForceTronics
 */ 

#ifndef Secondary_Comm_h
#define Secondary_Comm_h

#include <esp_now.h>
#include "Arduino.h"
#include "definitions.h"

//struct to hold secondary node data
struct secondaryData {
     float vReading = 0; //secondary node voltage reading
     int8_t digState = 0; //secondary node GPIO state reading
};

//struct that holds data from primary node: mac address and my node number
struct primaryData {
    uint8_t pAddr[MAC_BYTE_LENGTH];
    uint8_t nodeNumber = 0;
};

class Secondary_Comm
{
  public:
    Secondary_Comm();
    bool initSecondary_Comm();
    bool createPrimaryPeer();
    void copyPrimaryMACAddress(uint8_t* temp);  
    bool sendData2Primary();                                                                                                                          
    bool getRecDataFlag();
    void clearRecDataFlag();
    bool getSendDataFlag();
    void clearSendDataFlag();
    bool getNetworkState();
    void setNetworkState(bool netState);
    uint8_t getMyNodeNumber();
    
  private:
    esp_now_peer_info_t primaryPeer;
    esp_err_t espNowStatus = ESP_OK;
};

#endif
