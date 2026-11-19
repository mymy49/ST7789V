/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file ST7789V_with_Brush_RGB565.h
 * @brief Direct 2D drawing bridge connecting the ST7789V LCD controller to TurtleShip's Little-Endian RGB565 Brush.
 * @author Yoon-Ki Hong
 * @date 2015
 */

#ifndef YSS_MOD_TFT_LCD_DRIVER_ST7796__H_
#define YSS_MOD_TFT_LCD_DRIVER_ST7796__H_

#include "TurtleShip/BrushTftLcdRgb565LE.h"
#include "ST7789V.h"

/**
 * @class ST7789V_with_Brush_RGB565
 * @brief Hardware-accelerated 2D drawing canvas bridge for ST7789V displays in Little-Endian RGB565 format.
 *
 * @details
 * `ST7789V_with_Brush_RGB565` utilizes multiple inheritance:
 * - **`ST7789V`** (local module): Provides register access, hardware window clipping, and display orientation controls.
 * - **`BrushTftLcdRgb565LE`** (from the `TurtleShip` submodule): Provides device-independent 2D drawing primitives
 *   (lines, circles, rectangles, fonts, and bitmaps) designed for direct TFT LCD surfaces.
 *
 * By overriding the hardware primitives (`drawDot()`, `fillRectBase()`, `fillDotArray()`, and `drawBitmapBase()`),
 * this class transfers pixel drawing commands directly to the ST7789V controller memory without requiring a full-screen
 * framebuffer in system RAM. Off-screen sub-regions are streamed through an optional working framebuffer (`mFb`).
 *
 * ### Example Usage in Application
 * @code
 * #include <ST7789V/ST7789V_spi_with_Brush_RGB565.h>
 *
 * // Usually accessed via a board-level driver such as Touch_LCD_Shield_for_Arduino_2_8_inch
 * extern Touch_LCD_Shield_for_Arduino_2_8_inch lcd;
 *
 * void drawShapes(void)
 * {
 *     lcd.lock();
 *     lcd.setBackgroundColor({0x00, 0x00, 0x00});
 *     lcd.clear();
 *
 *     // Drawing primitives inherited from Brush
 *     lcd.setBrushColor({0xFF, 0x00, 0x00});
 *     lcd.drawLine({10, 10}, {100, 10});
 *     lcd.drawCircle({60, 60}, 30);
 *     lcd.fillRectangular({10, 110, 80, 40});
 *     lcd.unlock();
 * }
 * @endcode
 */
class ST7789V_with_Brush_RGB565 : public ST7789V, public BrushTftLcdRgb565LE
{
public:
	/**
	 * @brief Constructs an `ST7789V_with_Brush_RGB565` drawing surface instance.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning None.
	 * @note None.
	 * @pre None.
	 * @post Instance is created.
	 * @deprecated None.
	 */
	ST7789V_with_Brush_RGB565(void);

	// Brush Overrides
	/**
	 * @brief Draws a single pixel dot directly onto the LCD surface at `(x, y)` using the active brush color.
	 * @param[in] x Horizontal pixel coordinate on the display canvas.
	 * @param[in] y Vertical pixel coordinate on the display canvas.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Bypasses buffering; sends command and data directly over the bus.
	 * @note Overrides `Brush::drawDot()`. Sets 1x1 window at `(x, y)` and writes one 16-bit RGB565 word.
	 * @pre Bus and controller must be initialized.
	 * @post The pixel at `(x, y)` is updated on the physical display.
	 * @deprecated None.
	 */
	void drawDot(int16_t x, int16_t y) override;

	/**
	 * @brief Alpha-blended dot drawing (currently unsupported on direct LCD hardware).
	 * @param[in] x     Horizontal pixel coordinate.
	 * @param[in] y     Vertical pixel coordinate.
	 * @param[in] alpha Transparency weight (0 to 255).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Hardware read-modify-write is not supported over write-only SPI; method is a no-op.
	 * @note Overrides `Brush::blendDot()`.
	 * @pre None.
	 * @post No change.
	 * @deprecated None.
	 */
	void blendDot(int16_t x, int16_t y, uint8_t alpha) override;

	/**
	 * @brief Updates logical canvas width and height when display orientation or resolution changes.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::updateLcdSize()`.
	 * @note Intended to synchronize canvas size with panel dimensions.
	 * @pre None.
	 * @post Canvas dimensions are synchronized.
	 * @deprecated None.
	 */
	void updateLcdSize(void) override;

	/**
	 * @brief Fills a rectangular canvas region with a solid color, utilizing chunked buffer streaming.
	 * @param[in] x      Top-left horizontal coordinate.
	 * @param[in] y      Top-left vertical coordinate.
	 * @param[in] width  Width of the rectangle in pixels.
	 * @param[in] height Height of the rectangle in pixels.
	 * @param[in] color  Solid fill `Color`.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Requires working frame buffer (`mFb`) to be attached via `setFrameBuffer()`.
	 * @note Overrides `Brush::fillRectBase()`. Divides the rectangle into chunked horizontal spans to fit within `mFb`.
	 * @pre Display interface and working framebuffer must be initialized.
	 * @post The specified rectangle on the LCD is filled with `color`.
	 * @deprecated None.
	 */
	void fillRectBase(int16_t x, int16_t y, uint16_t width, uint16_t height, Color color) override;

protected :
	/**
	 * @brief Fills a linear consecutive run of pixels starting at buffer offset with a solid color.
	 * @param[in] offset Linear byte offset within the display coordinate space.
	 * @param[in] count  Number of consecutive pixels to fill.
	 * @param[in] color  Solid fill `Color`.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Requires working frame buffer (`mFb`) to be attached via `setFrameBuffer()`.
	 * @note Overrides `Brush::fillDotArray()`. Translates byte offset to `(x, y)` and streams chunks via `mFb`.
	 * @pre Working framebuffer must be attached.
	 * @post The pixel span on the LCD is updated with `color`.
	 * @deprecated None.
	 */
	void fillDotArray(uint32_t offset, uint32_t count, Color color) override;

	/**
	 * @brief Draws a clipped bitmap image within the designated destination canvas area.
	 * @param[in] canvasSize       Logical dimensions of the drawing canvas.
	 * @param[in] targetCanvasArea Destination clipping area on the canvas.
	 * @param[in] bitmapPos        Placement coordinates of the bitmap.
	 * @param[in] bitmap           Bitmap descriptor containing format, dimensions, and pixel array.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `Brush::drawBitmapBase()`.
	 * @note Dispatches to `drawBitmapRgb565()` or `drawBitmapArgb1555()` based on bitmap type.
	 * @pre Display must be ready.
	 * @post Bitmap image is rendered to the LCD surface.
	 * @deprecated None.
	 */
	void drawBitmapBase(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap) override;

	/**
	 * @brief Draws a full unclipped bitmap image at the specified canvas position.
	 * @param[in] pos    Top-left coordinate on the canvas where the bitmap will be drawn.
	 * @param[in] bitmap Bitmap descriptor (`bitmap_t`) containing pixel data and dimensions.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `Brush::drawBitmapBase()`.
	 * @note Clips image lines that extend outside canvas bounds and streams RGB565 pixel lines directly.
	 * @pre Bus and display must be initialized.
	 * @post The bitmap is transferred directly to the LCD controller.
	 * @deprecated None.
	 */
	void drawBitmapBase(Position pos, const bitmap_t bitmap) override;

private :
	/**
	 * @brief Internal helper to draw an RGB565 bitmap clipped to the specified destination area.
	 * @param[in] canvasSize       Logical dimensions of the canvas.
	 * @param[in] targetCanvasArea Destination clipping area on the canvas.
	 * @param[in] bitmapPos        Placement coordinates of the bitmap.
	 * @param[in] bitmap           Bitmap descriptor (`BITMAP_TYPE_RGB565`).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning None.
	 * @note None.
	 * @pre None.
	 * @post None.
	 * @deprecated None.
	 */
	void drawBitmapRgb565(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap);

	/**
	 * @brief Internal helper to draw an ARGB1555 1-bit transparent bitmap clipped to the destination area.
	 * @param[in] canvasSize       Logical dimensions of the canvas.
	 * @param[in] targetCanvasArea Destination clipping area on the canvas.
	 * @param[in] bitmapPos        Placement coordinates of the bitmap.
	 * @param[in] bitmap           Bitmap descriptor (`BITMAP_TYPE_ARGB1555`).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning None.
	 * @note None.
	 * @pre None.
	 * @post None.
	 * @deprecated None.
	 */
	void drawBitmapArgb1555(Size canvasSize, Area targetCanvasArea, Position bitmapPos, const bitmap_t bitmap);
};

#endif


