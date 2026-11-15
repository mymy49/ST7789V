/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file driver_ST7789V.cpp
 * @brief Implementation of the base ST7789V controller driver methods.
 * @author Yoon-Ki Hong
 * @date 2015
 */

#include "ST7789V.h"
#include <yss/Area.h>

/**
 * @details Initializes the base `ST7789V` instance with `mRotateFlag` set to `false` (portrait mode).
 * @see ST7789V::setDirection()
 * @todo None.
 * @bug None.
 */
ST7789V::ST7789V(void)
{
	mRotateFlag = false;
}

/**
 * @details Builds the 8-bit `MADCTL` (Memory Data Access Control) register payload based on mirroring
 * and rotation flags:
 * - When `rotate == true` (landscape mode):
 *   - Bit 5 (MV, Row/Column Exchange) is asserted (`0x20`).
 *   - If `xMirror == true`, Bit 7 (MY, Row Address Order) is asserted (`0x80`).
 *   - If `yMirror == true`, Bit 6 (MX, Column Address Order) is asserted (`0x40`).
 * - When `rotate == false` (portrait mode):
 *   - If `xMirror == true`, Bit 6 (MX) is asserted (`0x40`).
 *   - If `yMirror == true`, Bit 7 (MY) is asserted (`0x80`).
 *
 * Stores the orientation in `mRotateFlag`, invokes `updateLcdSize()` to synchronize canvas dimensions,
 * and writes the byte to the `MADCTL` register (`0x36`).
 * @see ST7789V::updateLcdSize(), ST7789V::cmd_t::MADCTL
 * @todo None.
 * @bug None.
 */
void ST7789V::setDirection(bool xMirror, bool yMirror, bool rotate)
{
	uint8_t memAccCtrl[] = {0x00};

	if(rotate)
	{
		memAccCtrl[0] |= 0x20;

		if(xMirror)
			memAccCtrl[0] |= 0x80;
		if(yMirror)
			memAccCtrl[0] |= 0x40;
	}
	else
	{
		if(xMirror)
			memAccCtrl[0] |= 0x40;
		if(yMirror)
			memAccCtrl[0] |= 0x80;
	}

	mRotateFlag = rotate;

	enable(TYPE_REG);

	updateLcdSize();

	sendReg(MADCTL, (uint8_t *)memAccCtrl, sizeof(memAccCtrl));

	disable();
}

/**
 * @details Sets the column and page boundaries of the drawing rectangle in ST7789V frame memory:
 * - Computes `endX = x + width - 1`, packs `[startX_MSB, startX_LSB, endX_MSB, endX_LSB]` into a 4-byte buffer,
 *   and writes to `COLUMN_ADDRESS_SET` (0x2A).
 * - Computes `endY = y + height - 1`, packs `[startY_MSB, startY_LSB, endY_MSB, endY_LSB]` into a 4-byte buffer,
 *   and writes to `PAGE_ADDRESS_SET` (0x2B).
 *
 * Subsequent pixel write commands (e.g. `MEMORY_WRITE` / 0x2C) will stream pixels strictly within these boundaries.
 * @see ST7789V::cmd_t::COLUMN_ADDRESS_SET, ST7789V::cmd_t::PAGE_ADDRESS_SET
 * @todo None.
 * @bug None.
 */
void ST7789V::setWindow(int16_t x, int16_t y, uint16_t width, uint16_t height)
{
	uint8_t data[4];
	uint16_t end;

	end = x + width - 1;
	data[0] = x >> 8;
	data[1] = x & 0xFF;
	data[2] = end >> 8;
	data[3] = end & 0xFF;

	sendReg(COLUMN_ADDRESS_SET, data, 4);
	
	end = y + height - 1;
	data[0] = y >> 8;
	data[1] = y & 0xFF;
	data[2] = end >> 8;
	data[3] = end & 0xFF;

	sendReg(PAGE_ADDRESS_SET, data, 4);
}

/**
 * @details Extracts coordinates and dimensions from `rect` (`rect.getPosition().getX()`, `getY()`,
 * `rect.getSize().getWidth()`, `getHeight()`) and configures the `COLUMN_ADDRESS_SET` and
 * `PAGE_ADDRESS_SET` boundaries on the display controller.
 * @see ST7789V::setWindow(int16_t, int16_t, uint16_t, uint16_t)
 * @todo None.
 * @bug None.
 */
void ST7789V::setWindow(Area rect)
{
	uint8_t data[4];
	uint16_t end;
	int16_t x = rect.getPosition().getX();
	int16_t y = rect.getPosition().getY();
	uint16_t width = rect.getSize().getWidth();
	uint16_t height = rect.getSize().getHeight();

	end = x + width - 1;
	data[0] = x >> 8;
	data[1] = x & 0xFF;
	data[2] = end >> 8;
	data[3] = end & 0xFF;

	sendReg(COLUMN_ADDRESS_SET, data, 4);
	
	end = y + height - 1;
	data[0] = y >> 8;
	data[1] = y & 0xFF;
	data[2] = end >> 8;
	data[3] = end & 0xFF;

	sendReg(PAGE_ADDRESS_SET, data, 4);
}


