/* ***************************************************************** */
/* File name:        GainMux.h                                       */
/* File description: Gain multiplexing interface header file         */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    09Jun2021                                       */
/* Revision date:    09Jun2021                                       */
/* ***************************************************************** */
 
#ifndef GainMux_h
#define GainMux_h


#include <Arduino.h>



class GainMux
{

public:

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
void initMux(unsigned char S0, unsigned char S1, unsigned char S2);


/* ************************************************************************************ */
/* Method's name:          changeGain                                                   */ 
/* Description:            Change device gain                                           */
/*                                                                                      */
/* Entry parameters:       unsigned char gainSelect -> Desired gain value               */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */  
void changeGain(unsigned char gainSelect);

 

private:

    unsigned char S0 = 32;
    unsigned char S1 = 25;
    unsigned char S2 = 27;

};
 
#endif
