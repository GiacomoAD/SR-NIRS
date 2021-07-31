/* ***************************************************************** */
/* File name:        SRBluetooth.h                                   */
/* File description: Bluetooth Connection Handler header file        */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    30jul2021                                       */
/* Revision date:    30jul2021                                       */
/* ***************************************************************** */
 
#ifndef SRBluetooth_h
#define SRBluetooth_h

#include "BluetoothSerial.h"



class SRBluetooth
{

public:

/* ************************************************************************************ */
/* Method's name:          initBt                                                       */ 
/* Description:            Bluetooth initialization. Set up device name and pin         */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
 void initBt();



/* ************************************************************************************ */
/* Method's name:          connect                                                      */ 
/* Description:            Connects SR-NIRS system with BT antenna and python GUI       */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
 void connect();


/* ************************************************************************************ */
/* Method's name:          sendData                                                     */ 
/* Description:            Sends an array of bytes (char) through Bluetooth connection  */
/*                                                                                      */
/* Entry parameters:       char* message -> array of data that will be sent             */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */ 
void sendData(char* message);

/* ************************************************************************************ */
/* Method's name:          sendGains                                                    */ 
/* Description:            Sends device gains passed as an unsigned char array through  */
/*                         Bluetooth connection                                         */
/*                                                                                      */
/* Entry parameters:       unsigned char* gains -> array of data that will be sent      */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */ 
void sendGains(unsigned char* gains);

private:


BluetoothSerial BtHandler; // Bluetooth comm

char *pin = "1234"; // Bluetooth connection security PIN
uint8_t address1[6] = {0x98, 0xD3, 0x31, 0xF9, 0x87, 0x94};    // 98D3:31:F98794
unsigned char connected_flag = 0;
unsigned char start_flag = 0;       // System start flag
char bufferIn[256] = {'\0'};    // Incoming bluetooth message buffer
  
};
 
#endif
