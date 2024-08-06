/* Primary_Comm class --> wrapper for ESP Now communication library for
 * primary nodes
 *
 * Created: 2/14/23
 * Author: ForceTronics
 */ 

#ifndef Primary_Comm_h
#define Primary_Comm_h

#include "Arduino.h"
#include "definitions.h"
#include <esp_now.h>
#include <WiFi.h>

//struct to hold secondary node data
struct secondaryData {
     float vReading; //voltage reading from secondary
     int8_t digState; //GPIO state reading from secondary
};

class Primary_Comm
{
  public:
    Primary_Comm();
    bool initPrimary_Comm();
    bool sendPrimaryMACAddress(uint8_t* sMAC, uint8_t* pMAC, uint8_t nNum);
    bool sendPrimaryDataRequest(uint8_t* sMAC);
    bool createSecondaryPeer(uint8_t* sAddr); 
    bool deleteSecondaryPeer(uint8_t* sAddr);                                                                                                                            
    bool getRecDataFlag();
    void clearRecDataFlag();
    bool getSendDataFlag();
    void waitForSend2Complete();
    void clearSendFlags();
    secondaryData getSecondaryData();
    void recSecondaryNodeMACAddress(uint8_t* rAddr);
    int getPeerNumber();
    
  private:
    bool espNowSendErrorStatus(esp_err_t errType);
    
};

#endif
