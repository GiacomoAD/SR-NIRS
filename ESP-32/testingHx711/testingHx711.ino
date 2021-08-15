/* ***************************************************************** */
/* File name:        testingHx711.ino                                */
/* File description: HX711 pressure reading test sketch              */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    13Aug2021                                       */
/* Revision date:    15Aug2021                                       */
/* ***************************************************************** */

// CALIBRATION:
// P = 611030*(3.8184e-09 * READING - 0.0038)

#include <HX711.h>
#include <DroneTimer.h>

/*DATA ACQUISITION FREQUENCY*/
#define FREQUENCY 100

#define PCONST1 611030
#define PCONST2 0.0000000038184
#define PCONST3 0.0038

/*IMPORTED CLASSES OBJECTS*/
DroneTimer timer;       // ESP-32 timer

/************ TIMER VARIABLES ***********/
volatile unsigned char timer_flag = 0;

/******** END OF TIMER VARIABLES ********/


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 17;
const int LOADCELL_SCK_PIN = 4;

HX711 sensorP;

/*Timer interruption routine*/
void IRAM_ATTR timerInterrupt()
{

  timer_flag = 1;
}

void setup()
{


  Serial.begin(115200);

  /* Setting up timer frequency and interruption routine */
  timer.initTimer(FREQUENCY, &timerInterrupt);

  timer.enableTimer();   // Enabling timer interruptions

  sensorP.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 64);
   
}

void loop()
{
  /* RUNS AT TIMER FREQUENCY*/
  if (timer_flag)
  {
    timer_flag = 0;
    
    if (sensorP.is_ready()) {
      long reading = sensorP.read();
      //Serial.print("HX711 reading: ");
      int P = PCONST1*(PCONST2 * reading - PCONST3);
      Serial.println(P);
    } 
    else {
      //Serial.println("HX711 not found.");
      yield();
    }

    
  }

  

}
