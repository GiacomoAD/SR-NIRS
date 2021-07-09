/* ***************************************************************** */
/* File name:        Sensor.cpp                                      */
/* File description: Sensor Base Class header file                   */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    07Jul2021                                       */
/* Revision date:    09Jul2021                                       */
/* ***************************************************************** */

#include "SRSensor.h"
#include <MuxCD74HC.h>
#include <SRNIRSLED.h>
#include "Adafruit_ADS1X15.h"


/*LED MULTIPLEXING PINS*/
#define LEDR (26)
#define LEDIR (14)

/*GAIN MULTIPLEXING PINS*/
#define Sg0 (32)
#define Sg1 (25)
#define Sg2 (27)

/*DETECTOR MULTIPLEXING PINS*/
#define Sd0 (23)
#define Sd1 (18)


/* ************************************************************************************ */
/* Method's name:          initMux                                                      */ 
/* Description:            Initialize sensor parameters                                 */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRSensor::initSensor()
{


  /* Initializing External ADC */
  ads.begin();

  /* Initializing both CD74HC mux */
  gainMux.initMux(Sg0, Sg1, Sg2);
  chMux.initMux(Sd0, Sd1);

  /* Initializing LED mux */
  ledMux.initLED(LEDR, LEDIR);

  ledMux.switchR();      // Turning on RED LED as visual cue
  gainMux.selectMux(0);  // Highest Gain setting
  chMux.selectMux(0);    // First detector setting

  chGains[0] = 0;
  chGains[1] = 0;
  chGains[2] = 0;

    
}

/* ************************************************************************************ */
/* Method's name:          readState                                                    */ 
/* Description:            Read optical sensor based on state machine                   */
/*                                                                                      */
/* Entry parameters:       unsigned char sensorState -> current state                   */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRSensor::readState(unsigned char sensorState){

/* Channel 1 - IR Wavelength */
  if(sensorState == 0){
    chMux.selectMux(0); // First detector setting
    ledMux.switchIR();  // Switching IR LED on

    channelReadingsIR[0] = ads.readADC_SingleEnded(0); // Reading ADC from I2C

  }

  /* Channel 1 - R Wavelength */
  else if(sensorState == 1){
    ledMux.switchR(); // Switching to R LED

    channelReadingsR[0] = ads.readADC_SingleEnded(0); // Reading ADC from I2C

    gainMux.selectMux(chGains[1]); // Channel 2 Gain setting

  }

  /* Channel 2 - IR Wavelength */
  else if(sensorState == 2){
    chMux.selectMux(1); // Second detector setting
    ledMux.switchIR();  // Switching IR LED on

    channelReadingsIR[1] = ads.readADC_SingleEnded(0); // Reading ADC from I2C

  }

  /* Channel 2 - R Wavelength */
  else if(sensorState == 3){
    ledMux.switchR(); // Switching to R LED

    channelReadingsR[1] = ads.readADC_SingleEnded(0); // Reading ADC from I2C

    gainMux.selectMux(chGains[2]); // Channel 2 Gain setting

  }

  /* Channel 3 - IR Wavelength */
  else if(sensorState == 4){
    chMux.selectMux(2); // Third detector setting
    ledMux.switchIR();  // Switching IR LED on

    channelReadingsIR[2] = ads.readADC_SingleEnded(0); // Reading ADC from I2C

  }

  /* Channel 3 - R Wavelength */
  else{
    ledMux.switchR(); // Switching to R LED

    channelReadingsR[2] = ads.readADC_SingleEnded(0); // Reading ADC from I2C

    gainMux.selectMux(chGains[0]); // Channel 1 Gain setting

  }

}

/* ************************************************************************************ */
/* Method's name:          changeDetGain                                                */ 
/* Description:            Change default gain for each channel                         */
/*                                                                                      */
/* Entry parameters:       unsigned char gain -> gain mux                               */
/*                         unsigned char det -> channel mux                             */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRSensor::changeDetGain(unsigned char gain, unsigned char det){
  
  chGains[det] = gain;
  return;
  
}

/* ************************************************************************************ */
/* Method's name:          changeCh                                                     */ 
/* Description:            Change current channel setting                               */
/*                                                                                      */
/* Entry parameters:       unsigned char det -> channel mux                             */
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 if invalid channel                        */
/*                                          1 if valid channel                          */
/* ************************************************************************************ */
unsigned char SRSensor::changeCh(unsigned char det){
  
  return chMux.selectMux(det);

}

/* ************************************************************************************ */
/* Method's name:          changeGain                                                   */ 
/* Description:            Change current gain setting                                  */
/*                                                                                      */
/* Entry parameters:       unsigned char gain -> gain mux                               */
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 if invalid gain                           */
/*                                          1 if valid gain                             */
/* ************************************************************************************ */
unsigned char SRSensor::changeGain(unsigned char gain){
  return gainMux.selectMux(gain);
}


/* ************************************************************************************ */
/* Method's name:          readSensor                                                   */ 
/* Description:            Read from current sensor setting                             */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> ADC reading (15 bits)                                 */
/*                                                                                      */
/* ************************************************************************************ */
int SRSensor::readSensor(){

  return ads.readADC_SingleEnded(0);

}
