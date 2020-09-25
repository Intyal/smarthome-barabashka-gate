#include "Arduino.h"

#include "config.h"

#define FILE_CONFIG "/config.json"

int LED = 22;

void setup() {
	Serial.begin(115200);

	pinMode (LED, OUTPUT);

	Config config(FILE_CONFIG);

	config.printConfig();

	config.loadConfig();

	Serial.println(config.param("wifi"));

}
 
void loop() {
	digitalWrite(LED, HIGH);
	delay(1000);
	digitalWrite(LED, LOW);
	delay(1000);
}
