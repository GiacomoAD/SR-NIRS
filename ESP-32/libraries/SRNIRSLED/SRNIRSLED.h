/* ***************************************************************** */
/* File name:        SRNIRSLED.h                                     */
/* File description: LED Multiplexer Class header file               */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    06Jul2021                                       */
/* Revision date:    06Jul2021                                       */
/* ***************************************************************** */
 
#ifndef SRNIRSLED_h
#define SRNIRSLED_h


#include <Arduino.h>



class SRNIRSLED
{

public:

/* ************************************************************************************ */
/* Method's name:          initLED                                                      */ 
/* Description:            Initialize LED multiplexing pins                             */
/*                                                                                      */
/* Entry parameters:       unsigned char LEDR -> Digital select pin LEDR                */
/*                         unsigned char LEDIR -> Digital select pin LEDIR              */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void initLED(unsigned char LEDR, unsigned char LEDIR);

/* ************************************************************************************ */
/* Method's name:          switchR                                                      */ 
/* Description:            Turn Red LED on                                              */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/*                                                                                      */
/* ************************************************************************************ */  
void switchR();

/* ************************************************************************************ */
/* Method's name:          switchIR                                                     */ 
/* Description:            Turn Infrared LED on                                         */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/*                                                                                      */
/* ************************************************************************************ */  
void switchIR();

private:

    unsigned char LEDR = 99;
    unsigned char LEDIR = 99;

};
 
#endif
