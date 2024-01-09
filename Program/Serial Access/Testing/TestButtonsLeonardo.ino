int ledPin1 = 8;
int ledPin2 = 9;
int ledPin3 = 10;
int ledPin4 = 11;

int smoothButton = 0;
bool buttonSelect1 = false;
bool buttonSelect2 = false;
bool buttonSelect3 = false;
bool buttonSelect4 = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  Serial.begin(9600);

}
void checkButtons(){
  smoothButton = smoothButton*0.8 + analogRead(A1)*0.2;
  if(smoothButton > 100 && smoothButton < 400){
    if(buttonSelect1 == false){
        buttonSelect1 = true;
    }else{
      buttonSelect1 = false;
    }
  }else if(smoothButton > 400 && smoothButton < 650){
    if(buttonSelect2 == false){
        buttonSelect2 = true;
    }else{
      buttonSelect2 = false;
    }
  }else if(smoothButton > 650 && smoothButton < 800){
    if(buttonSelect3 == false){
        buttonSelect3 = true;
    }else{
      buttonSelect3 = false;
    }

  }else if (smoothButton > 800){
      if(buttonSelect4 == false){
        buttonSelect4 = true;
    }else{
      buttonSelect4 = false;
    }
  }else{

  }

}

void setLeds(){
  if(buttonSelect1 == true){
    digitalWrite(ledPin1, HIGH);
  }else{
    digitalWrite(ledPin1, LOW);
  }

  if(buttonSelect2 == true){
    digitalWrite(ledPin2, HIGH);
  }else{
    digitalWrite(ledPin2, LOW);
  }

  if(buttonSelect3 == true){
    digitalWrite(ledPin3, HIGH);
  }else{
    digitalWrite(ledPin3, LOW);
  }

  if(buttonSelect4 == true){
    digitalWrite(ledPin4, HIGH);
  }else{
    digitalWrite(ledPin4, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  checkButtons();
  Serial.println(buttonSelect1);

}
