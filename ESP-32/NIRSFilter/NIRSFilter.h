/* ***************************************************************** */
/* File name:        NIRSFilter.h                                    */
/* File description: NIRS butterworth filter  header file            */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    02jun2021                                       */
/* Revision date:    02jun2021                                       */
/* ***************************************************************** */
 
#ifndef NIRSFilter_h
#define NIRSFilter_h


#include <Arduino.h>



class NIRSFilter
{

public:

    /* ************************************************************************************ */
    /* Method's name:          filter                                                       */ 
    /* Description:            Filter data with a 3rd order butterworth (Fc = 5 Hz)         */
    /*                                                                                      */
    /* Entry parameters:       int data -> data point that will be filtered                 */
    /*                                                                                      */
    /* Return parameters:      float y -> filtered data point                               */
    /* ************************************************************************************ */
    float filter(int data);



private:
  
  float b[4] = {0.0181, 0.0543, 0.0543, 0.0181};
  float a[4] = {1.0000,-1.7600, 1.1829,-0.2781};
  
  float x[4] = {0,0,0,0};
  float y[4] = {0,0,0,0};

  unsigned char n = 3;

};
 
#endif
