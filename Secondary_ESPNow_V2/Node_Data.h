/* Node_Data--> this class handles retrieving and handling measured data
 * 
 *  Created: 3/1/23
 *  Author: ForceTronics
 */

#ifndef Node_Data_h
#define Node_Data_h

#include "Arduino.h"
#include "definitions.h"

class Node_Data
{
  public:
    Node_Data();
    void initNodeData();
    float getvoltageMeas();
    uint8_t getDigitalReading();
    
  private:
   
};

#endif
