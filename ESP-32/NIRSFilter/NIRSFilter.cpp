/* ***************************************************************** */
/* File name:        NIRSFilter.cpp                                  */
/* File description: NIRS butterworth filter implementation file     */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    02jun2021                                       */
/* Revision date:    02jun2021                                       */
/* ***************************************************************** */

#include "NIRSFilter.h"

/* ************************************************************************************ */
/* Method's name:          filter                                                       */ 
/* Description:            Filter data with a 3rd order butterworth (Fc = 5 Hz)         */
/*                                                                                      */
/* Entry parameters:       int data -> data point that will be filtered                 */
/*                                                                                      */
/* Return parameters:      float y -> filtered data point                               */
/* ************************************************************************************ */
float NIRSFilter::filter(int data)
{  
    x[n-3] = x[n-2]; x[n-2] = x[n-1]; x[n-1] = x[n];
    x[n] = float(data);

    y[n-3] = y[n-2]; y[n-2] = y[n-1]; y[n-1] = y[n];
    y[n] = b[0]*x[n] + b[1]*x[n-1] + b[2]*x[n-2] + b[3]*x[n-3] - a[1]*y[n-1] - a[2]*y[n-2] - a[3]*y[n-3];

    return y[n];
}