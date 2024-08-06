#include "Arduino.h"
#include "definitions.h"
#include "Node_Data.h"

Node_Data::Node_Data() { } //class constructor

//init class by defining digital pin for measurements
void Node_Data::initNodeData() {
  pinMode(DIGITAL_PIN,INPUT);
}

//make ADC measurement and convert to voltage
//hard coded 12 bit ADC and 3.3V reference
//ADC pin specified in definitions file
float Node_Data::getvoltageMeas() {
  uint16_t aDC = analogRead(VOLT_PIN); 
  return (3.3*((float)aDC/4095.0));
}

//returns digital high or low based on specified pin 
//in definitions file
uint8_t Node_Data::getDigitalReading() {
  return digitalRead(DIGITAL_PIN);
}
