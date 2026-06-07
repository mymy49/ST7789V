/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

#include <config.h>

#include "ST7789V_spi_with_Brush_RGB565.h"
#include <yss/debug.h>

#if !defined(YSS_DRV_SPI_UNSUPPORTED) && !defined(YSS_DRV_GPIO_UNSUPPORTED)

static const Spi::specification_t gRegSpec =
{
	Spi::CLOCK_MODE_MODE0,	//uint8_t mode;
	15000000,			//uint32_t maxFreq;
	Spi::BIT_BIT8		//uint8_t bit;
};

static const Spi::specification_t gDataSpec =
{
	Spi::CLOCK_MODE_MODE0,	//uint8_t mode;
	15000000,			//uint32_t maxFreq;
	Spi::BIT_BIT16		//uint8_t bit;
};

ST7789V_spi_with_Brush_RGB565::ST7789V_spi_with_Brush_RGB565(void)
{
	mRegSpec = &gRegSpec;
	mDataSpec = &gDataSpec;
}

void ST7789V_spi_with_Brush_RGB565::setConfig(const config_t &config)
{
	mSpi = &config.spi;

	mRstPin = config.reset;
	mCsPin = config.chipSelect;
	mDcPin = config.dataCommand;

	mCsPin.port->setOutput(mCsPin.pin, true);
}

void ST7789V_spi_with_Brush_RGB565::setSpiSpecification(const Spi::specification_t &regSpec, const Spi::specification_t &dataSpec)
{
	mRegSpec = &regSpec;
	mDataSpec = &dataSpec;
}

void ST7789V_spi_with_Brush_RGB565::reset(void)
{
	if(mRstPin.port)
	{
		mRstPin.port->setOutput(mRstPin.pin, false);
		thread::delay(100);
		mRstPin.port->setOutput(mRstPin.pin, true);
		thread::delay(100);
	}
}

void ST7789V_spi_with_Brush_RGB565::sendReg(cmd_t cmd)
{
	mDcPin.port->setOutput(mDcPin.pin, false);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(cmd);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

void ST7789V_spi_with_Brush_RGB565::sendReg(cmd_t cmd, uint8_t data)
{
	mDcPin.port->setOutput(mDcPin.pin, false);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(cmd);
	mDcPin.port->setOutput(mDcPin.pin, true);
	mSpi->send(data);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

void ST7789V_spi_with_Brush_RGB565::sendReg(cmd_t cmd, uint8_t *data, uint32_t count)
{
	mDcPin.port->setOutput(mDcPin.pin, false);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(cmd);
	mDcPin.port->setOutput(mDcPin.pin, true);
	mSpi->send((int8_t *)data, count);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

void ST7789V_spi_with_Brush_RGB565::sendData(uint16_t *data, uint32_t count)
{
	mDcPin.port->setOutput(mDcPin.pin, true);
	mCsPin.port->setOutput(mCsPin.pin, false);
	mSpi->send(data, count);
	mCsPin.port->setOutput(mCsPin.pin, true);
}

void ST7789V_spi_with_Brush_RGB565::enable(type_t type)
{
	mSpi->lock();
	if(type == TYPE_DATA)
		mSpi->setSpecification(*mDataSpec);
	else
		mSpi->setSpecification(*mRegSpec);
}

void ST7789V_spi_with_Brush_RGB565::disable(void)
{
	mSpi->unlock();
}

#endif

