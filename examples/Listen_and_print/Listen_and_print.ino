#include <CC1101_ESP_Arduino.h>

//ESP32 PINs
const int SPI_SCK = 18; 			//board or mcu specific
const int SPI_MISO = 19; 			//board or mcu specific
const int SPI_MOSI = 23; 			//board or mcu specific
const int SPI_CS = 5; 				//select any pin
const int RADIO_INPUT_PIN = 12; 	//select any pin, this is the TX-PIN
const int RADIO_OUTPUT_PIN = 13; 	//select any pin, this is the RX-PIN

CC1101 cc1101(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS, RADIO_INPUT_PIN, RADIO_OUTPUT_PIN);

volatile long last_micros;
String buffer;

ICACHE_RAM_ATTR void radioHandlerOnChange() {
	int delta_micros = micros() - last_micros;
	
	bool input = digitalRead(RADIO_OUTPUT_PIN);
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
	Serial.printf("CC1101: 0x%02x, version: 0x%02x\n", cc1101.getPartnum(), cc1101.getVersion());
	cc1101.setMHZ(433.92);
	cc1101.setTXPwr(TX_PLUS_10_DBM);
	cc1101.setDataRate(5000);
	cc1101.setModulation(ASK_OOK);

	cc1101.setRx();

	attachInterrupt(RADIO_OUTPUT_PIN, radioHandlerOnChange, CHANGE);
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
