/* ***************************************************************** */
/* File name:        main_app.ino                                    */
/* File description: SR-NIRS sensor main app                         */
/* Author name:      Giacomo Dollevedo                               */
/* Creation date:    30Jun2021                                       */
/* Revision date:    22Jul2021                                       */
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
SRSensor NIRSsensor;
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

  NIRSsensor.initSensor();

  /* Setting up timer frequency and interruption routine */
  timer.initTimer(6 * FREQUENCY, &timerInterrupt);

  /* Setting up Bluetooth Connection*/
  ESP_BT.begin("ESP32", true);               //name of Bluetooth Device
  ESP_BT.setPin(pin);                        // Setting security PIN
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

  Wire.setClock(400000); // Setting I2C clock speed to 400 kHz


  while(start_flag == 0){
      if(ESP_BT.available()){
        ESP_BT.readBytesUntil('>', bufferIn, 255);
        start_flag = 1;
      }
      yield();
    }

  NIRSsensor.switchIR();
  timer.enableTimer();   // Enabling timer interruptions


  /**/
  while(flagMeans)
    findMean();

  /* AUTO GAIN CONTROL*/
  while (flagCalibrate)
    calibrate();

  for(int j=0;j<N_DET;j++){
    for(int i=0;i<N_GAIN;i++){
      if(i==0){
        sprintf(message_packet, "%s#C\0", message_packet, means[j][i]);
      }
      
      sprintf(message_packet, "%s%d;\0", message_packet, means[j][i]);
    }
    
    sprintf(message_packet, "%s\n\0", message_packet);
  }

  ESP_BT.printf("#C%d;%d;%d\n\0", NIRSsensor.chGains[0], NIRSsensor.chGains[1], NIRSsensor.chGains[2]);
  ESP_BT.printf("%s", message_packet);
  message_packet[0] = '\0';

  initTime = initTime = double(millis()) / 1000;
   
}

void loop()
{
  /* RUNS AT TIMER FREQUENCY*/
  if (timer_flag)
  {
    timer_flag = 0;

    if(sensorState == 0){
      cTime = getTimestamp();
    }
    
    NIRSsensor.readState(sensorState);
    sensorState++;
    
  }

  if(sensorState == 6){
    sensorState = 0;
    /*Writing to Bluetooth outgoing buffer*/
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

double getTimestamp(){
  return double(millis()) / 1000 - initTime;
}
