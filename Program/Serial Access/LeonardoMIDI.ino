#include "OnePinCapSense.h"
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();


OnePinCapSense opcs = OnePinCapSense();
const int touchThres = 50;
int touchedState = 300;

const int faderPin = A0;
const int motorUp = 2;
const int motorDown = 3;
const int button1 = 9;
const int led1 = 8;

int faderMin;
int faderMax;

int faderSense = 0;
int faderTouchPin = 4;

int faderValue = 0;
int faderValueOld = 0;

bool settingFader = false;

String input;

int channelInput;
int pitchInput;
int velocityInput;


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
    if(input.endsWith(";")){
      const int endInput = input.length();
      char inputChar[input.length() +1];
      input.remove(endInput-1);
      input.toCharArray(inputChar, input.length()+1);
      channelInput = atoi(strtok(inputChar, ":"));
      pitchInput = atoi(strtok(NULL, ":"));
      velocityInput = atoi(strtok(NULL, ";"));
      if(channelInput == 1){
        switch(pitchInput){
          case 21:
            setFader(velocityInput, 30);
        }
      }
      Serial.flush();
    }
  }
}





bool checkTouch(){
  faderSense = faderSense*0.7 + opcs.readCapacitivePin(faderTouchPin)*0.3;
  if(faderSense > touchedState -touchThres){
    return true;
  }else{
    return false;
  }
}


void initFaders(){
  pinMode(motorUp, OUTPUT);
  pinMode(motorDown, OUTPUT);

  digitalWrite(motorUp, HIGH);
  delay(500);
  digitalWrite(motorUp, LOW);
  faderMax = analogRead(faderPin);
  digitalWrite(motorDown, HIGH);
  delay(500);
  digitalWrite(motorDown, LOW);
  faderMin = analogRead(faderPin);
}

void setOutputs_INPUTS(){
  pinMode(button1, INPUT);
  pinMode(led1, OUTPUT);
}

void setFader(int position, int margin){
  position = map(position, 0, 127, faderMin, faderMax);
  int currentPosition = analogRead(faderPin);

  if(position > currentPosition){
    do{
      settingFader = true;
      if(checkTouch()){
        break;
      }
      currentPosition = analogRead(faderPin);
      Serial.println(currentPosition);
      digitalWrite(motorUp, HIGH);
    }while(position > currentPosition + margin);
    digitalWrite(motorUp, LOW);
    settingFader = false;
  }else if(position < currentPosition){
      do{
        settingFader = true;
        if(checkTouch()){
          break;
        }
      currentPosition = analogRead(faderPin);
      Serial.println(currentPosition);
      digitalWrite(motorDown, HIGH);
    }while(position < currentPosition - margin);
    digitalWrite(motorDown, LOW);
    settingFader = false;
  }else{
    digitalWrite(motorUp, LOW);
    digitalWrite(motorDown, LOW);
    settingFader = false;
  }
}




void setup() {
  // put your setup code here,   to run once:
  setOutputs_INPUTS();
  initFaders();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

  readMIDI();


  if(settingFader != true){
    faderValue = faderValue*0.5 + analogRead(faderPin)*0.5;
    if(faderValue != faderValueOld){
      sendMIDI(1, 21, map(faderValue, faderMin, faderMax, 0, 127));
      Serial.flush();
      faderValueOld = faderValue;
    }
  }
}
