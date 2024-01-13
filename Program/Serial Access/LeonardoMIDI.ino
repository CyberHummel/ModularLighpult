#include "OnePinCapSense.h"

const OnePinCapSense opcs = OnePinCapSense();
const int touchThres = 50;
const int touchedState = 300;

const int PhysicalfaderNum = 1;
const int faderPins[PhysicalfaderNum] = {A0};
const int motorsUp[PhysicalfaderNum] = {2};
const int motorsDown[PhysicalfaderNum] = {3};


int fadersMin[PhysicalfaderNum] = {0};
int fadersMax[PhysicalfaderNum] = {0};

int faderSenses[PhysicalfaderNum] = {0};
int faderTouchPins[PhysicalfaderNum] = {6};

int faderValues[PhysicalfaderNum] = {0};
int faderValuesOld[PhysicalfaderNum] = {0};
int faderValuesMapped[PhysicalfaderNum] = {0};

String input;

int channelInput;
int pitchInput;
int velocityInput;

const int VirtualFaderNumA0 = 4;
const int pitchValuesA0[VirtualFaderNumA0] = {21, 22, 23, 24};

int selectedA0 = 0;
int velocityValuesA0[VirtualFaderNumA0] = {0, 0, 0, 0};

const int buttonsPin = A2;
const int debounceDelay = 10;
unsigned long lastDebounceTime = 0;
int buttonState = HIGH;
int lastButtonState = HIGH;

int buttonSmooth = 0;

bool buttonsState[VirtualFaderNumA0] = {false, false, false, false};
const int ledPins[VirtualFaderNumA0] = {8, 9, 10, 11};



void checkButtons() {
  buttonSmooth = buttonSmooth * 0.8 + analogRead(buttonsPin) * 0.2;
  int reading = (buttonSmooth > 500) ? HIGH : LOW; // Assuming HIGH when not pressed and LOW when pressed

  if (reading != lastButtonState) {
    // Reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // Toggle the corresponding button state
      if (buttonSmooth > 850 && buttonSmooth < 890) {
        buttonsState[0] = !buttonsState[0];
        digitalWrite(ledPins[0], buttonsState[0] ? HIGH : LOW);
        if(buttonsState[0] == 1){
          selectedA0 = 0;
          setFader(velocityValuesA0[0], 60, 0);
        }
      } else if (buttonSmooth > 890 && buttonSmooth < 910) {
        buttonsState[1] = !buttonsState[1];
        digitalWrite(ledPins[1], buttonsState[1] ? HIGH : LOW);
        if(buttonsState[1] == 1){
          selectedA0 = 1;
          setFader(velocityValuesA0[1], 60, 0);
        }
      } else if (buttonSmooth > 910 && buttonSmooth < 930) {
        buttonsState[2] = !buttonsState[2];
        digitalWrite(ledPins[2], buttonsState[2] ? HIGH : LOW);
        if(buttonsState[2] == 1){
          selectedA0 = 2;
          setFader(velocityValuesA0[2], 60, 0);
        }
      } else if (buttonSmooth > 930 && buttonSmooth < 1005) {
        buttonsState[3] = !buttonsState[3];
        digitalWrite(ledPins[3], buttonsState[3] ? HIGH : LOW);
        if(buttonsState[3] == 1){
          selectedA0 = 3;
          setFader(velocityValuesA0[3], 60, 0);
        }
      }
    }
  }
  // Save the current button state for the next loop iteration
  lastButtonState = reading;
}



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

      if(channelInput == 1 && velocityInput <= 127){
        switch(pitchInput){
          case 21:
            if(selectedA0 == 0){
              setFader(velocityInput, 60, 0);
            }
            velocityValuesA0[0] = velocityInput;
            break;
          case 22:
            if(selectedA0 == 1){
              setFader(velocityInput, 60, 0);
            }
            velocityValuesA0[1] = velocityInput;
            break;
          case 23:
            if(selectedA0 == 2){
              setFader(velocityInput, 60, 0);
            }
            velocityValuesA0[2] = velocityInput;
            break;
          case 24:
            if(selectedA0 == 3){
              setFader(velocityInput, 60, 0);
            }
            velocityValuesA0[3] = velocityInput;
            break;
            }

      }
    }

  }






bool checkTouch(int faderNum){
  faderSenses[faderNum] = faderSenses[faderNum]*0.7 + opcs.readCapacitivePin(faderTouchPins[faderNum])*0.3;
  if(faderSenses[faderNum] > touchedState -touchThres){
    return true;
  }else{
    return false;
  }
}


void initFaders(){

  for(int i = 0; i < PhysicalfaderNum; i++){
    pinMode(motorsUp[i], OUTPUT);
    pinMode(motorsDown[i], OUTPUT);
    digitalWrite(motorsUp[i], HIGH);
  }

  delay(850);

  for(int j = 0; j < PhysicalfaderNum; j++){
    digitalWrite(motorsUp[j], LOW);
    fadersMax[j] = analogRead(faderPins[j]);
    digitalWrite(motorsDown[j], HIGH);
  }

  delay(850);

  for(int k = 0; k < PhysicalfaderNum; k++){
    digitalWrite(motorsDown[k], LOW);
    fadersMin[k] = analogRead(faderPins[k]);
  }

}

void setOutputs_INPUTS(){
  for(int i = 0; i < VirtualFaderNumA0; i ++){
    pinMode(ledPins[i], OUTPUT);
  }
}

void setFader(int position, int margin, int fader){
  position = map(position, 0, 127, fadersMin[fader], fadersMax[fader]);
  int currentPosition = analogRead(faderPins[fader]);



  if(position > currentPosition){
    do{
      if(checkTouch(fader)){
        break;
      }
      currentPosition = analogRead(faderPins[fader]);
      digitalWrite(motorsUp[fader], HIGH);
    }while(position > currentPosition + margin);
    digitalWrite(motorsUp[fader], LOW);
  }else if(position < currentPosition){
      do{
        if(checkTouch(fader)){
          break;
        }
      currentPosition = analogRead(faderPins[fader]);
      digitalWrite(motorsDown[fader], HIGH);
    }while(position < currentPosition - margin);
    digitalWrite(motorsDown[fader], LOW);
  }else{
    digitalWrite(motorsUp[fader], LOW);
    digitalWrite(motorsDown[fader], LOW);
  }
}




void setup() {
  // put your setup code here,   to run once:
  setOutputs_INPUTS();
  initFaders();
  Serial.begin(1000000);
  Serial.setTimeout(1);
}

void loop() {
  // put your main code here, to run repeatedly:

  checkButtons();
  readMIDI();

  if(!Serial.available()){
    for(int i = 0; i < PhysicalfaderNum; i++){
      if(checkTouch(i)){
        faderValues[i] = faderValues[i]*0.64 + analogRead(faderPins[i])*0.46;
        faderValuesMapped[i] = map(faderValues[i], fadersMin[i], fadersMax[i], 0, 100);
        if(faderValuesMapped[i] != faderValuesOld[i]){
          for(int j = 0; j < VirtualFaderNumA0; j++){
            switch (selectedA0){
              case 0:
                if(buttonsState[0] == 1){
                  velocityValuesA0[0] = faderValuesMapped[i];
                }
              case 1:
                if(buttonsState[1] == 1){
                  velocityValuesA0[1] = faderValuesMapped[i];
                }
              case 2:
                if(buttonsState[2] == 1){
                  velocityValuesA0[2] = faderValuesMapped[i];
                }
              case 3:
                if(buttonsState[3] == 1){
                  velocityValuesA0[3] = faderValuesMapped[i];
                }
            }
            if(buttonsState[j] == true){
              sendMIDI(1, pitchValuesA0[j], faderValuesMapped[i]);
              Serial.flush();
            }
          }
          faderValuesOld[i] = faderValuesMapped[i];
      }
    }
  }
  }
}
