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
  input = Serial.readString();
  if(input.endsWith(";")){
    const int endInput = input.length();
    char inputChar[input.length() +1];
    input.remove(endInput-1);
    input.toCharArray(inputChar, input.length()+1);
    channelInput = atoi(strtok(inputChar, ":"));
    pitchInput = atoi(strtok(NULL, ":"));
    velocityInput = atoi(strtok(NULL, ";"));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
}


void  loop() {
  input = Serial.readString();
  if(input.endsWith(";")){
    const int endInput = input.length();
    char inputChar[input.length() +1];
    input.remove(endInput-1);
    input.toCharArray(inputChar, input.length()+1);
    channelInput = atoi(strtok(inputChar, ":"));
    pitchInput = atoi(strtok(NULL, ":"));
    velocityInput = atoi(strtok(NULL, ";"));
    sendMIDI(channelInput, pitchInput, velocityInput);
  }


}