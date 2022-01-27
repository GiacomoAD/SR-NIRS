/* ***************************************************************** */
/* File name:        SRPressure.h                                    */
/* File description: Automated Sphygmomanometer handler header file  */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    28nov2021                                       */
/* Revision date:    27jan2022                                       */
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
void SRPressure::initPress()
{
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
long SRPressure::readPressure()
{
	if (sensorP.is_ready())
	{
		lastReading = sensorP.read();
		lastReading_kPa = PCONST1 * (PCONST2 * lastReading - PCONST3);
		checkPressure();
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
int SRPressure::convertPressure()
{
	lastReading_mmhg = lastReading_kPa * PA_TO_MMHG;
	return lastReading_mmhg;
}

/* ************************************************************************************ */
/* Method's name:          getPressure                                                  */
/* Description:            Returns last HX711 data in kPa                               */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> Pressure (kPa)                              			*/
/* ************************************************************************************ */
int SRPressure::getPressure()
{
	if (lastReading_kPa < 0)
	{
		return 0;
	}
	return lastReading_kPa;
}

/* ************************************************************************************ */
/* Method's name:          getPressuremmHg                                              */
/* Description:            Reads and convert HX711 data in mmHg                         */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      int -> Pressure (mmHg)                              			*/
/* ************************************************************************************ */
int SRPressure::getPressuremmHg()
{
	convertPressure();
	if (lastReading_mmhg < 0)
	{
		return 0;
	}
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
void SRPressure::togglePump()
{
	switch (pumpState)
	{
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
void SRPressure::toggleValve()
{
	switch (valveState)
	{
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
void SRPressure::setPressureCap(unsigned char pressure)
{
	pressureCap = int(pressure * MMHG_TO_PA);
	return;
}

/* ************************************************************************************ */
/* Method's name:          getPressureCap                                               */
/* Description:            Returns the maximum pressure of sphygmomanometer band        */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      long -> Maximum pressure of sphygmomanometer band (kPA)      */
/* ************************************************************************************ */
int SRPressure::getPressureCap()
{
	return pressureCap;
}

/* ************************************************************************************ */
/* Method's name:          checkPressure                                               	*/
/* Description:            Check on pressure to ensure its under safety limits          */
/*                                                                                      */
/* Entry parameters:       n/a                       									*/
/*                                                                                      */
/* Return parameters:      unsigned char -> 0 == NOT OK / 1 == OK                       */
/* ************************************************************************************ */
unsigned char SRPressure::checkPressure()
{
	if (lastReading_kPa >= MAX_PRESSURE)
	{
		digitalWrite(VALVE, LOW);
		valveState = 0;
		digitalWrite(PUMP, LOW);
		pumpState = 0;

		return 0;
	}

	return 1;
}

/* ************************************************************************************ */
/* Method's name:          setPump                                                      */
/* Description:            Sets pump GPIO state to on or off                            */
/*                                                                                      */
/* Entry parameters:       unsigned char -> state                                       */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::setPump(unsigned char state)
{
	switch (state)
	{
	case 0:
		digitalWrite(PUMP, LOW);
		pumpState = 0;
		break;
	default:
		digitalWrite(PUMP, HIGH);
		pumpState = 1;
		break;
	}
	return;
}

/* ************************************************************************************ */
/* Method's name:          setValve                                                     */
/* Description:            Sets valve GPIO state to on or off                           */
/*                                                                                      */
/* Entry parameters:       unsigned char -> state                                       */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::setValve(unsigned char state)
{
	switch (state)
	{
	case 0:
		digitalWrite(VALVE, LOW);
		valveState = 0;
		break;
	default:
		digitalWrite(VALVE, HIGH);
		valveState = 1;
		break;
	}
	return;
}

/* ************************************************************************************ */
/* Method's name:          setVotDuration                                               */
/* Description:            Sets the arm occlusion duration                              */
/*                                                                                      */
/* Entry parameters:       unsigned char duration -> Value to set (s)                   */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::setVotDuration(unsigned char duration)
{
	vot_duration = duration;
}

/* ************************************************************************************ */
/* Method's name:          getVotDuration                                               */
/* Description:            Returns the current VOT duration that has been setted        */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      unsigned char -> Arm occlusion experiment duration (kPA)     */
/* ************************************************************************************ */
unsigned char SRPressure::getVotDuration()
{
	return vot_duration;
}

void SRPressure::setVotState(unsigned char state)
{
	switch (state)
	{
	case 0:
		vot_state = 0;
		break;

	default:
		vot_state = 1;
		break;
	}
}

/* ************************************************************************************ */
/* Method's name:          runVOT                                                       */
/* Description:            Start and Stop VOT based on current vot_state                */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRPressure::runVOT()
{

	static unsigned char pump_flag = 0;
	static int VOT_start_time = 0;

	if (vot_state == 1)
	{

		/* If pressure is under desired value and pump is off */
		if (getPressure() < getPressureCap() && pump_flag == 0)
		{
			setValve(1); // Switches valve on (close)
			setPump(1);	 // Turns pump on
			pump_flag = 1;
		}

		/* If pump is on */
		if (pump_flag == 1)
		{
			/* If pressure raises above specified limit */
			if (getPressure() >= getPressureCap())
			{
				setPump(0);						  // Turns pump off
				VOT_start_time = millis() / 1000; // occlusion start timestamp
				pump_flag = 2;
			}
		}

		/* If occlusion is ongoing and pump is off*/
		if (pump_flag == 2)
		{
			if (int(millis() / 1000) - VOT_start_time >= getVotDuration())
			{
				setValve(0);   // Switches valve off (open)
				vot_state = 0; // Resets vot_state
				pump_flag = 0; // Resets pump_flag
			}
		}
	}

	else
	{
		setValve(0); // Switches valve off (open)
	}
}