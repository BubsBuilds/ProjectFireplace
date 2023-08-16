#include <ArduinoJson.h>

int pinMuxS0 = 2;
int pinMuxS1 = 3;
int pinMuxS2 = 4;

int pinMuxAO = A5;
int pinTherm1 = A6;
int pinTherm2 = A7;

bool muxA0[] = {0, 0, 0};
bool muxA1[] = {1, 0, 0}; //Soil2b
bool muxA2[] = {0, 1, 0}; //Soil2a
bool muxA3[] = {1, 1, 0};
bool muxA4[] = {0, 0, 1}; //Soil1a
bool muxA5[] = {1, 0, 1};
bool muxA6[] = {0, 1, 1}; //Soil1b
bool muxA7[] = {1, 1, 1};
bool muxCur[] = {0, 0, 0};

int soil1a;
int soil1b;
int soil2a;
int soil2b;
int temp1;
int temp2;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // Wait for serial connection
  }
  // Set up the command pins for the multiplexer
  pinMode(pinMuxS0, OUTPUT);
  pinMode(pinMuxS1, OUTPUT);
  pinMode(pinMuxS2, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    deserializeJson(doc, input);

    String command = doc["command"];
    if (command == "getTime") {
      StaticJsonDocument<200> response;
      response["status"] = "success";
      response["message"] = String(millis());
      serializeJson(response, Serial);
      Serial.println();
    }else if (command == "getAll"){
      getVals();
      StaticJsonDocument<200> response;
      response["therm1"] = temp1;
      response["therm2"] = temp2;
      response["soil1a"] = soil1a;
      response["soil1b"] = soil1b;
      response["soil2a"] = soil2a;
      response["soil2b"] = soil2b;
      serializeJson(response, Serial);
      Serial.println();
    }else if (command == "id"){
      StaticJsonDocument<200> response;
      response["id"] = "PF_DataMaker1";
      serializeJson(response, Serial);
      Serial.println();
    }
    // Add more command checks and responses here
  }
}
void getVals(){
  temp1 = analogRead(pinTherm1);
  delay(20);
  temp2 = analogRead(pinTherm2);
  delay(20);
  setMux("A4");
  delay(20);
  soil1a = analogRead(pinMuxAO);
  delay(20);
  setMux("A6");
  delay(20);
  soil1b = analogRead(pinMuxAO);
  delay(20);
  setMux("A2");
  delay(20);
  soil2a = analogRead(pinMuxAO);
  delay(20);
  setMux("A1");
  delay(20);
  soil2b = analogRead(pinMuxAO);
  delay(20);
}
void setMux(String ch){
  if (ch == "A0"){
    muxCur[0] = muxA0[0];
    muxCur[1] = muxA0[1];
    muxCur[2] = muxA0[2];
  } else if (ch == "A1"){
    muxCur[0] = muxA1[0];
    muxCur[1] = muxA1[1];
    muxCur[2] = muxA1[2];
  } else if (ch == "A2"){
    muxCur[0] = muxA2[0];
    muxCur[1] = muxA2[1];
    muxCur[2] = muxA2[2];
  }else if (ch == "A3"){
    muxCur[0] = muxA3[0];
    muxCur[1] = muxA3[1];
    muxCur[2] = muxA3[2];
  }else if (ch == "A4"){
    muxCur[0] = muxA4[0];
    muxCur[1] = muxA4[1];
    muxCur[2] = muxA4[2];
  }else if (ch == "A5"){
    muxCur[0] = muxA5[0];
    muxCur[1] = muxA5[1];
    muxCur[2] = muxA5[2];
  }else if (ch == "A6"){
    muxCur[0] = muxA6[0];
    muxCur[1] = muxA6[1];
    muxCur[2] = muxA6[2];
  }else if (ch == "A7"){
    muxCur[0] = muxA7[0];
    muxCur[1] = muxA7[1];
    muxCur[2] = muxA7[2];
  }
  digitalWrite(pinMuxS0, muxCur[0]);
  digitalWrite(pinMuxS1, muxCur[1]);
  digitalWrite(pinMuxS2, muxCur[2]);
}
