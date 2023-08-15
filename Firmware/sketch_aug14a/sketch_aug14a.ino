#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // Wait for serial connection
  }
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
    }
    // Add more command checks and responses here
  }
}
