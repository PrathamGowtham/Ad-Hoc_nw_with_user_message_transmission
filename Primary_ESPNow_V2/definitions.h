/*
 * definitions.h --> macros for arduino sketch
 *
 * Created: 2/28/23
 *  Author: ForceTronics
 */ 

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

//Timing 
#define DATA_COLLECTION_TIMER 5000
//MCU pins
#define MAC_PIN 26
//MAC Address 
#define MAC_LENGTH 18
#define MAC_BYTE_LENGTH 6
#define MAX_SECONDARY_NODES 20
//send data types
#define DATA_REQUEST 1
#define PRIMARY_MAC 0

//used for debug via the serial output, if you comment out next #define statement debug is off
#define SERIAL_PRINT 1
 #ifdef SERIAL_PRINT
  #define _SERIAL_BEGIN(x) Serial.begin(x);
  #define _SERIAL_PRINT(x) Serial.print(x);
  #define _SERIAL_PRINTLN(x) Serial.println(x);
  #define _SERIAL_PRINTHEX(x,y) Serial.print(x,y);
  #define _SERIAL_PRINTLNHEX(x,y) Serial.println(x,y);
  #define _SERIAL_AVAILABLE() ({bool flag; flag = Serial.available(); flag;})
  #define _SERIAL_READSTRING() ({String s; s = Serial.readString(); s;})
  #define _SERIAL_READ() ({char c; c = Serial.read(); c;})
 #else
  #define _SERIAL_BEGIN(x)
  #define _SERIAL_PRINT(x)
  #define _SERIAL_PRINTLN(x)
  #define _SERIAL_PRINTHEX(x,y) Serial.print(x,y);
  #define _SERIAL_PRINTLNHEX(x,y)
  #define _SERIAL_AVAILABLE()
  #define _SERIAL_READSTRING()
  #define _SERIAL_READ()
 #endif


#endif /* DEFINITIONS_H_ */
