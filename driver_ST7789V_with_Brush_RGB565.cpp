/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

#include <config.h>

#include "ST7789V_with_Brush_RGB565.h"
//#include <gui/BitmapDrawingCalculator.h>

ST7789V_with_Brush_RGB565::ST7789V_with_Brush_RGB565(void)
{

}

void ST7789V_with_Brush_RGB565::updateLcdSize(void)
{

}

void ST7789V_with_Brush_RGB565::drawDot(int16_t x, int16_t y)
{
	uint16_t code = mBrushColor.getCode();

	enable(TYPE_REG);
	setWindow(x, y);
	sendReg(MEMORY_WRITE);
	disable();

	enable(TYPE_DATA);
	sendData(&code, 2);
	disable();
}

void ST7789V_with_Brush_RGB565::blendDot(int16_t x, int16_t y, uint8_t alpha)
{
// 현재 지원하지 않습니다.
}

void ST7789V_with_Brush_RGB565::fillRectBase(int16_t x, int16_t y, uint16_t width, uint16_t height, Color color)
{
	if(mFb == nullptr)
		return;

	uint32_t maxPixelCount = mFb->getMaxPixelCount();
	uint16_t fwidth, fheight;

	fwidth = width;
	fheight = maxPixelCount / fwidth;
	if(mFb->setSize(fwidth, fheight) == false)
		return;

	uint32_t loop, remain;
	uint16_t *fb = (uint16_t*)mFb->getFrameBuffer();

	loop = height / fheight;
	remain = height % fheight;

	mFb->setBrushColor(color);
	mFb->fill();

	enable(TYPE_REG);
	setWindow(x, y, width, height);
	sendReg(MEMORY_WRITE);
	disable();

	enable(TYPE_DATA);
	for(uint32_t i=0;i<loop;i++)
	{
		sendData((uint16_t*)mFb->getFrameBuffer(), fwidth * fheight);
	}

	if(remain)
	{
		sendData((uint16_t*)mFb->getFrameBuffer(), fwidth * remain);
	}
	disable();
}

void ST7789V_with_Brush_RGB565::fillDotArray(uint32_t offset, uint32_t count, Color color)
{
	if(mFb == nullptr)
		return;
	
	uint16_t width = getLcdSize().getWidth(), height = getLcdSize().getHeight();
	int16_t y = (offset >> 1) / width, x = (offset - width * 2 * y) >> 1;
	
	mFb->setSize(count, 1);
	mFb->setBrushColor(color);
	mFb->fill();

	enable(TYPE_REG);
	setWindow(x, y, count, 1);
	sendReg(MEMORY_WRITE);
	disable();

	enable(TYPE_DATA);
	sendData((uint16_t*)mFb->getFrameBuffer(), count);
	disable();
}

void ST7789V_with_Brush_RGB565::drawBitmapBase(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap)
{
	switch(bitmap.type)
	{
	case BITMAP_TYPE_RGB565 :
		drawBitmapRgb565(canvasSize, targetCanvasArea, bitmapPos, bitmap);
		break;
	
	case BITMAP_TYPE_ARGB1555 :
		drawBitmapArgb1555(canvasSize, targetCanvasArea, bitmapPos, bitmap);
		break;

	default :
		return;
	}
}

void ST7789V_with_Brush_RGB565::drawBitmapRgb565(Size canvasSize, Area canvasDesArea, Position bitmapPos, const bitmap_t bitmap)
{
	//if(bitmap.type != BITMAP_TYPE_RGB565 || mFb == nullptr)
	//	return;
	
	//Area bitmapArea = {bitmapPos, {bitmap.width, bitmap.height}};
	//BitmapDrawingCalculator bdc(canvasSize, canvasDesArea, bitmapArea);
	//uint16_t *src;
	
	//if(bdc.calculate() == false)
	//	return;

	//if(bdc.getTrimedBitmapWidth() != bitmap.width)
	//{
	//	bitmap_t bitmapCopy;
	//	Position pos;
		
	//	src = (uint16_t*)bitmap.data;
	//	src = &src[bdc.getBitmapSrcFrameBufferOffset()];

	//	bitmapCopy.data = (uint8_t*)src;
	//	bitmapCopy.height = bdc.getTrimedBitmapHeight();
	//	bitmapCopy.width = bitmap.width;
	//	bitmapCopy.type = BITMAP_TYPE_RGB565;

	//	pos = bdc.getTrimedBitmapSrcArea().getPosition();
	//	pos.setMinPosition(0, 0);

	//	mFb->setSize(bdc.getTrimedCanvasDesArea().getSize());
	//	mFb->drawBitmap(pos, bitmapCopy); 

	//	src = (uint16_t*)mFb->getFrameBuffer();
	//}
	//else
	//{
	//	src = (uint16_t*)bitmap.data;
	//	src = &src[bdc.getBitmapSrcFrameBufferOffset()];
	//}

	//enable(TYPE_REG);
	//setWindows(bdc.getTrimedCanvasDesArea());
	//sendReg(MEMORY_WRITE);
	//disable();

	//enable(TYPE_DATA);
	//sendData(src, bdc.getTrimedCanvasDesAreaValue());
	//disable();
}

void ST7789V_with_Brush_RGB565::drawBitmapBase(Position pos, const bitmap_t bitmap)
{
	if(bitmap.type != BITMAP_TYPE_RGB565)
		return;

	Area area{pos, {bitmap.width, bitmap.height}};
	uint16_t *fb = (uint16_t*)bitmap.data;

	setCalculatorSource(area);

	if(isOutsideCanvas())
	{
		area = calculateValidArea();
		int32_t count = area.getSize().getHeight();
		int32_t width = area.getSize().getWidth();
		int32_t offset = mSrcOffset;
		area.setHeight(1);
	
		for(int32_t i = 0; i < count; i++)
		{
			setWindow(area);
			sendData(&fb[offset], width);
			area.addY(1);
			offset += bitmap.width;
		}
	}
	else 
	{
		setWindow(area);
		sendData(fb, bitmap.width * bitmap.height);
	}
}

void ST7789V_with_Brush_RGB565::drawBitmapArgb1555(Size canvasSize, Area canvasDesArea, Position bitmapPos, const bitmap_t bitmap)
{
	//if(bitmap.type != BITMAP_TYPE_ARGB1555 || mFb == nullptr)
	//	return;
	
	//Area bitmapArea = {bitmapPos, {bitmap.width, bitmap.height}};
	//BitmapDrawingCalculator bdc(canvasSize, canvasDesArea, bitmapArea);
	
	//if(bdc.calculate() == false)
	//	return;

	//uint16_t *src;
	//bitmap_t bitmapCopy;
	//Position pos;

	//src = (uint16_t*)bitmap.data;
	//src = &src[bdc.getBitmapSrcFrameBufferOffset()];

	//bitmapCopy.data = (uint8_t*)src;
	//bitmapCopy.height = bdc.getTrimedBitmapHeight();
	//bitmapCopy.width = bitmap.width;
	//bitmapCopy.type = BITMAP_TYPE_RGB565;

	//pos = bdc.getTrimedBitmapSrcArea().getPosition();
	//pos.setMinPosition(0, 0);

	//mFb->setSize(bdc.getTrimedCanvasDesArea().getSize());
	//mFb->setBackgroundColor({0x00, 0x00, 0x00});
	//mFb->clear();
	//mFb->drawBitmap(pos, bitmapCopy); 

	//enable(TYPE_REG);
	//setWindow(bdc.getTrimedCanvasDesArea());
	//sendReg(MEMORY_WRITE);
	//disable();

	//enable(TYPE_DATA);
	//sendData((uint16_t*)mFb->getFrameBuffer(), bdc.getTrimedCanvasDesAreaValue());
	//disable();
}

