/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>
#include <imxrt.h>

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// Uncomment this line to add detection of mismatched begin/end transactions.
// A mismatch occurs if other libraries fail to use SPI.endTransaction() for
// each SPI.beginTransaction().  Connect a LED to this pin.  The LED will turn
// on if any mismatch is ever detected.
//#define SPI_TRANSACTION_MISMATCH_LED 5

// SPI_HAS_TRANSFER_BUF - is defined to signify that this library supports
// a version of transfer which allows you to pass in both TX and RX buffer
// pointers, either of which could be NULL
#define SPI_HAS_TRANSFER_BUF 1


#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

/**********************************************************/
/*     32 bit Teensy 4.x                                  */
/**********************************************************/

#define SPI_HAS_NOTUSINGINTERRUPT 1
#define SPI_ATOMIC_VERSION 1

//#include "debug/printf.h"

class SPISettings {
public:
	SPISettings(uint32_t clockIn, uint8_t bitOrderIn, uint8_t dataModeIn) : _clock(clockIn) {
	}

	SPISettings() : _clock(4000000) {
	}
private:


	inline uint32_t clock() {return _clock;}

	uint32_t _clock;
	uint32_t tcr; // transmit command, pg 2664 (RT1050 ref, rev 2)
	friend class SPIClass;
};

class SPIClass { // Teensy 4
public:
	#if defined(ARDUINO_TEENSY41)
	// T4.1 has SPI2 pins on memory connectors as well as SDCard
	static const uint8_t CNT_MISO_PINS = 2;
	static const uint8_t CNT_MOSI_PINS = 2;
	static const uint8_t CNT_SCK_PINS = 2;
	static const uint8_t CNT_CS_PINS = 3;
	#else
	static const uint8_t CNT_MISO_PINS = 1;
	static const uint8_t CNT_MOSI_PINS = 1;
	static const uint8_t CNT_SCK_PINS = 1;
	static const uint8_t CNT_CS_PINS = 1;
#endif
	typedef struct {
		volatile uint32_t &clock_gate_register;
		const uint32_t clock_gate_mask;
		uint8_t  tx_dma_channel;
		uint8_t  rx_dma_channel;
		void     (*dma_rxisr)();
		// MISO pins
		const uint8_t  		miso_pin[CNT_MISO_PINS];
		const uint32_t  	miso_mux[CNT_MISO_PINS];
		const uint8_t 		miso_select_val[CNT_MISO_PINS];
		volatile uint32_t 	&miso_select_input_register;

		// MOSI pins
		const uint8_t  		mosi_pin[CNT_MOSI_PINS];
		const uint32_t  	mosi_mux[CNT_MOSI_PINS];
		const uint8_t 		mosi_select_val[CNT_MOSI_PINS];
		volatile uint32_t 	&mosi_select_input_register;

		// SCK pins
		const uint8_t  		sck_pin[CNT_SCK_PINS];
		const uint32_t  	sck_mux[CNT_SCK_PINS];
		const uint8_t 		sck_select_val[CNT_SCK_PINS];
		volatile uint32_t 	&sck_select_input_register;

		// CS Pins
		const uint8_t  		cs_pin[CNT_CS_PINS];
		const uint32_t  	cs_mux[CNT_CS_PINS];
		const uint8_t  		cs_mask[CNT_CS_PINS];
		const uint8_t 		pcs_select_val[CNT_CS_PINS];
		volatile uint32_t 	*pcs_select_input_register[CNT_CS_PINS];

	} SPI_Hardware_t;
	static const SPI_Hardware_t spiclass_lpspi4_hardware;
	static const SPI_Hardware_t spiclass_lpspi3_hardware;
	static const SPI_Hardware_t spiclass_lpspi1_hardware;
public:
	constexpr SPIClass(uintptr_t myport, uintptr_t myhardware)
		: port_addr(myport), hardware_addr(myhardware) {
	}
//	constexpr SPIClass(IMXRT_LPSPI_t *myport, const SPI_Hardware_t *myhardware)
//		: port(myport), hardware(myhardware) {
//	}
	friend uintptr_t Teensyduino_Test_constinit_SPI(int instance, int index);
	// Initialize the SPI library
	void begin();

	// If SPI is to used from within an interrupt, this function registers
	// that interrupt with the SPI library, so beginTransaction() can
	// prevent conflicts.  The input interruptNumber is the number used
	// with attachInterrupt.  If SPI is used from a different interrupt
	// (eg, a timer), interruptNumber should be 255.
	void usingInterrupt(uint8_t n) {}

	void usingInterrupt(IRQ_NUMBER_t interruptName);
	void notUsingInterrupt(IRQ_NUMBER_t interruptName);

	// Before using SPI.transfer() or asserting chip select pins,
	// this function is used to gain exclusive access to the SPI bus
	// and configure the correct settings.
	void beginTransaction(SPISettings settings) {

	}

	// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
	uint8_t transfer(uint8_t data) {
	}
	uint16_t transfer16(uint16_t data) {

	}
    uint32_t transfer32(uint32_t data) {
    }

	void inline transfer(void *buf, size_t count) {
#if 0
		// TODO: byte order still needs work to match SPISettings
		if (__builtin_constant_p(count)) {
			if (count < 1) return;
			if (((count & 3) == 0) && (((uint32_t)buf & 3) == 0)) {
				// size is multiple of 4 and buffer is 32 bit aligned
				transfer32(buf, buf, count >> 2);
				return;
			}
			if (((count & 1) == 0) && (((uint32_t)buf & 1) == 0)) {
				// size is multiple of 2 and buffer is 16 bit aligned
				transfer16(buf, buf, count >> 1);
				return;
			}
		}
#endif
		transfer(buf, buf, count);
	}
	void setTransferWriteFill(uint8_t ch ) {_transferWriteFill = ch;}
	void transfer(const void * buf, void * retbuf, size_t count);

	// Asynch support (DMA )
#ifdef SPI_HAS_TRANSFER_ASYNC
	bool transfer(const void *txBuffer, void *rxBuffer, size_t count,  EventResponderRef  event_responder);

	friend void _spi_dma_rxISR0(void);
	inline void dma_rxisr(void);
#endif


	// After performing a group of transfers and releasing the chip select
	// signal, this function allows others to access the SPI bus
	void endTransaction(void) {
	}

	// Disable the SPI bus
	void end();

	// This function is deprecated.	 New applications should use
	// beginTransaction() to configure SPI settings.
	void setBitOrder(uint8_t bitOrder);

	// This function is deprecated.	 New applications should use
	// beginTransaction() to configure SPI settings.
	void setDataMode(uint8_t dataMode);

	// This function is deprecated.	 New applications should use
	// beginTransaction() to configure SPI settings.
	void setClockDivider(uint8_t clockDiv) {
		if (clockDiv == SPI_CLOCK_DIV2) {
			setClockDivider_noInline(12000000);
		} else if (clockDiv == SPI_CLOCK_DIV4) {
			setClockDivider_noInline(4000000);
		} else if (clockDiv == SPI_CLOCK_DIV8) {
			setClockDivider_noInline(2000000);
		} else if (clockDiv == SPI_CLOCK_DIV16) {
			setClockDivider_noInline(1000000);
		} else if (clockDiv == SPI_CLOCK_DIV32) {
			setClockDivider_noInline(500000);
		} else if (clockDiv == SPI_CLOCK_DIV64) {
			setClockDivider_noInline(250000);
		} else { /* clockDiv == SPI_CLOCK_DIV128 */
			setClockDivider_noInline(125000);
		}
	}
	void setClockDivider_noInline(uint32_t clk);

	// These undocumented functions should not be used.  SPI.transfer()
	// polls the hardware flag which is automatically cleared as the
	// AVR responds to SPI's interrupt
	void attachInterrupt() { }
	void detachInterrupt() { }

	// Teensy 3.x can use alternate pins for these 3 SPI signals.
	void setMOSI(uint8_t pin);
	void setMISO(uint8_t pin);
	void setSCK(uint8_t pin);

	// return true if "pin" has special chip select capability
	uint8_t pinIsChipSelect(uint8_t pin);
	bool pinIsMOSI(uint8_t pin);
	bool pinIsMISO(uint8_t pin);
	bool pinIsSCK(uint8_t pin);
	// return true if both pin1 and pin2 have independent chip select capability
	bool pinIsChipSelect(uint8_t pin1, uint8_t pin2);
	// configure a pin for chip select and return its SPI_MCR_PCSIS bitmask
	// setCS() is a special function, not intended for use from normal Arduino
	// programs/sketches.  See the ILI3941_t3 library for an example.
	uint8_t setCS(uint8_t pin);

private:
private:
	const SPI_Hardware_t & hardware() { return *(const SPI_Hardware_t *)hardware_addr; }
	uintptr_t port_addr;
	uintptr_t hardware_addr;

	uint32_t _clock = 0;
	uint32_t _ccr = 0;

	uint8_t miso_pin_index = 0;
	uint8_t mosi_pin_index = 0;
	uint8_t sck_pin_index = 0;
	uint8_t interruptMasksUsed = 0;
	uint32_t interruptMask[(NVIC_NUM_INTERRUPTS+31)/32] = {};
	uint32_t interruptSave[(NVIC_NUM_INTERRUPTS+31)/32] = {};
	#ifdef SPI_TRANSACTION_MISMATCH_LED
	uint8_t inTransactionFlag = 0;
	#endif

	uint8_t _transferWriteFill = 0;

	// DMA Support
#ifdef SPI_HAS_TRANSFER_ASYNC
	bool initDMAChannels();
	enum DMAState { notAllocated, idle, active, completed};
	enum {MAX_DMA_COUNT=32767};
	DMAState     _dma_state = DMAState::notAllocated;
	uint32_t	_dma_count_remaining = 0;	// How many bytes left to output after current DMA completes
	DMAChannel   *_dmaTX = nullptr;
	DMAChannel    *_dmaRX = nullptr;
	EventResponder *_dma_event_responder = nullptr;
#endif

	// Optimized buffer transfer
	void inline transfer16(void *buf, size_t count) {transfer16(buf, buf, count);}
	void inline transfer32(void *buf, size_t count) {transfer32(buf, buf, count);}
	void transfer16(const void * buf, void * retbuf, size_t count);
	void transfer32(const void * buf, void * retbuf, size_t count);
};




extern SPIClass SPI;
extern SPIClass SPI1;
extern SPIClass SPI2;
#endif