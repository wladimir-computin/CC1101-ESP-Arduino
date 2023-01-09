//https://github.com/wladimir-computin/CC1101-ESP-Arduino
#include <CC1101_ESP_Arduino.h>

//https://github.com/wladimir-computin/espBitBanger
#include <EspBitBanger.h>

//ESP32 PINs
const int SPI_SCK = 18; 			//board or mcu specific
const int SPI_MISO = 19; 			//board or mcu specific
const int SPI_MOSI = 23; 			//board or mcu specific
const int SPI_CS = 5; 				//select any pin
const int RADIO_INPUT_PIN = 12; 	//select any pin, this is the TX-PIN
const int RADIO_OUTPUT_PIN = 13; 	//select any pin, this is the RX-PIN

EspBitBanger bitBanger;
CC1101 cc1101(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS, RADIO_INPUT_PIN, RADIO_OUTPUT_PIN);

uint8_t buffer[] = {0b11101110, 0b10001110, 0b10001000, 0b10001110, 0b11101110, 0b10001000, 0b10001000, 0b10001000, 0b10001000, 0b11101110, 0b10001110, 0b10001110, 0b10000000};

void setup(){
	Serial.begin(115200);
	bitBanger.begin(2666, -1, RADIO_INPUT_PIN);
	cc1101.init();
	printfDebug("CC1101: 0x%02x, version: 0x%02x\n", cc1101.getPartnum(), cc1101.getVersion());
	cc1101.setMHZ(433.92);
	cc1101.setTXPwr(TX_PLUS_10_DBM);
	cc1101.setDataRate(10000);
	cc1101.setModulation(ASK_OOK);
	
	cc1101.setTx();
	for(int i = 0; i < 5; i++){
		delay(10);
		bitBanger.write(buffer, sizeof(buffer) / sizeof(buffer[0]));
	}
	cc1101.setIdle();
}

void loop(){
	
}
