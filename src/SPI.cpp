/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

/**********************************************************/
/*     32 bit Teensy 4.x                                  */
/**********************************************************/

void SPIClass::begin()
{
	// We should initialize the SPI to be in a known default state.
	beginTransaction(SPISettings());
	endTransaction();
}

void SPIClass::setClockDivider_noInline(uint32_t clk) {
	_clock = clk;
}


uint8_t SPIClass::pinIsChipSelect(uint8_t pin)
{
	return 0;
}

bool SPIClass::pinIsChipSelect(uint8_t pin1, uint8_t pin2)
{
	return false;
}


bool SPIClass::pinIsMOSI(uint8_t pin)
{
	return false;
}

bool SPIClass::pinIsMISO(uint8_t pin)
{
	return false;
}

bool SPIClass::pinIsSCK(uint8_t pin)
{
	return false;
}

// setCS() is not intended for use from normal Arduino programs/sketches.
uint8_t SPIClass::setCS(uint8_t pin)
{
	return 0;
}

void SPIClass::setMOSI(uint8_t pin)
{
    mosi_pin_index = pin;
}

void SPIClass::setMISO(uint8_t pin)
{
    miso_pin_index = pin;
}

void SPIClass::setSCK(uint8_t pin)
{
    sck_pin_index = pin;
}


void SPIClass::setBitOrder(uint8_t bitOrder)
{
}

void SPIClass::setDataMode(uint8_t dataMode)
{


}

void _spi_dma_rxISR0(void) {}

// NOTE pin definitions are in the order MISO, MOSI, SCK, CS
// With each group, having pin number[n], setting[n], INPUT_SELECT_MUX settings[n], SELECT INPUT register
#if defined(ARDUINO_TEENSY41)
const SPIClass::SPI_Hardware_t  SPIClass::spiclass_lpspi4_hardware = {
	CCM_CCGR1, CCM_CCGR1_LPSPI4(CCM_CCGR_ON),
	DMAMUX_SOURCE_LPSPI4_TX, DMAMUX_SOURCE_LPSPI4_RX, _spi_dma_rxISR0,
	12, 255,  // MISO
	3 | 0x10, 0,
	0, 0,
	IOMUXC_LPSPI4_SDI_SELECT_INPUT,
	11, 255, // MOSI
	3 | 0x10, 0,
	0, 0,
	IOMUXC_LPSPI4_SDO_SELECT_INPUT,
	13, 255, // SCK
	3 | 0x10, 0,
	0, 0,
	IOMUXC_LPSPI4_SCK_SELECT_INPUT,
	10, 37, 36, // CS
	3 | 0x10, 2 | 0x10, 2 | 0x10,
	1, 2, 3,
	0, 0, 0,
	&IOMUXC_LPSPI4_PCS0_SELECT_INPUT, 0, 0
};
#else
const SPIClass::SPI_Hardware_t  SPIClass::spiclass_lpspi4_hardware = {
	CCM_CCGR1, CCM_CCGR1_LPSPI4(CCM_CCGR_ON),
	DMAMUX_SOURCE_LPSPI4_TX, DMAMUX_SOURCE_LPSPI4_RX, _spi_dma_rxISR0,
	12,
	3 | 0x10,
	0,
	IOMUXC_LPSPI4_SDI_SELECT_INPUT,
	11,
	3 | 0x10,
	0,
	IOMUXC_LPSPI4_SDO_SELECT_INPUT,
	13,
	3 | 0x10,
	0,
	IOMUXC_LPSPI4_SCK_SELECT_INPUT,
	10,
	3 | 0x10,
	1,
	0,
	&IOMUXC_LPSPI4_PCS0_SELECT_INPUT
};
#endif

//using &IMXRT_LPSPI4_S somehow interferes with constexpr static init, but why?
//details: https://forum.pjrc.com/threads/73154
//SPIClass SPI((uintptr_t)&IMXRT_LPSPI4_S, (uintptr_t)&SPIClass::spiclass_lpspi4_hardware);
SPIClass SPI(IMXRT_LPSPI4_ADDRESS, (uintptr_t)&SPIClass::spiclass_lpspi4_hardware);

// T4 has two other possible SPI objects...
void _spi_dma_rxISR1(void) { }

const SPIClass::SPI_Hardware_t  SPIClass::spiclass_lpspi3_hardware = {
	CCM_CCGR1, CCM_CCGR1_LPSPI3(CCM_CCGR_ON),
	DMAMUX_SOURCE_LPSPI3_TX, DMAMUX_SOURCE_LPSPI3_RX, _spi_dma_rxISR1,
	1, 39,
	7 | 0x10, 2 | 0x10,
	0, 1,
	IOMUXC_LPSPI3_SDI_SELECT_INPUT,
	26, 255,
	2 | 0x10, 0,
	1, 0,
	IOMUXC_LPSPI3_SDO_SELECT_INPUT,
	27, 255,
	2 | 0x10, 0,
	1,  0,
	IOMUXC_LPSPI3_SCK_SELECT_INPUT,
	0, 38, 255,
	7 | 0x10, 2 | 0x10, 0,
	1, 1, 0,
	0, 1, 0,
	&IOMUXC_LPSPI3_PCS0_SELECT_INPUT, &IOMUXC_LPSPI3_PCS0_SELECT_INPUT, 0
};
//SPIClass SPI1((uintptr_t)&IMXRT_LPSPI3_S, (uintptr_t)&SPIClass::spiclass_lpspi3_hardware);
SPIClass SPI1(IMXRT_LPSPI3_ADDRESS, (uintptr_t)&SPIClass::spiclass_lpspi3_hardware);

void _spi_dma_rxISR2(void) {}

const SPIClass::SPI_Hardware_t  SPIClass::spiclass_lpspi1_hardware = {
	CCM_CCGR1, CCM_CCGR1_LPSPI1(CCM_CCGR_ON),
	DMAMUX_SOURCE_LPSPI1_TX, DMAMUX_SOURCE_LPSPI1_RX, _spi_dma_rxISR1,
	42, 54,
	4 | 0x10, 3 | 0x10,
	1, 0,
	IOMUXC_LPSPI1_SDI_SELECT_INPUT,
	43, 50,
	4 | 0x10, 3 | 0x10,
	1, 0,
	IOMUXC_LPSPI1_SDO_SELECT_INPUT,
	45, 49,
	4 | 0x10, 3 | 0x10,
	1, 0,
	IOMUXC_LPSPI1_SCK_SELECT_INPUT,
	44, 255, 255,
	4 | 0x10, 0, 0,
	1, 0, 0,
	0, 0, 0,
	&IOMUXC_LPSPI1_PCS0_SELECT_INPUT, 0, 0
};

//SPIClass SPI2((uintptr_t)&IMXRT_LPSPI1_S, (uintptr_t)&SPIClass::spiclass_lpspi1_hardware);
SPIClass SPI2(IMXRT_LPSPI1_ADDRESS, (uintptr_t)&SPIClass::spiclass_lpspi1_hardware);

//SPIClass SPI(&IMXRT_LPSPI4_S, &spiclass_lpspi4_hardware);

void SPIClass::usingInterrupt(IRQ_NUMBER_t interruptName)
{

}

void SPIClass::notUsingInterrupt(IRQ_NUMBER_t interruptName)
{

}

void SPIClass::transfer(const void * buf, void * retbuf, size_t count)
{
}

void SPIClass::transfer16(const void * buf, void * retbuf, size_t count)
{

}

void SPIClass::transfer32(const void * buf, void * retbuf, size_t count)
{

}


void SPIClass::end() {

}

//=============================================================================
// ASYNCH Support
//=============================================================================
//=========================================================================
// Try Transfer using DMA.
//=========================================================================
#ifdef SPI_HAS_TRANSFER_ASYNC
static uint8_t bit_bucket;
#define dontInterruptAtCompletion(dmac) (dmac)->TCD->CSR &= ~DMA_TCD_CSR_INTMAJOR

//=========================================================================
// Init the DMA channels
//=========================================================================
bool SPIClass::initDMAChannels() {
	return true;
}

//=========================================================================
// Main Async Transfer function
//=========================================================================
#ifndef TRANSFER_COUNT_FIXED
inline void DMAChanneltransferCount(DMAChannel * dmac, unsigned int len) {
}
#else
inline void DMAChanneltransferCount(DMAChannel * dmac, unsigned int len) {
}
#endif
#ifdef DEBUG_DMA_TRANSFERS
void dumpDMA_TCD(DMABaseClass *dmabc)
{
}
#endif

bool SPIClass::transfer(const void *buf, void *retbuf, size_t count, EventResponderRef event_responder) {
	return true;
}


//-------------------------------------------------------------------------
// DMA RX ISR
//-------------------------------------------------------------------------
void SPIClass::dma_rxisr(void) {
}
#endif // SPI_HAS_TRANSFER_ASYNC