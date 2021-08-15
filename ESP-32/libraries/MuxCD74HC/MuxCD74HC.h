/* ***************************************************************** */
/* File name:        MuxCD74HC.h                                     */
/* File description: Multiplexer Base Class header file              */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    05Jul2021                                       */
/* Revision date:    05Jul2021                                       */
/* ***************************************************************** */
 
#ifndef MuxCD74HC_h
#define MuxCD74HC_h


#include <Arduino.h>



class MuxCD74HC
{

public:

/* ************************************************************************************ */
/* Method's name:          initMux                                                      */ 
/* Description:            Initialize gain multiplexing pins                            */
/*                                                                                      */
/* Entry parameters:       unsigned char S0 -> Digital select pin 0                     */
/*                         unsigned char S1 -> Digital select pin 1                     */
/*                         unsigned char S2 -> Digital select pin 2                     */
/*                         unsigned char S3 -> Digital select pin 3                     */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void initMux(unsigned char S0);
void initMux(unsigned char S0, unsigned char S1);
void initMux(unsigned char S0, unsigned char S1, unsigned char S2);
void initMux(unsigned char S0, unsigned char S1, unsigned char S2, unsigned char S3);

/* ************************************************************************************ */
/* Method's name:          selectMux                                                    */ 
/* Description:            Change mux output                                            */
/*                                                                                      */
/* Entry parameters:       unsigned char select -> Desired output to be enabled         */
/*                                                                                      */
/* Return parameters:      1 -> if operation was successful                             */
/*                         0 -> if operation failed                                     */
/* ************************************************************************************ */  
unsigned char selectMux(unsigned char select);

private:

    unsigned char S0 = 99;
    unsigned char S1 = 99;
    unsigned char S2 = 99;
    unsigned char S3 = 99;
    unsigned char muxType = 0;


    unsigned char selectMuxType1(unsigned char select);
    unsigned char selectMuxType2(unsigned char select);
    unsigned char selectMuxType3(unsigned char select);
    unsigned char selectMuxType4(unsigned char select);
};
 
#endif
