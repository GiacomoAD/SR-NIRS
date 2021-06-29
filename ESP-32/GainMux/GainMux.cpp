/* ***************************************************************** */
/* File name:        GainMux.cpp                                     */
/* File description: Gain multiplexing implementation file           */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    09Jun2021                                       */
/* Revision date:    09Jun2021                                       */
/* ***************************************************************** */

#include "GainMux.h"

/* ************************************************************************************ */
/* Method's name:          initMux                                                      */ 
/* Description:            Initialize gain multiplexing pins                            */
/*                                                                                      */
/* Entry parameters:       unsigned char S0 -> Digital select pin 0                     */
/*                         unsigned char S1 -> Digital select pin 1                     */
/*                         unsigned char S2 -> Digital select pin 2                     */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void GainMux::initMux(unsigned char S0, unsigned char S1, unsigned char S2)     
{
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
  
}

/* ************************************************************************************ */
/* Method's name:          changeGain                                                   */ 
/* Description:            Change device gain                                           */
/*                                                                                      */
/* Entry parameters:       unsigned char gainSelect -> Desired gain value               */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */  
void GainMux::changeGain(unsigned char gainSelect)
{

  switch(gainSelect){
    case 0:
      digitalWrite(S0, LOW);
      digitalWrite(S1, LOW);
      digitalWrite(S2, LOW);
      break;

    case 1:
      digitalWrite(S0, HIGH);
      digitalWrite(S1, LOW);
      digitalWrite(S2, LOW);
      break;

    case 2:
      digitalWrite(S0, LOW);
      digitalWrite(S1, HIGH);
      digitalWrite(S2, LOW);
      break;

    case 3:
      digitalWrite(S0, HIGH);
      digitalWrite(S1, HIGH);
      digitalWrite(S2, LOW);
      break;

    case 4:
      digitalWrite(S0, LOW);
      digitalWrite(S1, LOW);
      digitalWrite(S2, HIGH);
      break;

    case 5:
      digitalWrite(S0, HIGH);
      digitalWrite(S1, LOW);
      digitalWrite(S2, HIGH);
      break;
      
    case 6:
      digitalWrite(S0, LOW);
      digitalWrite(S1, HIGH);
      digitalWrite(S2, HIGH);
      break;

    case 7:
      digitalWrite(S0, HIGH);
      digitalWrite(S1, HIGH);
      digitalWrite(S2, HIGH);
      break;

    default:
      digitalWrite(S0, HIGH);
      digitalWrite(S1, LOW);
      digitalWrite(S2, HIGH);
    
  }
}
