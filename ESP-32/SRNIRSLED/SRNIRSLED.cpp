/* ***************************************************************** */
/* File name:        SRNIRSLED.h                                     */
/* File description: LED Multiplexer Class Implementation file       */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    06Jul2021                                       */
/* Revision date:    06Jul2021                                       */
/* ***************************************************************** */

#include "SRNIRSLED.h"


/* ************************************************************************************ */
/* Method's name:          initLED                                                      */ 
/* Description:            Initialize LED multiplexing pins                             */
/*                                                                                      */
/* Entry parameters:       unsigned char LEDR -> Digital select pin LEDR                */
/*                         unsigned char LEDIR -> Digital select pin LEDIR              */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRNIRSLED::initLED(unsigned char LEDR, unsigned char LEDIR){

    this->LEDR = LEDR;
    this->LEDIR = LEDIR;
    pinMode(LEDR, OUTPUT);
    pinMode(LEDIR, OUTPUT);
       
}

/* ************************************************************************************ */
/* Method's name:          switchR                                                      */ 
/* Description:            Turn Red LED on                                              */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/*                                                                                      */
/* ************************************************************************************ */  
void SRNIRSLED::switchR(){

  digitalWrite(LEDIR, LOW);
  digitalWrite(LEDR, HIGH);

}

/* ************************************************************************************ */
/* Method's name:          switchIR                                                     */ 
/* Description:            Turn Infrared Red LED on                                     */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/*                                                                                      */
/* ************************************************************************************ */  
void SRNIRSLED::switchIR(){

  digitalWrite(LEDR, LOW);
  digitalWrite(LEDIR, HIGH);

}