#include <DroneTimer.h>
#include <Adafruit_ADS1X15.h>
#include <arduinoFFT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <NIRSFilter.h>
#include "BluetoothSerial.h"
#include <GainMux.h>

#define ADC0 (36)
#define DEBUG 0
#define LEDR (26)
#define LEDIR (14)
#define S0 (32)
#define S1 (25)
#define S2 (27)
#define N_GAIN 8

#define DET_PIN (23)
#define S1_DET (18)

#define FREQUENCY 50
#define MAX_SIZE 16384
#define PACKET_SIZE 150

#define CONSTY 0.8819
#define CONSTU 0.1181

#define FFT_ON 0
#define CALIBRACAO 1


DroneTimer timer;
NIRSFilter filtroIR;
NIRSFilter filtroR;

GainMux mux;

arduinoFFT FFT = arduinoFFT();
LiquidCrystal_I2C lcd(0x27, 16, 2);
BluetoothSerial ESP_BT;


/************** FFT DECLARATIONS *************/
const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = FREQUENCY;
//unsigned int timerFrequency = 2*FREQUENCY;
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/  
double vReal[samples];
double vImag[samples];
int i = 0;
//unsigned char flagFFT = FFT_ON;
/********** END OF FFT DECLARATIONS *********/


/************ TIMER DECLARATIONS ***********/
volatile unsigned char timer_flag = 0;
volatile unsigned char timer_count = 0;
volatile unsigned char troca = 0;

int leitura1 = 0;
float leitura1_filtrada = 0;
float leitura2_filtrada = 0;
int leitura2 = 0;
int leitura3 = 0;
int leitura4 = 0;
int leitura5 = 0;
int leitura6 = 0;

double tempo_zero = 0;
double tempo = 0;

unsigned char estado = 0;
/********** END TIMER DECLARATIONS *********/


Adafruit_ADS1115 ads;

/************** SELF GAIN CHANGE *************/

unsigned char flagCalibrate = CALIBRACAO;
int meanVal[N_GAIN];
int compare[N_GAIN];
int nRead = 0;
unsigned char currentChannel = 0;
int compara = 0;

/********** END OF SELF GAIN CHANGE *********/

/************** BLUETOOTH CONN *************/

unsigned char start_flag = 0;
char bufferIn[256] = {'\0'};

char *message_packet = (char *)calloc(MAX_SIZE, sizeof(char));

unsigned char packet_ready = 0;
int packets = 0;

uint8_t address1[6] = {0x98, 0xD3, 0x31, 0xF9, 0x87, 0x94}; // 98D3:31:F98794
char *pin = "1234";
unsigned char conectado = 0;

/********** END OF BLUETOOTH CONN **********/


/*Timer interruption routine*/
void IRAM_ATTR timerInterrupt()
{
    
    timer_flag = 1;

    //timer_count++;

}

void setup() {

    for(int i=0;i<N_GAIN;i++){
      meanVal[i] = 0;
      compare[i] = 0;
    }
  
    Serial.begin(115200);
    delay(500);
    Wire.setClock(400000);
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("ESP32 - Testando");
    lcd.setCursor(0, 1);
    lcd.print("BPM: ");
    
    ads.begin();
    timer.initTimer(6*FREQUENCY, &timerInterrupt);

    ESP_BT.begin("ESP32", true); //name of Bluetooth
    ESP_BT.setPin(pin);
    conectado = ESP_BT.connect(address1);
    
    if(conectado) {
      Serial.println("Connected Succesfully!");
    } 
    else {
      while(!ESP_BT.connected(10000)) {
        Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
      }
    }

    if(DEBUG){
      Serial.println("Setup OK!");  
    }
    
    pinMode(ADC0, INPUT);
    pinMode(LEDR, OUTPUT);
    pinMode(LEDIR, OUTPUT);

    pinMode(DET_PIN, OUTPUT);
    pinMode(S1_DET, OUTPUT);

    

    mux.initMux(S0,S1,S2);

    mux.changeGain(4);
   
    while(start_flag == 0){
      if(ESP_BT.available()){
        ESP_BT.readBytesUntil('>', bufferIn, 255);
        start_flag = 1;
      }
      yield();
    }

    Wire.setClock(400000);
    digitalWrite(LEDIR, HIGH);
    digitalWrite(DET_PIN, LOW);
    digitalWrite(S1_DET, LOW);
    mux.changeGain(0);
    ads.readADC_Differential_0_1();
    timer.enableTimer();
    tempo_zero = double(millis())/1000;

    
    
}

void loop() {

  if(timer_flag){
    timer_flag = 0;

    if(flagCalibrate == 1){
      meanVal[currentChannel] += ads.readADC_SingleEnded(0);
      nRead++;
      //Serial.println(nRead);

      if(nRead == 500){
        meanVal[currentChannel] = meanVal[currentChannel]/nRead;
        //Serial.println(meanVal[currentChannel]);
        currentChannel++;
        nRead = 0;

        mux.changeGain(currentChannel);

        if(currentChannel == N_GAIN){
          while(nRead < N_GAIN){
              compare[nRead] = meanVal[nRead] - 15000;
              //Serial.println(compare[nRead]);
              nRead++;
          }

          nRead = 0;
          currentChannel = 0;

          while(nRead < N_GAIN){
            if(nRead == 0){
              compara = compare[nRead];
              currentChannel = nRead;
              nRead++;
            }

            else{
              //Compara atual com o maior anterior
              // Se menor (distancia ate o 0 menor), troca
              if(abs(compare[nRead]) < abs(compara)){
                compara = compare[nRead];
                currentChannel = nRead;
              }
              nRead++;
            }
          }

          mux.changeGain(currentChannel);
          
          flagCalibrate = 0;
          //Serial.println(currentChannel);
          tempo_zero = double(millis())/1000;
        }
      }
      
      
    }

    if(flagCalibrate == 0 && estado == 0){

      digitalWrite(DET_PIN, LOW);
      digitalWrite(S1_DET, LOW);
      
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDIR, HIGH);
      
      tempo = double(millis()) / 1000 - tempo_zero;
      leitura1 = ads.readADC_SingleEnded(0);
      //leitura1_filtrada = (filtroIR.filter(leitura1));
      
      /*if(i < samples){
        vReal[i] = double(leitura1);
        vImag[i] = 0;
        i++;
      }*/
      
      estado++;
    }
    
    else if(flagCalibrate == 0 && estado == 1){
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDIR, LOW);
      
      leitura2 = ads.readADC_SingleEnded(0);
      //leitura2_filtrada = filtroR.filter(leitura2);

      estado++;
      mux.changeGain(0);
      
    }

    else if(flagCalibrate == 0 && estado == 2){
      digitalWrite(DET_PIN, HIGH);
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDIR, HIGH);
      
      leitura3 = ads.readADC_SingleEnded(0);

      estado++;
      
    }

    else if(flagCalibrate == 0 && estado == 3){
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDIR, LOW);

      leitura4 = ads.readADC_SingleEnded(0);

      estado++;
      
    }

    else if(flagCalibrate == 0 && estado == 4){
      digitalWrite(DET_PIN, LOW);
      digitalWrite(S1_DET, HIGH);
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDIR, HIGH);
      
      leitura5 = ads.readADC_SingleEnded(0);

      estado++;
      
    }

    else if(flagCalibrate == 0 && estado == 5){
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDIR, LOW);
      estado = 0;
      
      leitura6 = ads.readADC_SingleEnded(0);
      
      mux.changeGain(currentChannel);
      
      sprintf(message_packet, "%s%.2f;%d;%d;%d;%d;%d;%d;%d;%d\n\0", message_packet, tempo, leitura1, leitura2, leitura3, leitura4,leitura5,leitura6,currentChannel,0);
      packets++;
      

      if(packets == PACKET_SIZE){
        packet_ready = 1;
        packets = 0;
      }
      
    }
   
  }

  if(packet_ready){
    packet_ready = 0;
    ESP_BT.printf("%s", message_packet);
    message_packet[0] = '\0';
  }

  /*DESCOMENTAR PARA TESTE DE GANHOS*/
  /*if(timer_count == 150){
    timer_count = 0;
    troca++;
    mux.changeGain(troca);
    if(troca == 7){
      troca = -1;
    }
  }*/

  if(i == samples && FFT_ON){
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    //Serial.println("FFT COMPUTADA");
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
    //Serial.println("MAGNITUDE COMPUTADA");
    double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
    //Serial.print("MAIOR PICO: ");
    //Serial.println(x, 6);
    x = x*60;
    if(x < 300 && x > 35){
      lcd.setCursor(5, 1);
      lcd.print("   ");
      lcd.setCursor(5, 1);
      lcd.print(int(x));
      i = 0;
    }
  }
  
}
