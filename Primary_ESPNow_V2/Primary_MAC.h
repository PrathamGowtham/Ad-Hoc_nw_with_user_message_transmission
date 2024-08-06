/* Primary_MAC --> this class handles retrieving and storing MAC address of primary node
 *  It also handles storing and retreiving secondary node mac address that are on the network
 * 
 *  Created: 2/14/23
 *  Author: ForceTronics
*/

#ifndef Primary_MAC_h
#define Primary_MAC_h

#include "Arduino.h"
#include "definitions.h"
#include <WiFi.h>

class Primary_MAC
{
  public:
    Primary_MAC();
    String initGetMAC();
    void getMyMACAddress(uint8_t* mAddr); 
    void getSecondaryMACAddress(uint8_t* mAddr, uint8_t sPos);
    void storeSecondaryMAC(String s, uint8_t arrPos);
    void getEnteredMACByteArray(String m, uint8_t* bArray);
    uint8_t deleteSecondaryMAC(String s, uint8_t* gAddr);
    bool secondaryNodeIsOnline(uint8_t aNum);
    uint8_t convertSecondaryAddr2NodeNumber(uint8_t* sAddr);
    uint8_t getNextSecondaryNodeNumber();
    
  private:
    void convertMACString2MACAddress(char* mChar, uint8_t* macAddr);
    uint8_t deleteAddrSecondaryArray(uint8_t* sAddr);
    uint8_t myMAC[MAC_BYTE_LENGTH];
    uint8_t secondaryMAC[MAX_SECONDARY_NODES][MAC_BYTE_LENGTH];
    void setMACArray2Zeros();
};

#endif
