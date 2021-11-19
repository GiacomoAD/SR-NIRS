#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <DroneTimer.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

DroneTimer timer;       // ESP-32 timer

#define LEDR  (26)
#define LEDIR (14)

#define Sg0 (32)
#define Sg1 (25)
#define Sg2 (27)

#define Sd0 (2)
#define Sd1 (4)

#define FREQUENCY 300

unsigned char currDet = 0;


volatile unsigned char timer_flag = 0;
volatile unsigned char contador = 0;
volatile unsigned char estado = -1;

int16_t adc0 = 0;
int16_t adc1 = 0;

int16_t R1 = 0;
int16_t IR1 = 0;
int16_t R2 = 0;
int16_t IR2 = 0;
int16_t R3 = 0;
int16_t IR3 = 0;

/*Timer interruption routine*/
void IRAM_ATTR timerInterrupt()
{

  timer_flag = 1;

  contador++;
  estado++;
  /*if(contador == 250){
    estado++;
  }

  if(estado == 8)
    estado = 0;
*/
  if(contador == 254)
    contador = 0;
  

}

void setup(void) 
{
  //Wire.begin();
  //Wire.setClock(400000);
  Serial.begin(115200);
  Serial.println("Hello!");
  
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADSg1015, 0.1875mV/ADSg1115)");
  
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADSg1015  ADSg1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
   ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV


  /* Setting up timer frequency and interruption routine */
  timer.initTimer(FREQUENCY, &timerInterrupt);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDIR, OUTPUT);

  pinMode(Sd0, OUTPUT);
  pinMode(Sd1, OUTPUT);

  pinMode(Sg0, OUTPUT);
  pinMode(Sg1, OUTPUT);
  pinMode(Sg2, OUTPUT);
  
  
  digitalWrite(Sd0, HIGH);
  digitalWrite(Sd1, LOW);

  digitalWrite(Sg0, LOW);
  digitalWrite(Sg1, LOW);
  digitalWrite(Sg2, LOW);

  digitalWrite(LEDIR, HIGH);
  
  
  ads.begin(0x48);

  timer.enableTimer();   // Enabling timer interruptions
}

void loop(void) 
{


  if(timer_flag){
    timer_flag = 0;

    if(contador%2 == 0){
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDIR,LOW);
      
    }
    
    else{
      digitalWrite(LEDIR, HIGH);
      digitalWrite(LEDR, LOW);
    }

    switch(estado){
      case 0:
      digitalWrite(Sd0, LOW);
      digitalWrite(Sd1, HIGH);
      IR1 = ads.readADC_SingleEnded(0);
      break;
      
      case 1:
      
      R1 = ads.readADC_SingleEnded(0);
            
      break;

      case 2:
      digitalWrite(Sd0,HIGH);
      digitalWrite(Sd1, LOW);
      IR2 = ads.readADC_SingleEnded(0);
      break;
      
      case 3:
        
      R2 = ads.readADC_SingleEnded(0);
            
      break;

      case 4:
      digitalWrite(Sd0, LOW);
      digitalWrite(Sd1, LOW);
      IR3 = ads.readADC_SingleEnded(0);
      break;
      
      case 5:
      
      R3 = ads.readADC_SingleEnded(0);
      estado = -1;
      Serial.printf("%d\t%d\t%d\t%d\t%d\t%d\n", IR1,R1,IR2,R2,IR3,R3);
            
      break;

      /*case 2:
      
      
      digitalWrite(LEDIR, HIGH);
      digitalWrite(LEDR, LOW);
      adc2 = ads.readADC_SingleEnded(0);
      break;
      
      case 3:
      digitalWrite(LEDIR, LOW);
      digitalWrite(LEDR, HIGH);
      adc3 = ads.readADC_SingleEnded(0);
      digitalWrite(Sd0, LOW);
      digitalWrite(Sd1, HIGH);
      changeGain(5);
      estado = -1;
      Serial.printf("%d,%d,%d,%d\n", adc0,adc1,adc2,adc3);
      break;*/
    }

    /*if(contador%2 == 0){
      adc0 = ads.readADC_SingleEnded(0);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDIR,LOW);
      
      }
    else{
      adc1 = ads.readADC_SingleEnded(0);
      digitalWrite(LEDIR, HIGH);
      digitalWrite(LEDR, LOW);
      Serial.printf("%d,%d\n", adc0, adc1);
    }    
  }

  
    
  }*/
}
}

/*
void changeGain(unsigned char select){
switch(select){
      case 0:
        digitalWrite(Sg0, LOW);
        digitalWrite(Sg1, LOW);
        digitalWrite(Sg2, LOW);
        break;

      case 1:
        digitalWrite(Sg0, HIGH);
        digitalWrite(Sg1, LOW);
        digitalWrite(Sg2, LOW);
        break;

      case 2:
        digitalWrite(Sg0, LOW);
        digitalWrite(Sg1, HIGH);
        digitalWrite(Sg2, LOW);
        break;

      case 3:
        digitalWrite(Sg0, HIGH);
        digitalWrite(Sg1, HIGH);
        digitalWrite(Sg2, LOW);
        break;

      case 4:
        digitalWrite(Sg0, LOW);
        digitalWrite(Sg1, LOW);
        digitalWrite(Sg2, HIGH);
        break;

      case 5:
        digitalWrite(Sg0, HIGH);
        digitalWrite(Sg1, LOW);
        digitalWrite(Sg2, HIGH);
        break;
        
      case 6:
        digitalWrite(Sg0, LOW);
        digitalWrite(Sg1, HIGH);
        digitalWrite(Sg2, HIGH);
        break;

      case 7:
        digitalWrite(Sg0, HIGH);
        digitalWrite(Sg1, HIGH);
        digitalWrite(Sg2, HIGH);
        break;

      default:
        digitalWrite(Sg0, HIGH);
        digitalWrite(Sg1, LOW);
        digitalWrite(Sg2, HIGH);
      
    }
}
*/
/*switch(estado){
      case 0:
        changeGain(0);
        break;
        
      case 1:
        changeGain(1);
        break;
        
      case 2:
        changeGain(2);
        break;

      case 3:
        changeGain(3);
        break;

      case 4:
        changeGain(4);
        break;
        
      case 5:
        changeGain(5);
        break;
        
      case 6:
        changeGain(6);
        break;

      case 7:
        changeGain(7);
        break;
    }*/
