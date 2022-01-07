/* ***************************************************************** */
/* File name:        SRPressure.h                                    */
/* File description: Automated Sphygmomanometer handler header file  */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    28nov2021                                       */
/* Revision date:    28nov2021                                       */
/* ***************************************************************** */
 
#ifndef SRPressure_h
#define SRPressure_h

#define PCONST1 611030
#define PCONST2 0.0000000076368
#define PCONST3 0.0038

#define PA_TO_MMHG 0.0075
#define MMHG_TO_PA 133.322

#define PUMP 4
#define VALVE 2

#define LC_DOUT 17
#define LC_SCK  16

#define DEFAULT_PRESSURE 23000
#define MAX_PRESSURE 24000

#include "HX711.h"

// CALIBRATION:
// P = 611030*(3.8184e-09 * READING - 0.0038)
// 7.6368e-09

class SRPressure
{

public:

/* ************************************************************************************ */
/* Method's name:          initPress                                                    */ 
/* Description:            Sphygmomanometer controler initialization                    */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void initPress();

/* ************************************************************************************ */
/* Method's name:          readPressure                                                 */ 
/* Description:            Reads pressure from HX711                                    */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      long -> Raw from HX711                                       */
/* ************************************************************************************ */
long readPressure();

/* ************************************************************************************ */
/* Method's name:          convertPressure                                              */ 
/* Description:            Converts raw reading to mmHg                                 */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> Converted mmHg pressure                               */
/* ************************************************************************************ */
int convertPressure();

/* ************************************************************************************ */
/* Method's name:          getPressure                                                  */ 
/* Description:            Reads and convert HX711 data                                 */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> Converted mmHG pressure                               */
/* ************************************************************************************ */
int getPressure();

/* ************************************************************************************ */
/* Method's name:          togglePump                                                   */ 
/* Description:            Toggles pump motor GPIO                                      */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void togglePump();

/* ************************************************************************************ */
/* Method's name:          toggleValve                                                  */ 
/* Description:            Toggles solenoid valve GPIO                                  */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void toggleValve();

/* ************************************************************************************ */
/* Method's name:          setPressureCap                                               */ 
/* Description:            Sets the maximum pressure of sphygmomanometer band           */
/*                                                                                      */
/* Entry parameters:       unsigned char pressure -> Value to set                       */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void setPressureCap(unsigned char pressure);


/* ************************************************************************************ */
/* Method's name:          checkPressure                                               	*/ 
/* Description:            Check on pressure to ensure its under safety limits          */
/*                                                                                      */
/* Entry parameters:       n/a                       									*/
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 == NOT OK / 1 == OK                       */
/* ************************************************************************************ */
unsigned char checkPressure();

private:

HX711 sensorP;
long lastReading = 0;
int lastReading_mmhg = 0;

unsigned char pumpState = 0;
unsigned char valveState = 0;

int pressureCap = DEFAULT_PRESSURE;


};
 
#endif
