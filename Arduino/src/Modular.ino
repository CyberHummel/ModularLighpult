#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "OnePinCapSense-master/OnePinCapSense.h"

LiquidCrystal_I2C lcd(0x27, 16, 4);
OnePinCapSense opcs = OnePinCapSense();

const int touchThres = 50;
int touchedState = 300;

const int faderCount = 1;
const int faderPins[faderCount] = {A0};
const int faderTouchPins[faderCount] = {4};
int faderMin[faderCount] = {};
int faderMax[faderCount] = {};

int faderSmooth[faderCount] = {0};
int faderSense[faderCount] = {0};
bool faderTouched[faderCount] = {false};

const int motorsUp[faderCount] = {2};
const int motorsDown[faderCount] = {3};


void initFaders(){
    for(int i = 0; i < faderCount; i++){
        pinMode(motorsUp[i], OUTPUT);
        pinMode(motorsDown[i], OUTPUT);
        digitalWrite(motorsUp[i], HIGH);
    }
    delay(500);
    for(int i = 0; i < faderCount; i++){
        digitalWrite(motorsUp[i], LOW);
        faderMax[i] = analogRead(faderPins[i]);
    }
    for(int i = 0; i < faderCount; i++){
        digitalWrite(motorsDown[i], HIGH);
    }
    delay(500);
    for(int i = 0; i < faderCount; i++){
        digitalWrite(motorsDown[i], LOW);
        faderMin[i] = analogRead(faderPins[i]);
    }
}



void checkTouch(int faderIndex){
    faderSense[faderIndex] = faderSense[faderIndex]*0.7 + opcs.readCapacitivePin(faderTouchPins[faderIndex])*0.3;
    if(faderSense[faderIndex] > touchedState -touchThres){
        faderTouched[faderIndex] = true;
    }else{
        faderTouched[faderIndex] = false;
    }
}

void setFader(int faderIndex, int position, int margin){
    position = map(position, 0, 127, faderMin[faderIndex], faderMax[faderIndex]);
    int currentVal = analogRead(faderPins[faderIndex]);

    if(position > currentVal){
        while (analogRead(faderPins[faderIndex]) > position+margin){
            checkTouch(faderIndex);
            if(faderTouched[faderIndex] == true){
                break;
            }else{
                digitalWrite(motorsUp[faderIndex], HIGH);
            }
        }
        digitalWrite(motorsUp[faderIndex], LOW);
    }else if(position < currentVal){
        while (analogRead(faderPins[faderIndex]) > position+margin){
            checkTouch(faderIndex);
            if(faderTouched[faderIndex] == true){
                break;
            }else{
                digitalWrite(motorsDown[faderIndex], HIGH);
            }
        }
        digitalWrite(motorsDown[faderIndex], LOW);
    }else{
        digitalWrite(motorsDown[faderIndex], LOW);
        digitalWrite(motorsUp[faderIndex], LOW);
    }
}



void setup(){
    lcd.init();
    lcd.backlight();

    initFaders();
}

void loop(){

}