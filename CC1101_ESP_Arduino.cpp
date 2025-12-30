/*
 *  CC1101.cpp - CC1101 module library
 *  Copyright (c) 2010 Michael.
 *    Author: Michael, <www.elechouse.com>
 *    Version: November 12, 2010
 * 
 *  This library is designed to use CC1101/CC1100 module on Arduino platform.
 *  CC1101/CC1100 module is an useful wireless module.Using the functions of the 
 *  library, you can easily send and receive data by the CC1101/CC1100 module. 
 *  Just have fun!
 *  For the details, please refer to the datasheet of CC1100/CC1101.
 * --------------------------------------------------------------------------------
 * 
 *  CC1101 ESP32/ESP8266/Arduino Driver. Mod by wladimir-computin.
 * 
 */
#include "CC1101_ESP_Arduino.h"

/****************************************************************/

CC1101::CC1101(int8_t sck, int8_t miso, int8_t mosi, int8_t cs, int8_t gdo0, int8_t gdo2){
	SCK_PIN = sck;
	MISO_PIN = miso;
	MOSI_PIN = mosi;
	SS_PIN = cs;
	GDO0 = gdo0;
	GDO2 = gdo2;
}

void CC1101::spiStart(){
	pinMode(SCK_PIN, OUTPUT);
	pinMode(MOSI_PIN, OUTPUT);
	pinMode(MISO_PIN, INPUT);
	pinMode(SS_PIN, OUTPUT);
	
	if(GDO2 != -1){
		pinMode(GDO0, OUTPUT);
		pinMode(GDO2, INPUT);
	} else {
		pinMode(GDO0, INPUT);
	}

	#if defined ESP32
	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
	#else
	SPI.begin();
	#endif
}

void CC1101::spiEnd(){
	SPI.end();
}

void CC1101::spiStartTransaction(){
	SPI.beginTransaction(spiSettings);
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN)){
	 	yield();
	};
}

void CC1101::spiEndTransaction(){
	//while(digitalRead(MISO_PIN)){
	//	yield();
	//};
	delay(1);
	digitalWrite(SS_PIN, HIGH);
	SPI.endTransaction();
}

void CC1101::spiStrobe(uint8_t strobe){
	spiStartTransaction();
	SPI.transfer(strobe);
	spiEndTransaction();
}

uint8_t CC1101::spiReadReg(uint8_t addr){
	uint8_t temp, value;
	temp = addr| READ_SINGLE;
	spiStartTransaction();
	SPI.transfer(temp);
	value=SPI.transfer(0);
	spiEndTransaction();
	return value;
}

uint8_t CC1101::spiReadStatus(uint8_t addr){
	uint8_t value,temp;
	temp = addr | READ_BURST;
	spiStartTransaction();
	SPI.transfer(temp);
	value=SPI.transfer(0);
	spiEndTransaction();
	return value;
}

void CC1101::spiReadRegBurst(uint8_t addr, uint8_t *buffer, uint8_t buffer_len){
	uint8_t temp;
	temp = addr | READ_BURST;
	spiStartTransaction();
	SPI.transfer(temp);
	for(uint8_t i = 0; i < buffer_len; i++){
		buffer[i]=SPI.transfer(0);
	}
	spiEndTransaction();
}

void CC1101::spiWriteReg(uint8_t addr, uint8_t value){
	spiStartTransaction();
	SPI.transfer(addr);
	SPI.transfer(value);
	spiEndTransaction();
}

void CC1101::spiWriteRegBurst(uint8_t addr, const uint8_t *buffer, uint8_t buffer_len){
	uint8_t temp;
	temp = addr | WRITE_BURST;
	spiStartTransaction();
	SPI.transfer(temp);
	for (uint8_t i = 0; i < buffer_len; i++) {
		SPI.transfer(buffer[i]);
	}
	spiEndTransaction();
}

void CC1101::init(){
	spiStart();
	hardReset();
	regConfigSettings();
	setIdle();
}

void CC1101::regConfigSettings() {
	
	//spiWriteReg(CC1101_FSCTRL1,  0x06);
	//spiWriteReg(CC1101_FSCTRL0,  0x00);
	
	spiWriteReg(CC1101_MDMCFG2,  0b00110000);
	
	//spiWriteReg(CC1101_DEVIATN,  0x15);
	//spiWriteReg(CC1101_FREND1,   0x56);
	spiWriteReg(CC1101_FREND0,   0b00010001);
	spiWriteReg(CC1101_MCSM0 ,   0b00011000);
	spiWriteReg(CC1101_FOCCFG,   0b00010100);
	//spiWriteReg(CC1101_BSCFG,    0x1C);
	//spiWriteReg(CC1101_AGCCTRL2, 0xC7);
	//spiWriteReg(CC1101_AGCCTRL1, 0x00);
	//spiWriteReg(CC1101_AGCCTRL0, 0xB2);
	//spiWriteReg(CC1101_FSCAL3,   0xE9);
	//spiWriteReg(CC1101_FSCAL2,   0x2A);
	//spiWriteReg(CC1101_FSCAL1,   0x00);
	//spiWriteReg(CC1101_FSCAL0,   0x1F);
	//spiWriteReg(CC1101_FSTEST,   0x59);
	//spiWriteReg(CC1101_TEST2,    0x81);
	//spiWriteReg(CC1101_TEST1,    0x35);
	//spiWriteReg(CC1101_TEST0,    0x09);
	
	if(GDO2 != -1){
		spiWriteReg(CC1101_IOCFG2,   0x0D);		//Serial Data Output. Used for asynchronous serial mode.
	} else {
		spiWriteReg(CC1101_IOCFG0,   0x0D);		//Serial Data Output. Used for asynchronous serial mode.
	}
	
	spiWriteReg(CC1101_PKTCTRL0, 0b00110010);

	//spiWriteReg(CC1101_PKTCTRL1, 0x04);

	//spiWriteReg(CC1101_PKTCTRL0, 0x32);
	//spiWriteReg(CC1101_ADDR,     0x00);
	//spiWriteReg(CC1101_PKTLEN,   0x3D);
}

void CC1101::hardReset(){	
	digitalWrite(SS_PIN, LOW);
	delay(1);
	digitalWrite(SS_PIN, HIGH);
	delay(1);
	spiStartTransaction();
	SPI.transfer(CC1101_SRES);
	spiEndTransaction();
	spiStrobe(CC1101_SCAL);
}

void CC1101::softReset(){
	spiStrobe(CC1101_SRES);
}

void CC1101::setMHZ(double target_freq_mhz){
	target_freq_mhz -= 0.015;
	unsigned long target_freq = target_freq_mhz * pow(10,6);
	unsigned long freq_raw = (target_freq * pow(2,16)) / crystal;
	
	uint8_t freq[3];
	freq[0] = (freq_raw & 0x00FF0000) >> 16;
	freq[1] = (freq_raw & 0x0000FF00) >> 8;
	freq[2] =  freq_raw & 0x000000FF;
	
	spiWriteRegBurst(CC1101_FREQ2, freq, sizeof(freq) / sizeof(uint8_t));
}

void CC1101::setTXPwr(TX_DBM value){
	uint8_t patable[] = {0x00, PA_433[value], 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	spiWriteRegBurst(CC1101_PATABLE, patable, 8);
}

void CC1101::setChannelSpacing(int spacing_hz){
	unsigned long spacing = spacing_hz * 1000;
	uint8_t mdmcfg1 = spiReadReg(CC1101_MDMCFG1) & 0b11111100;

	uint8_t chsp_e = log2((spacing * pow(2,10)) / crystal);
	uint8_t chsp_m = ((spacing * pow(2,18)) / (crystal * pow(2,chsp_e))) - 256;
	mdmcfg1 = mdmcfg1 | chsp_e;
		
	spiWriteReg(CC1101_MDMCFG1, mdmcfg1);
	spiWriteReg(CC1101_MDMCFG0, chsp_m);
}

void CC1101::setRxBW(RX_BW_KHZ RxBW){
	if(RxBW < 16){
		uint8_t drate_e = spiReadReg(CC1101_MDMCFG4) & 0b00001111;
		uint8_t rbw = (RxBW << 4) | drate_e;
		spiWriteReg(CC1101_MDMCFG4, rbw);
	}
}

void CC1101::setDataRate(unsigned long baud){
	if (baud >= 600 && baud <= 500000){
		uint8_t rbw = spiReadReg(CC1101_MDMCFG4) & 0b11110000;

		uint8_t drate_e = log2((baud * pow(2,20)) / crystal);
		uint8_t drate_m = ((baud * pow(2,28)) / (crystal * pow(2,drate_e))) - 256;
		uint8_t mdmcfg4 = rbw | (drate_e & 0b00001111);
		
		spiWriteReg(CC1101_MDMCFG4, mdmcfg4);
		spiWriteReg(CC1101_MDMCFG3, drate_m);
	}
}

void CC1101::setModulation(MOD_FORMAT mod){
	uint8_t data_old = spiReadReg(CC1101_MDMCFG2) & 0b10001111;
	uint8_t data_new = ((uint8_t)mod << 4) | data_old;
	spiWriteReg(CC1101_MDMCFG2, data_new);
}

void CC1101::setDeviationHZ(int deviation_hz){
	uint8_t deviation_m = 0;
	uint8_t deviation_e = 0;
	int closest = INT_MAX;
	for(uint8_t test_e = 0; test_e < 8; test_e++){
		for(uint8_t test_m = 0; test_m < 8; test_m++){
			int test_deviation = (crystal/pow(2,17))*(8+test_m)*pow(2, test_e);
			int diff = abs(deviation_hz - test_deviation);
			if (diff < closest){
				closest = diff;
				deviation_m = test_m;
				deviation_e = test_e;
			}
		}
	}
	uint8_t data_new = (deviation_e << 4) | deviation_m;
	Serial.println(data_new);
	spiWriteReg(CC1101_DEVIATN, data_new);
}

void CC1101::setChannel(uint8_t channel){
	spiWriteReg(CC1101_CHANNR, channel);
}

void CC1101::setIdle(){
	spiStrobe(CC1101_SIDLE);
}

void CC1101::setTx(){
	setIdle();
	spiStrobe(CC1101_STX);
	if(GDO2 == -1){
		pinMode(GDO0, OUTPUT);
	}
}

void CC1101::setRx(){;
	setIdle();
	spiStrobe(CC1101_SRX);
	if(GDO2 == -1){
		pinMode(GDO0, INPUT);
	}
}

uint8_t CC1101::receiveData(uint8_t *rxBuffer, uint8_t len){
	uint8_t size;
	uint8_t status[2];
	
	if(spiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO) {
		size=spiReadReg(CC1101_RXFIFO);
		spiReadRegBurst(CC1101_RXFIFO,rxBuffer,len);
		spiReadRegBurst(CC1101_RXFIFO,status,2);
		spiStrobe(CC1101_SFRX);
		return size;
	} else {
		spiStrobe(CC1101_SFRX);
		return 0;
	}
}

uint8_t CC1101::getPartnum(){
	return spiReadStatus(CC1101_PARTNUM);
}

uint8_t CC1101::getVersion(){
	return spiReadStatus(CC1101_VERSION);
}

uint8_t CC1101::log2(double value){
	return log10(value) / log10(2);
}
