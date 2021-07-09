/* ***************************************************************** */
/* File name:        Sensor.h                                        */
/* File description: Sensor Base Class header file              	 */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    07Jul2021                                       */
/* Revision date:    09Jul2021                                       */
/* ***************************************************************** */
 
#ifndef Sensor_h
#define Sensor_h


#include <Arduino.h>
#include <MuxCD74HC.h>
#include <SRNIRSLED.h>
#include "Adafruit_ADS1X15.h"

class SRSensor
{

public:

/* ************************************************************************************ */
/* Method's name:          initMux                                                      */ 
/* Description:            Initialize sensor parameters                                 */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void initSensor();

/* ************************************************************************************ */
/* Method's name:          readState                                                    */ 
/* Description:            Read optical sensor based on state machine                   */
/*                                                                                      */
/* Entry parameters:       unsigned char sensorState -> current state                   */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void readState(unsigned char sensorState);

int channelReadingsR[3];  // R wavelength reading channels 1-3
int channelReadingsIR[3]; // IR wavelength reading channels 1-3
unsigned char chGains[3];

/* ************************************************************************************ */
/* Method's name:          changeCh                                                     */ 
/* Description:            Change current channel setting                               */
/*                                                                                      */
/* Entry parameters:       unsigned char det -> channel mux                             */
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 if invalid channel                        */
/*                                          1 if valid channel                          */
/* ************************************************************************************ */
unsigned char changeCh(unsigned char det);

/* ************************************************************************************ */
/* Method's name:          changeGain                                                   */ 
/* Description:            Change current gain setting                                  */
/*                                                                                      */
/* Entry parameters:       unsigned char gain -> gain mux                               */
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 if invalid gain                           */
/*                                          1 if valid gain                             */
/* ************************************************************************************ */
unsigned char changeGain(unsigned char gain);

/* ************************************************************************************ */
/* Method's name:          changeDetGain                                                */ 
/* Description:            Change default gain for each channel                         */
/*                                                                                      */
/* Entry parameters:       unsigned char gain -> gain mux                               */
/*                         unsigned char det -> channel mux                             */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void changeDetGain(unsigned char gain, unsigned char det);

/* ************************************************************************************ */
/* Method's name:          readSensor                                                   */ 
/* Description:            Read from current sensor setting                             */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> ADC reading (15 bits)                                 */
/*                                                                                      */
/* ************************************************************************************ */
int readSensor();

private:




MuxCD74HC gainMux;      // Gain multiplexer
MuxCD74HC chMux;        // Detector multiplexer
SRNIRSLED ledMux;       // LED multiplexer
Adafruit_ADS1115 ads;   // External ADC



};


 
#endif
