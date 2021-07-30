/* ***************************************************************** */
/* File name:        main_app.ino                                    */
/* File description: SR-NIRS sensor main app                         */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    30Jun2021                                       */
/* Revision date:    27Jul2021                                       */
/* ***************************************************************** */

#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include "BluetoothSerial.h"

#include <DroneTimer.h>
#include <NIRSFilter.h>
#include <SRsensor.h>

/*SET DEBUG TO 1 TO ENABLE SERIAL MONITOR PRINTS*/
#define DEBUG 0

/*SET CALIBRATION TO 1 TO ENABLE AUTO-GAIN CHANGE*/
#define CALIBRATION 1
#define N_GAIN 8
#define MEANVAL 1
#define N_DET 3

/*DATA ACQUISITION FREQUENCY*/
#define FREQUENCY 50

/*BLUETOOTH MESSAGE PACKET*/
#define MAX_SIZE 16384
#define PACKET_SIZE 150

/*IMPORTED CLASSES OBJECTS*/
DroneTimer timer;       // ESP-32 timer
SRSensor NIRSsensor;    // NIRS Sensor
BluetoothSerial ESP_BT; // Bluetooth comm

/************ TIMER VARIABLES ***********/
volatile unsigned char timer_flag = 0;

double initTime = 0.0; // Initial time
double cTime = 0.0;    // Current time
/******** END OF TIMER VARIABLES ********/

/************ SENSOR READ VARIABLES ************/

unsigned char sensorState = 0;

/******** END OF SENSOR READ VARIALES *********/

/************** SELF GAIN CHANGE VARIABLES *************/
unsigned char flagCalibrate = CALIBRATION;

int meanVal[N_GAIN];
int compare[N_GAIN];
int nRead = 0;
int compara = 0;
unsigned char currentChannel = 0;
unsigned char cDet = 0;
/********** END OF SELF GAIN VARIABLES *********/

/************** MEAN VALUE VARIABLES *************/

int means[N_DET][N_GAIN];
unsigned char currCh = 0;
unsigned char currGain = 0;
unsigned char flagMeans = MEANVAL;

/************** END OF MEAN VALUE VARIABLES *************/


/************** BLUETOOTH CONN VARIABLES *************/
int packets = 0; //

char *pin = "1234"; // Bluetooth connection security PIN

char bufferIn[256] = {'\0'};    // Incoming bluetooth message buffer
unsigned char start_flag = 0;   // System start flag
unsigned char packet_ready = 0; // Flag for ready to send packets
unsigned char connected_flag = 0;
uint8_t address1[6] = {0x98, 0xD3, 0x31, 0xF9, 0x87, 0x94};    // 98D3:31:F98794
char *message_packet = (char *)calloc(MAX_SIZE, sizeof(char)); // Outgoing bluetooth message buffer

/********** END OF BLUETOOTH CONN VARIABLES **********/

/*Timer interruption routine*/
void IRAM_ATTR timerInterrupt()
{

  timer_flag = 1;
}

void setup()
{

  /* Initializing variables */
  for (int i = 0; i < N_GAIN; i++)
  {
    meanVal[i] = 0;
    compare[i] = 0;
    means[0][i] = 0;
    means[1][i] = 0;
    means[2][i] = 0;
  }

  if (DEBUG)
  {
    Serial.begin(115200);
  }

  /*Initializing NIRS Sensor object*/
  NIRSsensor.initSensor();

  /* Setting up timer frequency and interruption routine */
  timer.initTimer(6 * FREQUENCY, &timerInterrupt);

  /* Setting up Bluetooth Connection*/
  ESP_BT.begin("ESP32", true);               //name of Bluetooth Device
  ESP_BT.setPin(pin);                        // Setting security PIN

  /*Connecting to Bluetooth HC-05 antenna*/
  connected_flag = ESP_BT.connect(address1); // Connecting to specific address
  if (connected_flag)
  {
    if (DEBUG)
      Serial.println("Connected Succesfully!");
  }

  else
  {
    while (!ESP_BT.connected(10000))
    {
      if (DEBUG)
        Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
    }
  }

  if (DEBUG)
  {
    Serial.println("Setup OK!");
  } 


  while(start_flag == 0){
    ESP_BT.printf("#R\n");
    if(ESP_BT.available()){
      ESP_BT.readBytesUntil('>', bufferIn, 255);
      start_flag = 1;
    }
    yield();
  }
  start_flag = 0;

  Wire.setClock(400000); // Setting I2C clock speed to 400 kHz


  /*Waiting for GUI start signal*/
  while(start_flag == 0){
      if(ESP_BT.available()){
        ESP_BT.readBytesUntil('>', bufferIn, 255);
        start_flag = 1;
        ESP_BT.printf("BTSTART\n\0");
      }
      yield();
  }

  /*Switching to IR to calibrate*/
  NIRSsensor.switchIR();
  
  timer.enableTimer();   // Enabling timer interruptions


  /*AUTO GAIN SETTING ROUTINE*/
  /*Finding mean read value for all channels + gains*/
  while(flagMeans)
    findMean();

  /*Finding which gain sets the value closer to the middle of the adc range*/
  while (flagCalibrate)
    calibrate();

  /*Creating configuration parameters message*/
  for(int j=0;j<N_DET;j++){
    for(int i=0;i<N_GAIN;i++){
      if(i==0){
        sprintf(message_packet, "%s#C\0", message_packet, means[j][i]);
      }
      
      sprintf(message_packet, "%s%d;\0", message_packet, means[j][i]);
    }
    
    sprintf(message_packet, "%s\n\0", message_packet);
  }

  /*Sending configuration message over Bluetooth*/
  ESP_BT.printf("#C%d;%d;%d\n\0", NIRSsensor.chGains[0], NIRSsensor.chGains[1], NIRSsensor.chGains[2]); /*Auto gain settings*/
  ESP_BT.printf("%s", message_packet); /*Sending all means by channel*/
  message_packet[0] = '\0';

  /*Resetting time variable to timestamp*/
  initTime = initTime = double(millis()) / 1000;
   
}

void loop()
{
  /* RUNS AT TIMER FREQUENCY*/
  if (timer_flag)
  {
    timer_flag = 0;

    /*Timestamping on the first data point*/
    if(sensorState == 0){
      cTime = getTimestamp();
    }

    /*Reading from NIRS Sensor*/
    NIRSsensor.readState(sensorState);
    sensorState++;
    
  }

  /*When the last sensor is read the outgoing message buffer is written*/
  if(sensorState == 6){
    sensorState = 0;
    /*Writing to Bluetooth outgoing buffer*/
    /*Message contains data structured as:*/
    /*time;IR[1];R[1];IR[2];R[2];IR[3];R[3]*/
    /*where 1, 2 and 3 are each respective channel*/
    sprintf(message_packet, "%s%.2f;%d;%d;%d;%d;%d;%d\n\0", message_packet, cTime, NIRSsensor.channelReadingsIR[0], NIRSsensor.channelReadingsR[0],
      NIRSsensor.channelReadingsIR[1], NIRSsensor.channelReadingsR[1], NIRSsensor.channelReadingsIR[2], NIRSsensor.channelReadingsR[2]);

    packets++; // adding to # of lines on packet

    /* If the packet ready to send */
    if(packets == PACKET_SIZE){
      packet_ready = 1; // Flipping flag to send packets
      packets = 0;      // Resetting # of lines written
    }
  }

  /* If the packet ready to send */
  if(packet_ready){
    packet_ready = 0; // Resetting flag
    ESP_BT.printf("%s", message_packet);  // Sending outgoing buffer through Bluetooth
    message_packet[0] = '\0';
  }

  
}

/* ************************************************************************************ */
/* Method's name:          findMean                                                     */ 
/* Description:            Finds every channel mean read value for each gain, based on  */
/*                         timer frequency                                              */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void findMean(){
  if(timer_flag){
    timer_flag = 0;

    means[currCh][currGain] += NIRSsensor.readSensor();
    nRead++;

    if(nRead == 500){
      means[currCh][currGain] = means[currCh][currGain]/nRead;
      currGain++;
      
      
      if(currGain==N_GAIN){
        currGain = 0;
        currCh++;
        
      }

      else{
        NIRSsensor.changeGain(currGain);
      }
  
      if(currCh == N_DET){
        flagMeans = 0;
        nRead = 0;
        NIRSsensor.changeGain(0);
        NIRSsensor.changeCh(0);
      }

      else{
        NIRSsensor.changeCh(currCh);
        nRead = 0;
      }
      
    }
    
  }
}

/* ************************************************************************************ */
/* Method's name:          calibrate                                                    */ 
/* Description:            Uses mean values array to find set which gain will get the   */
/*                         sensor reading closer to the middle of adc range             */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      n/a                                                          */
/* ************************************************************************************ */
void calibrate(){

  int testComp[N_DET][N_GAIN];

  unsigned char currGain = 0;
  int lastValue = 0;

  for(int i=0;i<N_DET;i++){
    for(int j=0;j<N_GAIN;j++){
      testComp[i][j] = means[i][j] - 15000;
    }
  }

  for(int i=0;i<N_DET;i++){
    currGain = 0;
    lastValue = 0;
    for(int j=0;j<N_GAIN;j++){
      
      if(j == 0){
        lastValue = testComp[i][j];
        currGain = j;
      }

      else{
        if (abs(testComp[i][j]) < abs(lastValue)){
          lastValue = testComp[i][j];
          currGain = j;
        }        
      }
    }
    
    NIRSsensor.changeDetGain(currGain, i);
    
  }
  
  flagCalibrate = 0;
  
}

/* ************************************************************************************ */
/* Method's name:          getTimestamp                                                 */ 
/* Description:            Calculate time based on the initTime variable                */
/*                                                                                      */
/* Entry parameters:       n/a                                                          */
/*                                                                                      */
/* Return parameters:      double -> time since the device began the measuring routine  */
/* ************************************************************************************ */
double getTimestamp(){
  return double(millis()) / 1000 - initTime;
}
