/* ***************************************************************** */
/* File name:        SRPressure.h                                    */
/* File description: Automated Sphygmomanometer handler header file  */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    28nov2021                                       */
/* Revision date:    28nov2021                                       */
/* ***************************************************************** */
 

#include "SRPressure.h"


/* ************************************************************************************ */
/* Method's name:          initPress                                                    */ 
/* Description:            Sphygmomanometer controler initialization                    */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::initPress(){
	sensorP.begin(LC_DOUT, LC_SCK, 32);
	pinMode(PUMP, OUTPUT);
	pinMode(VALVE, OUTPUT);

}

/* ************************************************************************************ */
/* Method's name:          readPressure                                                 */ 
/* Description:            Reads pressure from HX711                                    */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      long -> Raw from HX711                                       */
/* ************************************************************************************ */
long SRPressure::readPressure(){
	if(sensorP.is_ready()){
      lastReading = sensorP.read();
	}
	return lastReading;
}

/* ************************************************************************************ */
/* Method's name:          convertPressure                                              */ 
/* Description:            Converts raw reading to mmHg                                 */
/*                                                                                      */
/* Entry parameters:       n/a 									                        */
/*                                                                                      */
/* Return parameters:      int -> Converted mmHg pressure                               */
/* ************************************************************************************ */
int SRPressure::convertPressure(){
	lastReading_mmhg = PCONST1*(PCONST2 * lastReading - PCONST3);
	lastReading_mmhg = lastReading_mmhg*PA_TO_MMHG;
	return lastReading_mmhg;
}

/* ************************************************************************************ */
/* Method's name:          getPressure                                                  */ 
/* Description:            Reads and convert HX711 data                                 */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> Converted mmHG pressure                               */
/* ************************************************************************************ */
int SRPressure::getPressure(){
	readPressure();
	convertPressure();
	return lastReading_mmhg;
}

/* ************************************************************************************ */
/* Method's name:          togglePump                                                   */ 
/* Description:            Toggles pump motor GPIO                                      */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::togglePump(){
	switch(pumpState){
		case 0:
			digitalWrite(PUMP, HIGH);
			pumpState = 1;
			break;

		case 1:
			digitalWrite(PUMP, LOW);
			pumpState = 0;
			break;
		default:
			digitalWrite(PUMP, LOW);
			pumpState = 0;
	}
}

/* ************************************************************************************ */
/* Method's name:          toggleValve                                                  */ 
/* Description:            Toggles solenoid valve GPIO                                  */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::toggleValve(){
	switch(valveState){
		case 0:
			digitalWrite(VALVE, HIGH);
			valveState = 1;
			break;

		case 1:
			digitalWrite(VALVE, LOW);
			valveState = 0;
			break;
		default:
			digitalWrite(VALVE, LOW);
			valveState = 0;
	}
}


/* ************************************************************************************ */
/* Method's name:          setPressureCap                                               */ 
/* Description:            Sets the maximum pressure of sphygmomanometer band           */
/*                                                                                      */
/* Entry parameters:       unsigned char pressure -> Value (mmHg) to set                */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::setPressureCap(unsigned char pressure){
	pressureCap = pressure*MMHG_TO_PA;
	return;
}

/* ************************************************************************************ */
/* Method's name:          checkPressure                                               	*/ 
/* Description:            Check on pressure to ensure its under safety limits          */
/*                                                                                      */
/* Entry parameters:       n/a                       									*/
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 == NOT OK / 1 == OK                       */
/* ************************************************************************************ */
unsigned char SRPressure::checkPressure(){
	if(lastReading >= MAX_PRESSURE){
		digitalWrite(VALVE, LOW);
		valveState = 0;
		digitalWrite(PUMP, LOW);
		pumpState = 0;

		return 0;
	}

}