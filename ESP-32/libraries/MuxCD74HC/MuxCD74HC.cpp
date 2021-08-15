/* ***************************************************************** */
/* File name:        MuxCD74HC.h                                     */
/* File description: Multiplexer Base Class header file              */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    05Jul2021                                       */
/* Revision date:    05Jul2021                                       */
/* ***************************************************************** */

#include "MuxCD74HC.h"

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
void MuxCD74HC::initMux(unsigned char S0)
{
    this->S0 = S0;
    muxType = 1;
    pinMode(S0, OUTPUT);
    
}
void MuxCD74HC::initMux(unsigned char S0, unsigned char S1)
{
    this->S0 = S0;
    this->S1 = S1;
    muxType = 2;
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
  
}
void MuxCD74HC::initMux(unsigned char S0, unsigned char S1, unsigned char S2)
{
    this->S0 = S0;
    this->S1 = S1;
    this->S2 = S2;
    muxType = 3;
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);

}

void MuxCD74HC::initMux(unsigned char S0, unsigned char S1, unsigned char S2, unsigned char S3)
{
    this->S0 = S0;
    this->S1 = S1;
    this->S2 = S2;
    this->S3 = S3;
    muxType = 4;
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
  
}

/* ************************************************************************************ */
/* Method's name:          selectMux                                                    */ 
/* Description:            Change mux output                                            */
/*                                                                                      */
/* Entry parameters:       unsigned char select -> Desired output to be enabled         */
/*                                                                                      */
/* Return parameters:      1 -> if operation was successful                             */
/*                         0 -> if operation failed                                     */
/* ************************************************************************************ */  
unsigned char MuxCD74HC::selectMux(unsigned char select)
{
  switch(muxType){
    
    case 1:
      return selectMuxType1(select);
    break;
    
    case 2:
      return selectMuxType2(select);
    break;
    
    case 3:
      return selectMuxType3(select);
    break;
    
    case 4:
      return selectMuxType4(select);
    break;

  }

}

unsigned char MuxCD74HC::selectMuxType1(unsigned char select){
  if(S0 == 99){
    return 0;
  }

  else{
    switch(select){
      case 0:
        digitalWrite(S0, LOW);
        break;

      case 1:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
        break;

      default:
        digitalWrite(S0, LOW);
    }

    return 1;
  }
}

unsigned char MuxCD74HC::selectMuxType2(unsigned char select){
  if(S0 == 99 || S1 == 99){
    return 0;
  }
  else{
    switch(select){
      case 0:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
        break;

      case 1:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, LOW);
        break;

      case 2:
        digitalWrite(S0, LOW);
        digitalWrite(S1, HIGH);
        break;

      case 3:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, HIGH);
        break;

      default:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
      
    }
    return 1;
  }
}

unsigned char MuxCD74HC::selectMuxType3(unsigned char select){

  if(S0 == 99 || S1 == 99 || S2 == 99){
    return 0;
  }

  else{
    switch(select){
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
    return 1;
  }
}

unsigned char MuxCD74HC::selectMuxType4(unsigned char select){

  if(S0 == 99 || S1 == 99 || S2 == 99 || S3 == 99){
    return 0;
  }

  else{
    switch(select){
      case 0:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
        digitalWrite(S3, LOW);
        break;

      case 1:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
        digitalWrite(S3, LOW);
        break;

      case 2:
        digitalWrite(S0, LOW);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, LOW);
        digitalWrite(S3, LOW);
        break;

      case 3:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, LOW);
        digitalWrite(S3, LOW);
        break;

      case 4:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, LOW);
        break;

      case 5:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, LOW);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, LOW);
        break;
        
      case 6:
        digitalWrite(S0, LOW);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, LOW);
        break;

      case 7:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, LOW);
        break;

      case 8:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
        digitalWrite(S3, HIGH);
        break;

      case 9:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
        digitalWrite(S3, HIGH);
        break;

      case 10:
        digitalWrite(S0, LOW);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, LOW);
        digitalWrite(S3, HIGH);
        break;

      case 11:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, LOW);
        digitalWrite(S3, HIGH);
        break;

      case 12:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, HIGH);
        break;

      case 13:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, LOW);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, HIGH);
        break;
        
      case 14:
        digitalWrite(S0, LOW);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, HIGH);
        break;

      case 15:
        digitalWrite(S0, HIGH);
        digitalWrite(S1, HIGH);
        digitalWrite(S2, HIGH);
        digitalWrite(S3, HIGH);
        break;

      default:
        digitalWrite(S0, LOW);
        digitalWrite(S1, LOW);
        digitalWrite(S2, LOW);
        digitalWrite(S3, LOW);  
    }

    return 1;
  }
}