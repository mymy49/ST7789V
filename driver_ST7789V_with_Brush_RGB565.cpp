/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file driver_ST7789V_with_Brush_RGB565.cpp
 * @brief Implementation of hardware-accelerated 2D drawing methods on ST7789V in Little-Endian RGB565.
 * @author Yoon-Ki Hong
 * @date 2015
 */

#include <config.h>

#include "ST7789V_with_Brush_RGB565.h"
//#include <gui/BitmapDrawingCalculator.h>

/**
 * @details Default constructor for `ST7789V_with_Brush_RGB565`.
 * @see ST7789V_with_Brush_RGB565::drawDot()
 * @todo None.
 * @bug None.
 */
ST7789V_with_Brush_RGB565::ST7789V_with_Brush_RGB565(void)
{

}

/**
 * @details Updates logical canvas width and height when display orientation changes.
 * @see ST7789V::setDirection()
 * @todo Synchronize canvas dimensions with physical panel resolution upon rotation.
 * @bug None.
 */
void ST7789V_with_Brush_RGB565::updateLcdSize(void)
{

}

/**
 * @details Writes a single 16-bit RGB565 dot to the LCD panel:
 * 1. Sets a 1x1 write window at `(x, y)` via `setWindow(x, y)`.
 * 2. Issues `MEMORY_WRITE` (0x2C) command in register mode (`TYPE_REG`).
 * 3. Switches to data mode (`TYPE_DATA`) and transmits the 16-bit color code `mBrushColor.getCode()`.
 * @see ST7789V::setWindow(), Brush::drawDot()
 * @todo None.
 * @bug None.
 */
void ST7789V_with_Brush_RGB565::drawDot(int16_t x, int16_t y)
{
	uint16_t code = mBrushColor.getCode();

	enable(TYPE_REG);
	setWindow(x, y);
	sendReg(MEMORY_WRITE);
	disable();

	enable(TYPE_DATA);
	sendData(&code, 1);
	disable();
}

/**
 * @details Alpha-blended dot drawing is currently not supported on write-only SPI TFT LCDs.
 * @see Brush::blendDot()
 * @todo Implement read-modify-write blendDot if bidirectional bus or frame buffer memory is available.
 * @bug None.
 */
void ST7789V_with_Brush_RGB565::blendDot(int16_t x, int16_t y, uint8_t alpha)
{
	// Currently not supported on write-only hardware.
}

/**
 * @details Fills a rectangular region on the display with a solid color in chunked bursts using `mFb`:
 * 1. Verifies that `mFb` (working framebuffer) is allocated.
 * 2. Calculates the maximum number of scanlines `fheight` that fit within `mFb->getMaxPixelCount()`.
 * 3. Sets `mFb` dimensions to `fwidth x fheight`, fills `mFb` with `color`.
 * 4. Configures the ST7789V memory window to `(x, y, width, height)` and sends `MEMORY_WRITE`.
 * 5. Streams repeated chunks of `mFb` buffer over the bus to complete the full rectangle.
 * @see ST7789V::setWindow(), Brush::fillRectangular()
 * @todo None.
 * @bug None.
 */
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

/**
 * @details Translates a linear byte offset into `(x, y)` coordinates on the LCD canvas and fills
 * `count` consecutive horizontal pixels with `color` using the working framebuffer `mFb`.
 * @see ST7789V_with_Brush_RGB565::fillRectBase(), Brush::fillDotArray()
 * @todo None.
 * @bug None.
 */
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

/**
 * @details Dispatches clipped bitmap rendering requests to `drawBitmapRgb565()` or `drawBitmapArgb1555()`
 * according to the pixel encoding format specified in `bitmap.type`.
 * @see ST7789V_with_Brush_RGB565::drawBitmapRgb565(), ST7789V_with_Brush_RGB565::drawBitmapArgb1555()
 * @todo None.
 * @bug None.
 */
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

/**
 * @details Internal routine for rendering a clipped RGB565 bitmap into the target canvas area.
 * @see ST7789V_with_Brush_RGB565::drawBitmapBase()
 * @todo Re-enable BitmapDrawingCalculator pipeline for clipped off-screen blitting when needed.
 * @bug None.
 */
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

/**
 * @details Draws an RGB565 bitmap image directly onto the LCD:
 * - If the bitmap extends outside the physical canvas boundaries (`isOutsideCanvas() == true`),
 *   calculates the clipped visible area via `calculateValidArea()` and streams each scanline
 *   individually with appropriate row stride offsets (`mSrcOffset`).
 * - If the bitmap lies entirely within canvas boundaries, sets the full destination window at once
 *   and transfers all `width * height` 16-bit words in a single continuous burst.
 * @see ST7789V::setWindow(), Brush::drawBitmap()
 * @todo None.
 * @bug None.
 */
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
			enable(TYPE_REG);
			setWindow(area);
			sendReg(MEMORY_WRITE);
			disable();
			enable(TYPE_DATA);
			sendData(&fb[offset], width);
			disable();
			area.addY(1);
			offset += bitmap.width;
		}
	}
	else 
	{
		enable(TYPE_REG);
		setWindow(area);
		sendReg(MEMORY_WRITE);
		disable();
		enable(TYPE_DATA);
		sendData(fb, bitmap.width * bitmap.height);
		disable();
	}
}

/**
 * @details Internal routine for rendering a clipped ARGB1555 (1-bit alpha transparency) bitmap.
 * @see ST7789V_with_Brush_RGB565::drawBitmapBase()
 * @todo Re-enable ARGB1555 masked transparency blit pipeline when needed.
 * @bug None.
 */
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


