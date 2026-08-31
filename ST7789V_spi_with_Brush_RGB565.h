/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

#ifndef YSS_MOD_TFT_LCD_DRIVER_ST7796_SPI__H_
#define YSS_MOD_TFT_LCD_DRIVER_ST7796_SPI__H_

#include <drv/Gpio.h>
#include <drv/Spi.h>
#include "ST7789V_with_Brush_RGB565.h"

class ST7789V_spi_with_Brush_RGB565 : public ST7789V_with_Brush_RGB565
{
public :
	typedef struct 
	{
		Spi &spi;
		pin_t chipSelect;
		pin_t dataCommand;
		pin_t reset;
	}config_t;

	ST7789V_spi_with_Brush_RGB565(void);

	virtual error_t initialize(config_t config) = 0;

	void setSpiSpecification(const Spi::specification_t &regSpec, const Spi::specification_t &dataSpec);

	void reset(void);

  protected:
	Spi *mSpi;
	pin_t mCsPin;
	pin_t mDcPin;
	pin_t mRstPin;
	const Spi::specification_t *mRegSpec, *mDataSpec;

	void setConfig(const config_t &config);

	void sendReg(cmd_t cmd) override;

	void sendReg(cmd_t cmd, uint8_t data) override;
	
	void sendReg(cmd_t cmd, uint8_t *data, uint32_t count) override;

	void sendData(uint16_t *data, uint32_t count) override;

	void enable(type_t type) override;

	void disable(void) override;

	void read(uint8_t cmd, uint8_t &des);
};

#endif

