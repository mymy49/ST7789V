/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

#include "ST7789V.h"
#include "TurtleShip/Area.h"

ST7789V::ST7789V(void)
{
	mRotateFlag = false;
}

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

