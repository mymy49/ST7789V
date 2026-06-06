/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

#ifndef YSS_MOD_TFT_LCD_DRIVER_ST7796__H_
#define YSS_MOD_TFT_LCD_DRIVER_ST7796__H_

#include "BrushTftLcdRgb565LE.h"
#include "ST7789V.h"

class ST7789V_with_Brush_RGB565 : public ST7789V, public BrushTftLcdRgb565LE
{
public:
	ST7789V_with_Brush_RGB565(void);

	// Brush
	void drawDot(int16_t x, int16_t y) override;

	void blendDot(int16_t x, int16_t y, uint8_t alpha) override;

	void updateLcdSize(void) override;

	void fillRectBase(int16_t x, int16_t y, uint16_t width, uint16_t height, Color color) override;

protected :
	void fillDotArray(uint32_t offset, uint32_t count, Color color);

	void drawBitmapBase(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap) override;

	void drawBitmapBase(Position pos, const bitmap_t bitmap) override;

private :
	void drawBitmapRgb565(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap);

	void drawBitmapArgb1555(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap);
};

#endif

