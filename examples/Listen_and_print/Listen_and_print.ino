#include <CC1101_ESP_Arduino.h>

//ESP32 PINs
const int SPI_SCK = 18; 			//board or mcu specific
const int SPI_MISO = 19; 			//board or mcu specific
const int SPI_MOSI = 23; 			//board or mcu specific
const int SPI_CS = 5; 				//select any pin
const int RADIO_INPUT_PIN = 13; 	//select any pin
const int RADIO_OUTPUT_PIN = 12; 	//select any pin

CC1101 cc1101(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS, RADIO_OUTPUT_PIN, RADIO_INPUT_PIN);

volatile long last_micros;
String buffer;

void radioHandlerOnChange() {
	int delta_micros = micros() - last_micros;
	
	bool input = digitalRead(RADIO_INPUT_PIN);
	if(input == 1){
		buffer += "\n0 -> 1 after " + String(delta_micros);
	} else {
		buffer += "\n1 -> 0 after " + String(delta_micros);
	}
	
	last_micros = micros();
}

void setup(){
	Serial.begin(115200);
	
	cc1101.init();
	cc1101.setMHZ(433.9);
	cc1101.setDataRate(5000);

	cc1101.setRx();

	attachInterrupt(RADIO_INPUT_PIN, radioHandlerOnChange, CHANGE);
}

volatile long last_millis;

// Print received data every 5 seconds, set CC1101 into IDLE mode while printing
void loop(){
	if (millis() > (last_millis + 5000)){
		cc1101.setIdle();
		Serial.println(buffer);
		buffer = "";
		cc1101.setRx();
		
		last_millis = millis();
	}
}
