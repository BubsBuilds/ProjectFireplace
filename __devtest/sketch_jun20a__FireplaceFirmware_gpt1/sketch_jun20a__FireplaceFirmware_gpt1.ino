#include <Servo.h>

Servo resServo;
Servo distServo;

int resValues[5] = {500, 1000, 1500, 1900, 1250}; // corresponds to 'res1', 'res2', 'res3', 'res4', 'off'
int distValues[5] = {1700,1300, 900, 500, 1500}; // corresponds to 'tar1', 'tar2', 'tar3', 'off'

int pumpPin = 2;
int v1Pin = 6;
int v2Pin = 7;
bool pumpState = false;
bool resChange = false;
bool tarChange = false;

int tar1a;
int tar1b;
int tar2a;
int tar2b;
int tar1av;
int tar2av;
int tar1 = 220;
int tar2 = 300;

void setup() {
  Serial.begin(9600);
  pinMode(pumpPin, OUTPUT);
  pinMode(v1Pin, OUTPUT);
  pinMode(v2Pin, OUTPUT);
  pinMode(5, INPUT);
  digitalWrite(pumpPin,HIGH); 
  digitalWrite(v1Pin, HIGH);
  digitalWrite(v2Pin, HIGH);
  
  resServo.attach(3);
  distServo.attach(9);
}

void loop() {
  if(digitalRead(5) == 1) {
    digitalWrite(pumpPin, LOW);
  }else {
    digitalWrite(pumpPin, HIGH);
  }
  if(Serial.available() > 0) {
    String command = Serial.readString();;
    //Serial.println("Command received: " + command);
    handleCommand(command);
    delay(20);
  }else {
  tar1a = analogRead(A0);
  tar1b = analogRead(A1);
  tar2a = analogRead(A2);
  tar2b = analogRead(A3);
  tar1av = (tar1a+tar1b)/2;
  tar2av = (tar2a+tar2b)/2;
  if (tar1av > tar1){
    Serial.print("tar1 dry, starting hydro cycle. Measured val: ");
    Serial.println(tar1av);
    runHydro(1);
    
  }
  if (tar2av > tar2){
    Serial.print("tar2 dry, starting hydro cycle. Measured val: ");
    Serial.println(tar2av);
    runHydro(0);
  }
  Serial.print("Measured vals:   ");
  Serial.print(tar1a);
  Serial.print(" --  ");
  Serial.print(tar1b);
  Serial.print(" --  ");
  Serial.print(tar2a);
  Serial.print(" --  ");
  Serial.print(tar2b);
  Serial.print(" -- -- -- ");
  Serial.print((tar1a+tar1b)/2);
  Serial.print(" - - - ");
  Serial.println((tar2a+tar2b)/2);
  
  delay(1000);
  }
}

void runHydro(int ch) {
  if (ch == 0){
    //This runs a 10 sec nute, 15 sec purge routine for the tomatoes
    //First move the 'target' valve
    handleMoveCommand("d tar2");
    // Then move the 'res' to res2
    handleMoveCommand("r res2");
    // Then turn on the pump for 10 seconds
    digitalWrite(pumpPin, LOW);
    delay(5000);
    delay(5000); // I'm splitting this to two, 5 sec delays because I've had issues with long delays.
    delay(5000);
    delay(5000);
    // With the pump still on, go ahead and switch res valve to vent
    digitalWrite(pumpPin, HIGH);
    delay(50);
    handleMoveCommand("r res3");
    digitalWrite(pumpPin, LOW);
    delay(50);
    //wait another 15 seconds
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    //Now pump off
    digitalWrite(pumpPin, HIGH);
    delay(20); //short delay for pin state change buffer
    // Close the valves with the pump still on to prevent back flow
    handleMoveCommand("d off");
    handleMoveCommand("r off");
    
    
  }else if (ch == 1){
    //This runs a 10 sec nute, 15 sec purge routine for the peppers
    //First move the 'target' valve
    handleMoveCommand("d tar2");
    // Then move the 'res' to res2
    handleMoveCommand("r res2");
    // Then turn on the pump for 10 seconds
    digitalWrite(pumpPin, LOW);
    delay(5000);
    delay(5000); // I'm splitting this to two, 5 sec delays because I've had issues with long delays.
    digitalWrite(pumpPin, HIGH);
    // With the pump still on, go ahead and switch res valve to vent
    handleMoveCommand("r res3");
    digitalWrite(pumpPin, LOW);
    //wait another 15 seconds
    delay(5000);
    delay(5000);
    delay(5000);
    delay(5000);
    // Close the valves with the pump still on to prevent back flow
    handleMoveCommand("d off");
    handleMoveCommand("r off");
    //Now pump off
    digitalWrite(pumpPin, HIGH);
    delay(20); //short delay for pin state change buffer
    
  }
}

void handleCommand(String command) {
  if(command.startsWith("GET ")) {
    handleGetCommand(command.substring(4));
  } 
  else if(command.startsWith("SET")) {
    if(pumpState == true) {
      digitalWrite(pumpPin, HIGH);
      pumpState = false;
    } else if(pumpState == false) {
      digitalWrite(pumpPin, LOW);
      pumpState = true;
    }
    //handleSetCommand(command.substring(4));
  }
  else if(command.startsWith("MOVE ")) {
    handleMoveCommand(command.substring(5));
  }
  else if(command.startsWith("HELP")) {
    handleHelpCommand();
  }else if (command.startsWith("QUERY")) {
    Serial.flush();
    //delay(500);
    tar1a = analogRead(A0);
    tar1b = analogRead(A1);
    tar2a = analogRead(A2);
    // Generate JSON structure
    Serial.print("{");
    Serial.print("\"tar1a\":"); Serial.print(tar1a); Serial.print(",");
    Serial.print("\"tar1b\":"); Serial.print(tar1b); Serial.print(",");
    Serial.print("\"tar2a\":"); Serial.print(tar2a);
    Serial.println("}");
  }else {
    Serial.println("Invalid command. Use GET, SET, MOVE or HELP.");
  }
}

void handleMoveCommand(String param) {
  if(param.startsWith("r ")) {
    String posName = param.substring(2);
    moveServoToPosition("resServo", posName);
  }
  else if(param.startsWith("d ")) {
    String posName = param.substring(2);
    moveServoToPosition("distServo", posName);
  }
  else {
    Serial.println("Invalid MOVE parameter. Use 'MOVE resServo [position]' or 'MOVE distServo [position]'.");
  }
}

void moveServoToPosition(String servoName, String posName) {
  if(servoName == "resServo") {
    digitalWrite(v1Pin, LOW);
    delay(30);
    if(posName == "res1") resServo.writeMicroseconds(resValues[0]);
    else if(posName == "res2") resServo.writeMicroseconds(resValues[1]);
    else if(posName == "res3") resServo.writeMicroseconds(resValues[2]);
    else if(posName == "res4") resServo.writeMicroseconds(resValues[3]);
    else if(posName == "off") resServo.writeMicroseconds(resValues[4]);
    else Serial.println("Invalid position for resServo. Use 'res1', 'res2', 'res3', 'res4', or 'off'.");
  }
  else if(servoName == "distServo") {
    digitalWrite(v2Pin, LOW);
    delay(30);
    if(posName == "tar1") distServo.writeMicroseconds(distValues[0]);
    else if(posName == "tar2") distServo.writeMicroseconds(distValues[1]);
    else if(posName == "tar3") distServo.writeMicroseconds(distValues[2]);
    else if(posName == "off") distServo.writeMicroseconds(distValues[3]);
    else Serial.println("Invalid position for distServo. Use 'tar1', 'tar2', 'tar3', or 'off'.");
  }
  else {
    Serial.println("Invalid servo name. Use 'resServo' or 'distServo'.");
  }
  delay(5000);
  digitalWrite(v1Pin, HIGH);
  delay(50);
  digitalWrite(v2Pin, HIGH);
  delay(50);
}


void handleGetCommand(String param) {
  if(param == "res1") Serial.println(resValues[0]);
  else if(param == "res2") Serial.println(resValues[1]);
  else if(param == "res3") Serial.println(resValues[2]);
  else if(param == "res4") Serial.println(resValues[3]);
  else if(param == "tar1") Serial.println(distValues[0]);
  else if(param == "tar2") Serial.println(distValues[1]);
  else if(param == "tar3") Serial.println(distValues[2]);
  else if(param == "tar1a") Serial.println(analogRead(A0));
  else if(param == "tar1b") Serial.println(analogRead(A1));
  else if(param == "tar2a") Serial.println(analogRead(A2));
  else if(param == "all tars") {
    Serial.print(analogRead(A0)); Serial.print(",");
    Serial.print(analogRead(A1)); Serial.print(",");
    Serial.println(analogRead(A2));
  }
  else Serial.println("Invalid GET parameter.");
}


void handleSetCommand(String param) {
  int splitIndex = param.indexOf(' ');
  String varName = param.substring(0, splitIndex);
  int val = param.substring(splitIndex+1).toInt();
  
  if(varName == "res1") resValues[0] = val;
  else if(varName == "res2") resValues[1] = val;
  else if(varName == "res3") resValues[2] = val;
  else if(varName == "res4") resValues[3] = val;
  else if(varName == "off") resValues[4] = val;
  else if(varName == "tar1") distValues[0] = val;
  else if(varName == "tar2") distValues[1] = val;
  else if(varName == "tar3") distValues[2] = val;
  else if(varName == "off") distValues[3] = val;
  else if(varName == "on") ;
  else Serial.println("Invalid SET parameter.");
  
  // Update servo positions based on the changed values
  resServo.writeMicroseconds(resValues[4]); // default to 'off' position
  distServo.writeMicroseconds(distValues[3]); // default to 'off' position
}

void handleHelpCommand() {
  Serial.println("HELP - Shows the list of valid commands and what they do.");
  Serial.println("Example: 'HELP'");
  
  Serial.println("GET [param] - Retrieves the current values for any of the named parameters.");
  Serial.println("Examples: 'GET res1', 'GET tar1', 'GET tar1a', 'GET all tars'");
  
  Serial.println("SET [param] [val] - Sets the named locations of the servos.");
  Serial.println("Examples: 'SET res1 550', 'SET tar1 600', 'SET off 1250'");
  
  Serial.println("MOVE [servoName] [position] - Moves the named servo to the specified position.");
  Serial.println("Examples: 'MOVE resServo res1', 'MOVE distServo tar1', 'MOVE resServo off'");
}
