/* Seconadry_MAC --> this class handles retrieving and storing MAC of this node
 * 
 *  Created: 2/14/23
 *  Author: ForceTronics
 */

#ifndef Seconadry_MAC_h
#define Seconadry_MAC_h

#include "Arduino.h"
#include "definitions.h"

class Seconadry_MAC
{
  public:
    Seconadry_MAC();
    String initGetMAC();
    void getMyMACAddress(uint8_t* mAddr); 
    
  private:
    void convertMACString2MACAddress(char* mChar, uint8_t* macAddr);
    uint8_t myMAC[MAC_BYTE_LENGTH];

};

#endif
