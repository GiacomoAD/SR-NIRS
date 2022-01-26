/* ***************************************************************** */
/* File name:        SRBluetooth.cpp                                 */
/* File description: Bluetooth Connection Handler implementation file*/
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    30jul2021                                       */
/* Revision date:    14jan2022                                       */
/* ***************************************************************** */

#include "SRBluetooth.h"

/* ************************************************************************************ */
/* Method's name:          initBt                                                       */
/* Description:            Bluetooth initialization. Set up device name and pin         */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRBluetooth::initBt()
{
    BtHandler.begin("SRNIRS", true);
    BtHandler.setPin(pin);
}

/* ************************************************************************************ */
/* Method's name:          connect                                                      */
/* Description:            Connects SR-NIRS system with BT antenna and python GUI       */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRBluetooth::connect()
{
    /*Connecting to Bluetooth HC-05 antenna*/
    connected_flag = BtHandler.connect(address1); // Connecting to specific address

    if (connected_flag)
    {
        yield();
    }

    else
    {
        while (!BtHandler.connected(10000))
        {
            yield();
        }
    }

    while (start_flag == 0)
    {

        BtHandler.printf("#R\n");

        if (BtHandler.available())
        {
            BtHandler.readBytesUntil('>', bufferIn, 255);
            start_flag = 1;
        }

        delay(100);
    }

    start_flag = 0;

    /*Waiting for GUI start signal*/
    while (start_flag == 0)
    {
        if (BtHandler.available())
        {
            BtHandler.readBytesUntil('>', bufferIn, 255);
            start_flag = 1;
            BtHandler.printf("BTSTART\n\0");
        }

        yield();
    }
}

/* ************************************************************************************ */
/* Method's name:          sendData                                                     */
/* Description:            Sends an array of bytes (char) through Bluetooth connection  */
/*                                                                                      */
/* Entry parameters:       char* message -> array of data that will be sent             */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRBluetooth::sendData(char *message)
{
    BtHandler.printf("%s", message);
}

/* ************************************************************************************ */
/* Method's name:          sendGains                                                    */
/* Description:            Sends device gains passed as an unsigned char array through  */
/*                         Bluetooth connection                                         */
/*                                                                                      */
/* Entry parameters:       unsigned char* gains -> array of data that will be sent      */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void SRBluetooth::sendGains(unsigned char *gains)
{
    BtHandler.printf("#C%d;%d;%d\n\0", gains[0], gains[1], gains[2]);
}

/* ************************************************************************************ */
/* Method's name:          readCommand                                                  */
/* Description:                                                                         */
/*                                                                                      */
/* Entry parameters:                                                                    */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
char *SRBluetooth::readCommand()
{
    //bufferIn[0] = '\0';

    if (BtHandler.available())
    {
        //bufferIn[0] = '\0';
        BtHandler.readBytesUntil('>', bufferIn, 255);
    }

    return bufferIn;
}

void SRBluetooth::clearBuffer()
{
    bufferIn[0] = '\0';
}