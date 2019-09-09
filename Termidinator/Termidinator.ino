// Constants

const bool midiMode = true;
const byte octaveMinusButtonPin = 11;
const byte octavePlusButtonPin = 12;
const byte pianoNaturalKeys[7][3] = { {4, 0, 2}, {5, 2, 3}, {6, 4, 5}, {7, 5, 7}, {8, 7, 11}, {9, 9, 13}, {10, 11, 17} }; // {Pin, Value, Prime Number}
const byte pianoNaturalKeysSize = sizeof pianoNaturalKeys/sizeof pianoNaturalKeys[0];
const byte pianoSharpKeys[5][3] = { {4, 1, 2}, {5, 3, 3}, {7, 6, 5}, {8, 8, 7}, {9, 10, 11} }; // {Pin, Value, Prime Number}
const byte pianoSharpKeysSize = sizeof pianoSharpKeys/sizeof pianoSharpKeys[0];



// Variables

bool isSharpKey = false;
bool octaveMinusButtonState = LOW;
bool octavePlusButtonState = LOW;
double octaveValue = 5;
int magicNumber = 1;



// Default Functions

void setup() {
  
  if (midiMode == true) {
    Serial.begin(31250);
  } else {
    Serial.begin(9600);
  }

  pinMode(octaveMinusButtonPin, INPUT);
  pinMode(octavePlusButtonPin, INPUT);
  
  for (int i=0; i<pianoNaturalKeysSize; i++) {
    pinMode(pianoNaturalKeys[i][0], INPUT);
  }
  
}

void loop() {

  if (digitalRead(octaveMinusButtonPin) == HIGH && octaveMinusButtonState == LOW && octaveValue >= 0) {
    octaveMinusButtonState = HIGH;
    if (isSharpKey == false && octaveValue > 0) {
      isSharpKey = true;
      octaveValue -= 1;
    } else {
        isSharpKey = false;
    }
  } else if (digitalRead(octaveMinusButtonPin) == LOW && octaveMinusButtonState == HIGH) {
    octaveMinusButtonState = LOW;
  }

  if (digitalRead(octavePlusButtonPin) == HIGH && octavePlusButtonState == LOW && octaveValue <= 10) {
    octavePlusButtonState = HIGH;
    if (isSharpKey == true && octaveValue < 10) {
      isSharpKey = false;
      octaveValue += 1;
    } else {
      isSharpKey = true;
    }
  } else if (digitalRead(octavePlusButtonPin) == LOW && octavePlusButtonState == HIGH) {
    octavePlusButtonState = LOW;
  }

  if (isSharpKey == false) {
    checkNoteStatus(pianoNaturalKeys, pianoNaturalKeysSize);
  } else {
    checkNoteStatus(pianoSharpKeys, pianoSharpKeysSize);
  }
  
}



// Functions

void checkNoteStatus(const byte pianoKeys[][3], const byte pianoKeysSize) {
  
  for (int i=0; i<pianoKeysSize; i++) {
    if (digitalRead(pianoKeys[i][0]) == HIGH && magicNumber % pianoKeys[i][2] != 0) {
      sendMidi(0x90, 12 * octaveValue + pianoKeys[i][1], 100);
      magicNumber = magicNumber * pianoKeys[i][2];
    } else if (digitalRead(pianoKeys[i][0]) == LOW && magicNumber % pianoKeys[i][2] == 0) {
      sendMidi(0x80, 12 * octaveValue + pianoKeys[i][1], 100);
      magicNumber = magicNumber / pianoKeys[i][2];
    }
  }
  
}

void sendMidi(int statusByte, int dataByte1, int dataByte2) {

  if (midiMode == true) {
    Serial.write(statusByte);
    Serial.write(dataByte1);
    Serial.write(dataByte2);
  } else {
      if (dataByte2 > 0) {
        Serial.print("Note ");
        Serial.print(dataByte1);
        Serial.print(" is active:  ");
        Serial.println(dataByte2);
      } else {
        Serial.print("Note ");
        Serial.print(dataByte1);
        Serial.println(" is off");
      }
  }
  
}
