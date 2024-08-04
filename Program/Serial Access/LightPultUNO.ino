String input;

int channelInput;
int pitchInput;
int velocityInput;

#include <ShiftRegister74HC595.h>

const int DS = 2;
const int STCP = 4;
const int SHCP = 5;

ShiftRegister74HC595<1> leds(DS, SHCP, STCP);

#include "OnePinCapSense.h"

const OnePinCapSense opcs = OnePinCapSense();
const int touchThres = 50;
const int touchedState = 300;

//virtualFaders:

int v1Val=0;
int v2Val=0;
int v3Val=0;
int v4Val=0;


//Faders:
int fader1Pin=A0;
int fader1Value=0;
int fader1ValueOld=0;
int faderTouch1Pin=12;
int faderSense1=0;
int fader1Max=0;
int fader1Min=0;

//Motors:
int fader1Up=9;
int fader1Down=8;

//Buttons:
const int buttonsPin = A2;
int buttonsVal = 0;
int lastButtonVal = 0;

const int b1Val = 142;
const int b2Val = 252;
const int b3Val = 436;
const int b4Val = 1021;

bool b1State = false;
bool b2State = false;
bool b3State = false;
bool b4State = false;

unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 20; 

void sendMIDI(int channel, int pitch, int velocity){
  Serial.print(channel);
  Serial.print(":");
  Serial.print(pitch);
  Serial.print(":");
  Serial.print(velocity);
  Serial.println(";");
  Serial.flush();
}


void readMIDI(){
  if(Serial.available()){
    input = Serial.readString();
      const int endInput = input.length();
      char inputChar[input.length() +1];
      input.remove(endInput-1);
      input.toCharArray(inputChar, input.length()+1);
      channelInput = atoi(strtok(inputChar, ":"));
      pitchInput = atoi(strtok(NULL, ":"));
      velocityInput = atoi(strtok(NULL, ";"));
      if(channelInput == 1 && velocityInput < 127 && velocityInput >= 0){
        if(pitchInput == 21){
          //TODO: fix Button 1
        } else if(pitchInput == 22){
          v2Val = velocityInput;
          if(b2State == true){
            setFader(v2Val, 5);
          }
        } else if(pitchInput == 23){
          v3Val = velocityInput;
          if(b3State == true){
            setFader(v3Val, 5);
          }          
        } else if(pitchInput == 24){
          v4Val = velocityInput;
          if(b4State == true){
            setFader(v4Val, 5);
          }          
        } else{
          sendMIDI(channelInput, pitchInput, velocityInput);
        }
      }
  }
}


void checkButtons(int margin){
  int reading = analogRead(buttonsPin);

  if (reading != lastButtonVal) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonsVal) {
      buttonsVal = reading;

      if(buttonsVal > b1Val-20 && buttonsVal < b1Val+40){
        b1State = !b1State;
        leds.set(1, b1State);
        if(b1State == true){
          setFader(v1Val, 5);
        }
      }else if(buttonsVal > b2Val-20 && buttonsVal < b2Val+40){
        b2State = !b2State;
        leds.set(2, b2State);
        if(b2State == true){
          setFader(v2Val, 5);
        }                
      }else if(buttonsVal > b3Val-20 && buttonsVal < b3Val+40){
        b3State = !b3State;
        leds.set(3, b3State);
        if(b3State == true){
          setFader(v3Val, 5);
        }                
      }else if(buttonsVal > b4Val-20 && buttonsVal < b4Val+40){
        b4State = !b4State;
        leds.set(4, b4State);
        if(b4State == true){
          setFader(v4Val, 5);
        }                
      }
    }
  }

  lastButtonVal = reading;
}


bool checkTouch(){
  faderSense1 = faderSense1*0.7 + opcs.readCapacitivePin(faderTouch1Pin)*0.3;
  if(faderSense1 > touchedState- touchThres){
    return true;
  }else{
    return false;
  }
}

void readFader(){
  if(checkTouch()){
    fader1Value = fader1Value*0.64 + analogRead(fader1Pin)*0.36;
    if(fader1Value != fader1ValueOld){
      int mapped = map(fader1Value, fader1Min, fader1Max, 0 , 127);
  
      if(b1State == true){
        v1Val = mapped;
        sendMIDI(1, 21, v1Val);
      }
      if(b2State == true){
        v2Val = mapped;
        sendMIDI(1, 22, v2Val);
      }
      if(b3State == true){
        v3Val = mapped;
        sendMIDI(1, 23, v3Val);
      }
      if(b4State == true){
        v4Val = mapped;
        sendMIDI(1, 24, v4Val);
      }      
    }
    fader1ValueOld = fader1Value;
  }
}

void initFader(){
  digitalWrite(fader1Up, HIGH);
  delay(1000);
  digitalWrite(fader1Up, LOW);
  fader1Max = analogRead(fader1Pin);
  Serial.println(fader1Max);
  digitalWrite(fader1Down, HIGH);
  delay(1000);
  digitalWrite(fader1Down, LOW);
  fader1Min = analogRead(fader1Pin);
  Serial.println(fader1Min);
}

void setOutputs(){
  pinMode(fader1Up, OUTPUT);
  pinMode(fader1Down, OUTPUT);
}

void setFader(int position, int margin){
  if(!checkTouch()){
      int currentPos = map(analogRead(fader1Pin), fader1Min, fader1Max, 0 , 127);
      
      if(position > currentPos){
        do{
          if(checkTouch()){
            break;
          }
          currentPos = map(analogRead(fader1Pin), fader1Min, fader1Max, 0 , 127);
          digitalWrite(fader1Up, HIGH);
        }while(position > currentPos + margin);
        digitalWrite(fader1Up, LOW);
      } else if (position < currentPos) {
      do{
        if(checkTouch()){
          break;
        }
        currentPos = map(analogRead(fader1Pin), fader1Min, fader1Max, 0 , 127);
        digitalWrite(fader1Down, HIGH);
      }while(position < currentPos-margin);
        digitalWrite(fader1Down, LOW);
      }else {
        digitalWrite(fader1Up, LOW);
        digitalWrite(fader1Down, LOW);
      }
  }
}

void setup(){
  Serial.begin(1000000); 
  Serial.setTimeout(1);
  initFader();
  setFader(126, 5);
  delay(1000);
  setFader(0, 5);
}

void loop(){
  readMIDI();
  checkButtons(30);
  readFader();

}
