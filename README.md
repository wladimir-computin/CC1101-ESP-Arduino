# CC1101-ESP-Arduino
ESP8266 / ESP32 / Arduino driver lib for using the great TI C1101 transceiver

## About

The Texas Instruments CC1101 is a very versatile, high quality and cheap transceiver, supporting a wide range of frequencies and modulations.
Ready-to-use CC1101 modules are as cheap as 5.00€ and at the same time significantly better than simple 433MHz receiver and transmitter modules.
The confusing part about these CC1101s is, they are actually kind of overpowered, providing logic for sync word detection, CRC checks, packet formats
and so on. However, it's still possible to use them as simple digital transceivers, giving them a minimum of configuration and letting the MCU (like our ESP8266/ESP32/Arduino) do all the work.
That's especially useful, if you want to transmit or decode non-standard packet formats. Like the vast majority of us, I suppose ;)

This library aims to help developers using the chip without having to read the whole 105 pages [datasheet](http://www.ti.com/lit/ds/symlink/cc1101.pdf) first. 

## Features
* High level access to important features, like switching frequencies, switching between IDLE/TX/RX, sending / receiving data etc.
* Low level access, so you can simply read/write directly all CC1101 registers without having to deal with SPI specific stuff.
* Written by someone, who actually read the datasheet ;)

## CC1101 technical info
### RX/TX modes
* Asynchronous serial mode
* Synchronous serial mode
* FIFO

## Limitations
I develop this library in my free time. Some features are not done yet.
For now, only OOK modulation and only asynchronous serial mode are supported.

### Progress
* Implemented and test low level SPI communication
* Switch between RX, TX and IDLE mode
* Read and write data
* Set frequency at runtime
* Set baud rate at runtime
* Set TX Power at runtime
* Set RX bandwith at runtime
* Set Channel and channel spacing (optional) at runtime
* Perform reset of module

### TODO
* SLEEP mode
* 2-FSK, 4-FSK, GFSK, MSK and ASK support
* synchronous serial mode
* FIFO mode
* Packet handling features

## Examples
Look into [/examples](https://github.com/wladimir-computin/CC1101-ESP-Arduino/blob/master/examples/) :)

## Wiring
Connect SPI_SCK, SPI_MISO, SPI_MOSI to the corresponding SPI Pins on your board.
SPI_CS can be any pin usable as INPUT and OUTPUT, but most boards have a default one.

The CC1101 has two multi-purpose pins, GDO0 and GDO2.
(Actually three, but GDO1 is SPI_MISO, which is only usable if SPI_CS is HIGH.)

| PIN  | FEATURES                            |
|------|-------------------------------------|
| GDO0 | INPUT/OUTPUT                        |
| GDO1 | OUTPUT (only usable if SPI_CS=HIGH) |
| GDO2 | OUTPUT                              |

* You can connect zero additional pins (GDO0 and GDO2 not connected), but then you MUST use the CC1101 RX/TX FIFOs.
In other words, you can't use the more flexible serial mode.

* You can connect only GDO0, if you are using asynchronous serial mode. The pin will switch automatically from INPUT to OUTPUT when you call setTX() and vice versa.

* You can connect GDO0 and GDO2, then you have the choice between asynchronous serial mode (GDO0 = TX, GDO2 = RX) and synchronous serial mode (GDO = TX/RX, GD02 = clock).

* You can use GDO1, but this isn't fully supported by the high level methods of this library. However, you may read the datasheet and use the low level SPI methods for configuration instead.

* You can also use the GDOx pins for something else. Again, that's what the low level SPI methods are for.

## API

### Constructors:
```cpp
CC1101(int gdo0);
CC1101(int gdo0, int gdo2);
CC1101(int sck, int miso, int mosi, int cs, int gdo0);
CC1101(int sck, int miso, int mosi, int cs, int gdo0, int gdo2);
```

### High level methods:
```cpp
void init();
void softReset();
void hardReset ();
void setIdle();
void setRx();
void setTx();
void setMHZ(double mhz);
void setChannel(uint8_t channel);
void setChannelSpacing(int spacing_hz);
void setTXPwr(TX_DBM value);
void setRxBW(RX_BW_KHZ RxBW);
void setDataRate(unsigned long baud);
```

### Low level methods:
```cpp
void spiStrobe(uint8_t strobe);
uint8_t spiReadReg(uint8_t addr);
uint8_t spiReadStatus(uint8_t addr);
void spiReadRegBurst(uint8_t addr, uint8_t* buffer, uint8_t buffer_len);
void spiWriteReg(uint8_t addr, uint8_t value);
void spiWriteRegBurst(uint8_t addr, const uint8_t* buffer, uint8_t buffer_len);
```
### Definitions (see CC1101_ESP_Arduino.h for more)
```cpp
enum TX_DBM {
	TX_DEFAULT_DBM = 0,
	TX_MINUS_30_DBM = 1,
	TX_MINUS_20_DBM = 2,
	TX_MINUS_15_DBM = 3,
	TX_MINUS_10_DBM = 4,
	TX_0_DBM = 5,
	TX_PLUS_5_DBM = 6,
	TX_PLUS_7_DBM = 7,
	TX_PLUS_10_DBM = 8
};

enum RX_BW_KHZ {
	RX_BW_812_KHZ = 0,
	RX_BW_650_KHZ = 1,
	RX_BW_541_KHZ = 2,
	RX_BW_464_KHZ = 3,
	RX_BW_406_KHZ = 4,
	RX_BW_325_KHZ = 5,
	RX_BW_270_KHZ = 6,
	RX_BW_232_KHZ = 7,
	RX_BW_203_KHZ = 8,
	RX_BW_162_KHZ = 9,
	RX_BW_135_KHZ = 10,
	RX_BW_116_KHZ = 11,
	RX_BW_102_KHZ = 12,
	RX_BW_81_KHZ = 13,
	RX_BW_68_KHZ = 14,
	RX_BW_58_KHZ = 15
};
```

## Contribute
Help is always welcome :)
