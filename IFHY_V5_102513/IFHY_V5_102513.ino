#include <MozziGuts.h>
#include <SampleHuffman.h>
#include "IFHY_CHORD.h"
#include <EventDelay.h>
#include <Oscil.h> 
#include <tables/sin2048_int8.h> 
#include <mozzi_midi.h>
#include <LowPassFilter.h>
#include <mozzi_fixmath.h>
#include <Line.h>
#include <Smooth.h>
#include <mozzi_analog.h>


//const int numReadings = 2;
//int readings[numReadings];      // the readings from the analog input
//int index = 0;                  // the index of the current reading
//int total = 0;                  // the running total
//int averageLight = 0;                // the average


#define INPUT_PIN 0

const int buttonPin = 4;     // the number of the pushbutton pin
boolean drone = false;
//sample
SampleHuffman IFHY(IFHY_CHORD_SOUNDDATA,IFHY_CHORD_HUFFMAN,IFHY_CHORD_SOUNDDATA_BITS);
EventDelay kTriggerDelay;
int LDR1,LDR2, a, a2;

//smoothness for Portamento 
float smoothness = 0.998f;
Smooth <unsigned int> aSmooth(smoothness);

const int threshold = 10;
//Low Pass Filter
//LowPassFilter lpf;


//sin wave
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

//musical scale
const int arraySize = 8; 
//float scale1[arraySize] = {
//  mtof(36.f), mtof(38.f),mtof(40.f),mtof(43.f),mtof(44.f),mtof(46.f),mtof(47.f),mtof(48.f)};

float scale1[arraySize] = {
  mtof(48.f), mtof(50.f),mtof(52.f),mtof(55.f),mtof(56.f),mtof(58.f),mtof(59.f),mtof(60.f)};

float scale2[arraySize] = {
  mtof(60.f), mtof(62.f),mtof(64.f),mtof(65.f),mtof(67.f),mtof(68.f),mtof(71.f), mtof(72.f)};

float scale3[arraySize] = {
  mtof(72.f), mtof(74.f),mtof(76.f),mtof(77.f),mtof(79.f),mtof(80.f),mtof(83.f),mtof(84.f)};


int newValue = 0;
int oldValue = 0;

void setup(){ 
  Serial.begin(115200);
  delay(200);
  startMozzi();
  aSin.setFreq(mtof(64.f)); 
  kTriggerDelay.set(200);
  a = 1;
  //lpf.setCutoffFreq(10000);


}

volatile unsigned int freq; 
boolean playing = false; 

void updateControl(){
  //int octave = int(mozziAnalogRead(A1));
  int change = 0; 
  int lastValuePot; 

  int octave = int(map(mozziAnalogRead(A1),380,615, 1,4));
  //Serial.println(octave);
  int pot;

  if(octave < 1){
    octave = 1;
  }

  newValue = mozziAnalogRead(INPUT_PIN);
  playing = (abs(newValue - oldValue) > threshold) ? true : false;
  if(playing) {
    if(octave == 1){

      if(playing) {
        pot = int(map(newValue,380, 610, 0, arraySize));
        for (int i = 0; i < arraySize; i++){
          if (pot == i){
            freq = scale1[i];

            //aSin.setFreq(scale1[i]); 

          }
        }
      }
    }  
    else if(octave == 2){
      pot = int(map(newValue,380, 610, 0, arraySize));
      //Serial.println(pot);
      for (int i = 0; i < arraySize; i++){
        if (pot == i){
          freq = scale2[i];
          //aSin.setFreq(scale2[i]); 
        }
      }
    }
    else if(octave == 3){
      pot = int(map( newValue,380, 610, 0, arraySize));
      //Serial.println(pot);
      for (int i = 0; i < arraySize; i++){
        if (pot == i){
          freq = scale3[i];
          //aSin.setFreq(scale3[i]); 
        }
      }
    }
    //  else if(octave == 4){
    //    pot = int(map(mozziAnalogRead(INPUT_PIN),380, 610, 0, arraySize));
    //    for (int i = 0; i < arraySize; i++){
    //      if (pot == i){
    //        aSin.setFreq(scale4[i]); 
    //      }
    oldValue = newValue;
  }
  else {
    
  }

  if(kTriggerDelay.ready()){
    IFHY.start();
    kTriggerDelay.start();
  }

  LDR1 = int(map(mozziAnalogRead(A5), 800, 1015, 1, 100)); 
  LDR2 = mozziAnalogRead(A4);

  //Serial.println(LDR1); 
  //Serial.println(LDR2);

  //
  if(LDR2 < 50){
    a = 1;
  }
  else{
    a = 0;
  }

  if(LDR1 < 50){
    a2 = 1;
  }
  else{
    a2 = 0;
  }  

}



int updateAudio(){
  int aSig;    

  int smoothedFreq = aSmooth.next(freq); // get the next filtered frequency
  aSin.setFreq(smoothedFreq);
  int b = (aSin.next()*a)>>4;

  //int b = aSin.next()>>3;
  //    Serial.print(a);
  //    Serial.print('\t');
  //    Serial.print(b);
  //    Serial.println();
  //aSig = lpf.next(IFHY.next() + b);
  aSig = (IFHY.next()*a2) + b; 
  //Serial.println(aSig);
  //Serial.println(aSig);
  return aSig; 
}

void loop(){
  audioHook();       
}












