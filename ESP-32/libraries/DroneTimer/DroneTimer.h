/* ***************************************************************** */
/* File name:        DroneTimer.h                                    */
/* File description: Timer interface header file                     */
/* Author name:      Giacomo Dollevedo, Gustavo Fernandes            */
/* Creation date:    18nov2020                                       */
/* Revision date:    14dec2020                                       */
/* ***************************************************************** */
 
#ifndef DroneTimer_h
#define DroneTimer_h


#include <Arduino.h>



class DroneTimer
{

public:

/* ************************************************************************************ */
/* Method's name:          initTimer                                                  	*/ 
/* Description:            Initialize hardware timer interruptions                		*/
/*                                                                                      */
/* Entry parameters:       int frequency -> timer interrupt frequency                   */
/*                         void (*fn)(void) -> function pointer that will be exectued   */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
  void initTimer(int frequency, void (*fn)(void));


/* ************************************************************************************ */
/* Method's name:          enableTimer                                                  */ 
/* Description:            Enable timer interruptions                					*/
/*                                                                                      */
/* Entry parameters:       n/a 															*/
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */  
  void enableTimer();


/* ************************************************************************************ */
/* Method's name:          disableTimer                                                 */ 
/* Description:            Disable timer interruptions                					*/
/*                                                                                      */
/* Entry parameters:       n/a 															*/
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */  
  void disableTimer();

  
/* ************************************************************************************ */
/* Method's name:          setFrequency                                                 */ 
/* Description:            Set the timer frequency 	                					*/
/*                                                                                      */
/* Entry parameters:       int frequency -> frequency to be set 						*/
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */ 
  void setFrequency(int frequency);


/* ************************************************************************************ */
/* Method's name:          getFrequency                                                 */ 
/* Description:            Get the timer frequency 	                					*/
/*                                                                                      */
/* Entry parameters:       n/a															*/
/*                                                                                      */
/* Return parameters:      int -> Timer frequency                                 		*/
/* ************************************************************************************ */ 
  int getFrequency();
 

private:
  /*Hardware timer for system loop time management*/
  hw_timer_t * timer0 = NULL;
  
  int timerFreq = 10;

};
 
#endif
