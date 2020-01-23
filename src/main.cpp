#include <Arduino.h>

int sensors[2] = { 0 };
int leftInput = A7;
int rightInput = A6;
int leftLight = 6;
int rightLight = 7;
int confidenceThreshold = 2;
int won = 0;
bool on = true;

/*
#pragma region ROAST
const int fn = 400;
const int hn = 200;
const int qn = 100;
const int f5 = 698;
const int c6 = 1047;
const int b5 = 988;
const int gh5 = 831;
const int ch6 = 1109;
const int dh6 = 1245;

int song[] = {
  f5,fn + hn,
  c6,hn,
  b5,qn,
  c6,qn,
  b5,qn,
  c6,qn,
  b5,hn,
  c6,hn,
  gh5,fn,
  f5,fn + hn,
  f5,hn,
  gh5,hn,
  c6,hn,
  ch6,fn,
  gh5,fn,
  ch6,fn,
  dh6,fn,
  c6,hn,
  ch6,hn,
  c6,hn,
  ch6,hn,
  c6,fn
  };

void setup() {
  Serial.begin(9600);
}

void loop() {

for (int i = 0; i < (sizeof(song)/sizeof(int)); i = i + 2){
  tone(8, song[i]);
  delay(song[i + 1]);
}

noTone(8);
delay(1000);

}
#pragma endregion
*/

int min_value[2] = { 1023, 1023 };
int max_value[2] = { 0 };

int black_threshold[2] = { 0 };
int grey_threshold[2] = { 511, 511 };
int white_threshold[2] = { 1023, 1023 };

int predict(int value, int sensor) {

    // if (num_prev_data == 1)  {
    //     min_value = 1023;
    //     max_value = 0;
    //     black_threshold = 0;
    //     grey_threshold = 511;
    //     white_threshold = 1023;
    // }

    if (value > max_value[sensor]) {
        max_value[sensor] = value;
        white_threshold[sensor] = (white_threshold[sensor] + max_value[sensor]) / 2;
        grey_threshold[sensor] = ((min_value[sensor] + max_value[sensor]) / 2 + grey_threshold[sensor]) / 2;
    }
    else if (value < min_value[sensor]) {
        min_value[sensor] = value;
        black_threshold[sensor] = (black_threshold[sensor] + min_value[sensor]) / 2;
        grey_threshold[sensor] = ((min_value[sensor] + min_value[sensor]) / 2 + grey_threshold[sensor]) / 2;
    }   

    int distance_to_black = abs(black_threshold[sensor] - value);
    int distance_to_grey = abs(grey_threshold[sensor] - value);
    int distance_to_white = abs(white_threshold[sensor] - value);

    if (distance_to_black < distance_to_grey && distance_to_black < distance_to_white) {
        // if (min_value[sensor] > black_threshold[sensor]) black_threshold[sensor] = min_value[sensor];
        black_threshold[sensor] = (black_threshold[sensor] + value) / 2;
        return 0;
    }

    if (distance_to_grey < distance_to_white && distance_to_grey < distance_to_black) {
        grey_threshold[sensor] = (grey_threshold[sensor] + value) / 2;
        return 1;
    }

    // if (max_value[sensor] < white_threshold) white_threshold = max_value[sensor];
    white_threshold[sensor] = (white_threshold[sensor] + value) / 2;
    return 2;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(leftLight, OUTPUT);
  pinMode(rightLight, OUTPUT);
  Serial.begin(9600);
}

void rotateRight(float a) {
  digitalWrite(rightLight, HIGH);
  digitalWrite(leftLight, LOW);
}

void rotateLeft(float a) {
  digitalWrite(leftLight, HIGH);
  digitalWrite(rightLight, LOW);
}

void moveForward() {
  digitalWrite(leftLight, HIGH);
  digitalWrite(rightLight, HIGH);
}

bool isBlack(int value) {
  return value == 0;
}
bool isGrey(int value) {
  return value == 1;
}
bool isWhite(int value) {
  return value == 2;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  if (won > confidenceThreshold) {
    on = !on;
        if (on) {
          moveForward();
        } else {
          digitalWrite(leftLight, LOW);
          digitalWrite(rightLight, LOW);
        }
        return;
  }
  sensors[0] = predict(analogRead(leftInput), 0);
  sensors[1] = predict(analogRead(rightInput), 0);

  if (isBlack(sensors[1])) {
    if (isBlack(sensors[0])) {
      // B&B - explore / go straight
      moveForward();
      won = 0;
    }
    else if (isGrey(sensors[0])) {
      // B&G - turn counter-clockwise
      rotateLeft(0.05);
      won = 0;
    }
    else if (isWhite(sensors[0])) {
      // B&W - turn clockwise
      rotateRight(0.05);
      won = 0;
    }
  }
  if (isGrey(sensors[1])) {
    if (isBlack(sensors[0])) {
      // G&B - turn clockwise
      rotateRight(0.02);
      won = 0;

    }
    else if (isGrey(sensors[0])) {
      // G&G - go straight
      moveForward();
    }
    else if (isWhite(sensors[0])) {
      // G&W - turn counter-clockwise
      rotateLeft(0.05);
      won++;
    }
  }
  if (isWhite(sensors[1])) {
    if (isBlack(sensors[0])) {
      // W&B - turn counter-clockwise
      rotateLeft(0.02);
      won = 0;
    }
    else if (isGrey(sensors[0])) {
      // W&G - turn clockwise
      rotateRight(0.04);
      won++;
    }
    else if (isWhite(sensors[0])) {
      // W&W - go straight / win (stop) if confident enough
      if (won < confidenceThreshold) {
        moveForward();
        won++;
      }
    }
  }
}