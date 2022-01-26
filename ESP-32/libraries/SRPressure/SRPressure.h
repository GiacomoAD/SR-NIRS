/* ***************************************************************** */
/* File name:        SRPressure.h                                    */
/* File description: Automated Sphygmomanometer handler header file  */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    28nov2021                                       */
/* Revision date:    19jan2022                                       */
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
#define LC_SCK 16

#define DEFAULT_PRESSURE 23000   // in [Pa]
#define MAX_PRESSURE 24000       // in [Pa]
#define DEFAULT_VOT_DURATION 180 // in [s]

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
    /* Description:            Returns last HX711 data in kPa                               */
    /*                                                                                      */
    /* Entry parameters:       n/a                                                          */
    /*                                                                                      */
    /* Return parameters:      int -> Pressure (kPa)                              			*/
    /* ************************************************************************************ */
    int getPressure();

    /* ************************************************************************************ */
    /* Method's name:          getPressuremmHg                                              */
    /* Description:            Reads and convert HX711 data in mmHg                         */
    /*                                                                                      */
    /* Entry parameters:       n/a                                                          */
    /*                                                                                      */
    /* Return parameters:      int -> Pressure (mmHg)                              			*/
    /* ************************************************************************************ */
    int getPressuremmHg();

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
    /* Method's name:          setPump                                                      */
    /* Description:            Sets pump GPIO state to on or off                            */
    /*                                                                                      */
    /* Entry parameters:       unsigned char -> state                                       */
    /*                                                                                      */
    /* Return parameters:      n/a                                                          */
    /* ************************************************************************************ */
    void setPump(unsigned char state);

    /* ************************************************************************************ */
    /* Method's name:          setValve                                                     */
    /* Description:            Sets valve GPIO state to on or off                           */
    /*                                                                                      */
    /* Entry parameters:       unsigned char -> state                                       */
    /*                                                                                      */
    /* Return parameters:      n/a                                                          */
    /* ************************************************************************************ */
    void setValve(unsigned char state);

    /* ************************************************************************************ */
    /* Method's name:          setPressureCap                                               */
    /* Description:            Sets the maximum pressure of sphygmomanometer band           */
    /*                                                                                      */
    /* Entry parameters:       unsigned char pressure -> Value to set (mmHg)                */
    /*                                                                                      */
    /* Return parameters:      n/a                                                          */
    /* ************************************************************************************ */
    void setPressureCap(unsigned char pressure);

    /* ************************************************************************************ */
    /* Method's name:          getPressureCap                                               */
    /* Description:            Returns the maximum pressure of sphygmomanometer band        */
    /*                                                                                      */
    /* Entry parameters:       n/a                                                          */
    /*                                                                                      */
    /* Return parameters:      int -> Maximum pressure of sphygmomanometer band (kPA)       */
    /* ************************************************************************************ */
    int getPressureCap();

    /* ************************************************************************************ */
    /* Method's name:          checkPressure                                               	*/
    /* Description:            Check on pressure to ensure its under safety limits          */
    /*                                                                                      */
    /* Entry parameters:       n/a                       									*/
    /*                                                                                      */
    /* Return parameters:      unsigned char -> 0 == NOT OK / 1 == OK                       */
    /* ************************************************************************************ */
    unsigned char checkPressure();

    /* ************************************************************************************ */
    /* Method's name:          setVotDuration                                               */
    /* Description:            Sets the arm occlusion duration                              */
    /*                                                                                      */
    /* Entry parameters:       unsigned char duration -> Value to set (s)                   */
    /*                                                                                      */
    /* Return parameters:      n/a                                                          */
    /* ************************************************************************************ */
    void setVotDuration(unsigned char duration);

    /* ************************************************************************************ */
    /* Method's name:          getVotDuration                                               */
    /* Description:            Returns the current VOT duration that has been setted        */
    /*                                                                                      */
    /* Entry parameters:       n/a                                                          */
    /*                                                                                      */
    /* Return parameters:      unsigned char -> Arm occlusion experiment duration (kPA)     */
    /* ************************************************************************************ */
    unsigned char getVotDuration();

    /* ************************************************************************************ */
    /* Method's name:          runVOT                                                       */
    /* Description:            Start and Stop VOT based on current vot_state                */
    /*                                                                                      */
    /* Entry parameters:       n/a                                                          */
    /*                                                                                      */
    /* Return parameters:      n/a                                                          */
    /* ************************************************************************************ */
    void runVOT();

    void setVotState(unsigned char state);

private:
    HX711 sensorP;
    long lastReading = 0;
    int lastReading_kPa = 0;
    int lastReading_mmhg = 0;

    unsigned char pumpState = 0;
    unsigned char valveState = 0;
    unsigned char vot_duration = DEFAULT_VOT_DURATION;
    unsigned char vot_state = 0;

    int pressureCap = DEFAULT_PRESSURE;
};

#endif
