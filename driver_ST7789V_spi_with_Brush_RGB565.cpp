/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file driver_ST7789V_spi_with_Brush_RGB565.cpp
 * @brief Implementation of the 4-wire SPI bus transport for ST7789V displays.
 * @author Yoon-Ki Hong
 * @date 2015
 */

#include <config.h>

#include "ST7789V_spi_with_Brush_RGB565.h"
#include <yss/debug.h>

#if !defined(YSS_DRV_SPI_UNSUPPORTED) && !defined(YSS_DRV_GPIO_UNSUPPORTED)

/**
 * @brief Default SPI profile for command transmission (Mode 0, 15 MHz, 8-bit).
 */
static const Spi::specification_t gRegSpec =
{
	Spi::CLOCK_MODE_MODE0,	//uint8_t mode;
	15000000,				//uint32_t maxFreq;
	Spi::BIT_BIT8			//uint8_t bit;
};

/**
 * @brief Default SPI profile for 16-bit RGB565 pixel data transmission (Mode 0, 15 MHz, 16-bit).
 */
static const Spi::specification_t gDataSpec =
{
	Spi::CLOCK_MODE_MODE0,	//uint8_t mode;
	15000000,				//uint32_t maxFreq;
	Spi::BIT_BIT16			//uint8_t bit;
};

/**
 * @details Initializes the SPI driver wrapper with default 8-bit command (`gRegSpec`) and 16-bit
 * pixel data (`gDataSpec`) SPI bus specifications running at 15 MHz in SPI Mode 0.
 * @see ST7789V_spi_with_Brush_RGB565::setSpiSpecification()
 * @todo None.
 * @bug None.
 */
ST7789V_spi_with_Brush_RGB565::ST7789V_spi_with_Brush_RGB565(void)
{
	// Assign default 15 MHz Mode 0 SPI profiles for register and pixel transfers
	mRegSpec = &gRegSpec;
	mDataSpec = &gDataSpec;
}

/**
 * @details Caches the hardware SPI peripheral reference and GPIO pin descriptors for Chip Select (`mCsPin`),
 * Data/Command (`mDcPin`), and Reset (`mRstPin`). Drives the CS pin to high (inactive/idle state).
 * @see ST7789V_spi_with_Brush_RGB565::config_t
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::setConfig(const config_t &config)
{
	// Store SPI instance and control pin descriptors
	mSpi = &config.spi;

	mRstPin = config.reset;
	mCsPin = config.chipSelect;
	mDcPin = config.dataCommand;

	// Drive active-low Chip Select pin HIGH (idle state)
	mCsPin.port->setOutput(mCsPin.pin, true);
}

/**
 * @details Assigns user-specified SPI bus specifications for command/register transfers (`regSpec`)
 * and pixel streaming (`dataSpec`).
 * @see ST7789V_spi_with_Brush_RGB565::enable()
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::setSpiSpecification(const Spi::specification_t &regSpec, const Spi::specification_t &dataSpec)
{
	// Update pointers to command and pixel data SPI specifications
	mRegSpec = &regSpec;
	mDataSpec = &dataSpec;
}

/**
 * @details Generates a hardware reset pulse if `mRstPin.port` is valid:
 * 1. Drives RESET low.
 * 2. Delays for 100 ms via `thread::delay()`.
 * 3. Drives RESET high.
 * 4. Delays for 100 ms to allow controller power-on reset stabilization.
 * @see ST7789V::cmd_t::SOFTWARE_RESET
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::reset(void)
{
	if(mRstPin.port)
	{
		// Assert active-low hardware reset for 100 ms, then release and wait 100 ms
		mRstPin.port->setOutput(mRstPin.pin, false);
		thread::delay(100);
		mRstPin.port->setOutput(mRstPin.pin, true);
		thread::delay(100);
	}
}

/**
 * @details Transmits a single command byte over SPI:
 * 1. Drives DC low (command mode).
 * 2. Asserts CS low.
 * 3. Writes `cmd` via `mSpi->send()`.
 * 4. Deasserts CS high.
 * @see ST7789V::sendReg(cmd_t)
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::sendReg(cmd_t cmd)
{
	// Select command mode (DC = LOW), assert CS = LOW, send command byte, and release CS = HIGH
	mDcPin.port->setOutput(mDcPin.pin, false);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(cmd);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

/**
 * @details Transmits a command byte followed by one 8-bit parameter byte:
 * 1. Drives DC low and asserts CS low to transmit `cmd`.
 * 2. Toggles DC high to transmit `data`.
 * 3. Deasserts CS high.
 * @see ST7789V::sendReg(cmd_t, uint8_t)
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::sendReg(cmd_t cmd, uint8_t data)
{
	// Send command index with DC = LOW, then parameter byte with DC = HIGH
	mDcPin.port->setOutput(mDcPin.pin, false);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(cmd);
	mDcPin.port->setOutput(mDcPin.pin, true);
	mSpi->send(data);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

/**
 * @details Transmits a command byte followed by an array of 8-bit parameter bytes:
 * 1. Drives DC low and asserts CS low to transmit `cmd`.
 * 2. Toggles DC high and transmits the buffer payload via `mSpi->send(data, count)`.
 * 3. Deasserts CS high.
 * @see ST7789V::sendReg(cmd_t, uint8_t*, uint32_t)
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::sendReg(cmd_t cmd, uint8_t *data, uint32_t count)
{
	// Send command index with DC = LOW, then stream parameter buffer with DC = HIGH
	mDcPin.port->setOutput(mDcPin.pin, false);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(cmd);
	mDcPin.port->setOutput(mDcPin.pin, true);
	mSpi->send((int8_t *)data, count);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

/**
 * @details Streams 16-bit pixel words over the SPI bus:
 * 1. Drives DC high (data mode).
 * 2. Asserts CS low.
 * 3. Transmits `count` 16-bit words via `mSpi->send(data, count)`.
 * 4. Deasserts CS high.
 * @see ST7789V::sendData(uint16_t*, uint32_t)
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::sendData(uint16_t *data, uint32_t count)
{
	// Stream 16-bit RGB565 pixel array in data mode (DC = HIGH)
	mDcPin.port->setOutput(mDcPin.pin, true);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(data, count);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

/**
 * @details Acquires the mutex lock on the SPI peripheral (`mSpi->lock()`) and configures the SPI
 * bus with either `mDataSpec` (16-bit mode for pixel data when `type == TYPE_DATA`) or `mRegSpec`
 * (8-bit mode for commands/parameters when `type == TYPE_REG`).
 * @see ST7789V_spi_with_Brush_RGB565::disable()
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::enable(type_t type)
{
	// Acquire SPI bus mutex and apply either 16-bit pixel or 8-bit register transfer profile
	mSpi->lock();
	if(type == TYPE_DATA)
		mSpi->setSpecification(*mDataSpec);
	else
		mSpi->setSpecification(*mRegSpec);
}

/**
 * @details Releases the mutex lock on the SPI peripheral (`mSpi->unlock()`).
 * @see ST7789V_spi_with_Brush_RGB565::enable()
 * @todo None.
 * @bug None.
 */
void ST7789V_spi_with_Brush_RGB565::disable(void)
{
	// Release SPI bus mutex
	mSpi->unlock();
}

#endif


