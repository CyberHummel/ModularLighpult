int buttonPin = A2;
int buttonSmooth = 0;

int led1 = 8;
int led2 = 9;
int led3 = 10;
int led4 = 11;

bool button1State = false;
bool button2State = false;
bool button3State = false;
bool button4State = false;

const int debounceDelay = 10; // Adjust this value based on your requirements
unsigned long lastDebounceTime = 0;
int buttonState = HIGH;  // current state of the button
int lastButtonState = HIGH; // previous state of the button

void checkButtons() {
  buttonSmooth = buttonSmooth * 0.8 + analogRead(buttonPin) * 0.2;

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
      if (buttonSmooth > 935 && buttonSmooth < 945) {
        button1State = !button1State;
        digitalWrite(led1, button1State ? HIGH : LOW);
      } else if (buttonSmooth > 945 && buttonSmooth < 965) {
        button2State = !button2State;
        digitalWrite(led2, button2State ? HIGH : LOW);
      } else if (buttonSmooth > 965 && buttonSmooth < 985) {
        button3State = !button3State;
        digitalWrite(led3, button3State ? HIGH : LOW);
      } else if (buttonSmooth > 985 && buttonSmooth < 1005) {
        button4State = !button4State;
        digitalWrite(led4, button4State ? HIGH : LOW);
      }
    }
  }

  // Save the current button state for the next loop iteration
  lastButtonState = reading;
}

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  pinMode(buttonPin, INPUT);

  Serial.begin(1000000);
}

void loop() {
  checkButtons();
}
