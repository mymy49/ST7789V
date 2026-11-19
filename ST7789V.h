/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file ST7789V.h
 * @brief Base controller driver for the Sitronix ST7789V 240x320 single-chip TFT LCD controller.
 * @author Yoon-Ki Hong
 * @date 2015
 */

#ifndef YSS_MOD_TFT_LCD_DRIVER_ST7789V__H_
#define YSS_MOD_TFT_LCD_DRIVER_ST7789V__H_

#include "TurtleShip/TftLcdDriver.h"

/**
 * @class ST7789V
 * @brief Base hardware driver class for the Sitronix ST7789V TFT LCD controller.
 *
 * @details
 * `ST7789V` inherits `TftLcdDriver` from the `TurtleShip` submodule (`TurtleShip/TftLcdDriver.h`).
 * It encapsulates the low-level command protocol, screen orientation control (`setDirection()`),
 * and coordinate window setup (`setWindow()`) of the ST7789V chip.
 *
 * Concrete bus implementations (such as 4-wire SPI via `ST7789V_spi_with_Brush_RGB565`) override
 * the pure virtual transmission routines (`sendReg()`, `sendData()`, `enable()`, `disable()`, `updateLcdSize()`).
 *
 * ### Example Usage in Derived Driver
 * @code
 * #include <ST7789V/ST7789V_spi_with_Brush_RGB565.h>
 *
 * class CustomST7789V : public ST7789V_spi_with_Brush_RGB565
 * {
 * public:
 *     error_t initialize(config_t config) override
 *     {
 *         setConfig(config);
 *         reset();
 *
 *         enable(TYPE_REG);
 *         sendReg(SOFTWARE_RESET);
 *         thread::delay(120);
 *         sendReg(SLPOUT);
 *         thread::delay(120);
 *         sendReg(DISPON);
 *         disable();
 *
 *         setDirection(false, false, false);
 *         return error_t::ERROR_NONE;
 *     }
 * };
 * @endcode
 */
class ST7789V : public TftLcdDriver
{
public:
	/**
	 * @brief Constructs an `ST7789V` controller instance with default portrait orientation (`mRotateFlag = false`).
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning None.
	 * @note None.
	 * @pre None.
	 * @post Default state is initialized.
	 * @deprecated None.
	 */
	ST7789V(void);

	/**
	 * @brief Configures display orientation, mirroring, and rotation settings via the MADCTL register.
	 * @param[in] xMirror Horizontal mirror mode flag (true: flip X, false: normal X).
	 * @param[in] yMirror Vertical mirror mode flag (true: flip Y, false: normal Y).
	 * @param[in] rotate  Screen rotation flag (true: landscape / 90 degree rotation, false: portrait).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning This function transmits MADCTL command to the controller; bus interface must be active.
	 * @note Invokes `updateLcdSize()` to notify derived classes of the new dimensions.
	 * @pre The display interface must be initialized.
	 * @post Panel scanning direction and canvas dimensions are updated.
	 * @deprecated None.
	 */
	void setDirection(bool xMirror, bool yMirror, bool rotate);

protected:
	/**
	 * @enum cmd_t
	 * @brief ST7789V command register addresses defined in the datasheet.
	 */
	typedef enum
	{
		NOP = 0x00,                           ///< No operation
		SOFTWARE_RESET = 0x01,                ///< Software reset
		READ_DISP_ID = 0x04,                  ///< Read display ID
		READ_DISP_STATUS = 0x09,              ///< Read display status
		READ_DISP_MADCTRL = 0x0B,             ///< Read display MADCTL
		READ_DISP_PIXEL_FORMAT = 0x0C,        ///< Read display pixel format
		READ_DISP_IMAGE_FORMAT = 0x0D,        ///< Read display image format
		READ_DISP_SIGNAL_MODE = 0x0E,         ///< Read display signal mode
		READ_DISP_SELF_DIAGNOSTIC = 0x0F,     ///< Read display self-diagnostic result
		ENTER_SLEEP_MODE = 0x10,              ///< Sleep in
		SLPOUT = 0x11,                        ///< Sleep out
		PARTIAL_MODE_ON = 0x12,               ///< Partial display mode on
		NORMAL_DISP_MODE_ON = 0x13,           ///< Normal display mode on
		DISP_INVERSION_OFF = 0x20,            ///< Display inversion off
		DISP_INVERSION_ON = 0x21,             ///< Display inversion on
		GAMMA_SET = 0x26,                     ///< Gamma set
		DISPLAY_OFF = 0x28,                   ///< Display off
		DISPON = 0x29,                        ///< Display on
		COLUMN_ADDRESS_SET = 0x2A,            ///< Column address set (CASET)
		PAGE_ADDRESS_SET = 0x2B,              ///< Page address set (RASET)
		MEMORY_WRITE = 0x2C,                  ///< Memory write (RAMWR)
		COLOR_SET = 0x2D,                     ///< Color set
		MEMORY_READ = 0x2E,                   ///< Memory read (RAMRD)
		PARTIAL_AREA = 0x30,                  ///< Partial area definition
		VERT_SCROLL_DEFINITION = 0x33,        ///< Vertical scrolling definition
		TEARING_EFFECT_LINE_OFF = 0x34,       ///< Tearing effect line off
		TEARING_EFFECT_LINE_ON = 0x35,        ///< Tearing effect line on
		MADCTL = 0x36,                        ///< Memory data access control
		VERT_SCROLL_START_ADDRESS = 0x37,     ///< Vertical scroll start address
		IDLE_MODE_OFF = 0x38,                 ///< Idle mode off
		IDLE_MODE_ON = 0x39,                  ///< Idle mode on
		COLMOD = 0x3A,                        ///< Interface pixel format
		WRITE_MEMORY_CONTINUE = 0x3C,         ///< Write memory continue
		READ_MEMORY_CONTINUE = 0x3E,          ///< Read memory continue
		SET_TEAR_SCANLINE = 0x44,             ///< Set tear scanline
		GET_SCANLINE = 0x45,                  ///< Get scanline
		WRITE_DISPLAY_BRIGHTNESS = 0x51,      ///< Write display brightness
		READ_DISPLAY_BRIGHTNESS = 0x52,       ///< Read display brightness
		WRITE_CTRL_DISPLAY = 0x53,            ///< Write CTRL display
		READ_CTRL_DISPLAY = 0x54,             ///< Read CTRL display
		WRCACE = 0x55,                        ///< Write content adaptive brightness control
		READ_CONTENT_ADAPT_BRIGHTNESS = 0x56, ///< Read content adaptive brightness control
		WRITE_MIN_CAB_LEVEL = 0x5E,           ///< Write CABC minimum brightness
		READ_MIN_CAB_LEVEL = 0x5F,            ///< Read CABC minimum brightness
		INTERFACE_MODE_CON = 0xB0,            ///< Interface control
		FRAME_RATE = 0xB1,                    ///< Frame rate control
		PORCTL = 0xB2,                        ///< Porch control
		DISPLAY_INVERSION_CON = 0xB4,         ///< Display inversion control
		BLANKING_PORCH_CON = 0xB5,            ///< Blanking porch control
		DISPLAY_CTRL = 0xB6,                  ///< Display function control
		GATE_CTRL = 0xB7,                     ///< Gate control
		VCOMS = 0xBB,                         ///< VCOM setting
		LCMCTLR = 0xC0,                       ///< LCM control
		POWER_CTRL2 = 0xC1,                   ///< Power control 2
		VDVVRHEN = 0xC2,                      ///< VDV and VRH command enable
		VRHS = 0xC3,                          ///< VRH set
		VDVS = 0xC4,                          ///< VDV set
		VCOM_CTRL1 = 0xC5,                    ///< VCOM offset set
		FRCTRL2 = 0xC6,                       ///< Frame rate control in normal mode
		VCOM_CTRL2 = 0xc7,                    ///< CABC control
		POWER_A = 0xcb,                       ///< Power control A
		POWER_B = 0xcf,                       ///< Power control B
		PWCTRL1 = 0xD0,                       ///< Power control 1
		READ_ID1 = 0xDA,                      ///< Read ID1
		READ_ID2 = 0xDB,                      ///< Read ID2
		READ_ID3 = 0xDC,                      ///< Read ID3
		PVGAMCTRL = 0xE0,                     ///< Positive voltage gamma control
		NVGAMCTRL = 0xE1,                     ///< Negative voltage gamma control
		DTCA = 0xE8,                          ///< Digital gamma look-up table control A
		DTCB = 0xea,                          ///< Digital gamma look-up table control B
		POWER_SEQ = 0xed,                     ///< Power on sequence control
		SET_CONFIG = 0xF0,                    ///< Enable extension command
		GAMMA3_FUNC_DIS = 0xf2,               ///< Gamma 3 function disable
		PRC = 0xf7                            ///< Pump ratio control
	}cmd_t;

	/**
	 * @enum type_t
	 * @brief Transmission phase type for the display bus interface.
	 */
	typedef enum
	{
		TYPE_REG = 0, ///< Command / Register index phase
		TYPE_DATA     ///< Parameter / Framebuffer pixel data phase
	}type_t;

	bool mRotateFlag; ///< Indicates whether landscape rotation mode (90 or 270 degrees) is active.

	/**
	 * @brief Sets the active pixel window rectangle on the ST7789V display memory.
	 * @param[in] x      Starting horizontal pixel coordinate (column).
	 * @param[in] y      Starting vertical pixel coordinate (page/row).
	 * @param[in] width  Width of the active window in pixels (default: 1).
	 * @param[in] height Height of the active window in pixels (default: 1).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Must be called while bus is enabled for register commands (`TYPE_REG`).
	 * @note Transmits COLUMN_ADDRESS_SET (0x2A) and PAGE_ADDRESS_SET (0x2B) to controller.
	 * @pre Controller must be active.
	 * @post Hardware memory write pointer is repositioned to the window boundaries.
	 * @deprecated None.
	 */
	void setWindow(int16_t x, int16_t y, uint16_t width = 1, uint16_t height = 1);

	/**
	 * @brief Sets the active pixel window on the ST7789V display memory using an `Area` descriptor.
	 * @param[in] rect Drawing area containing top-left coordinate and dimensions.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Must be called while bus is enabled for register commands (`TYPE_REG`).
	 * @note Convenience overload extracting `Position` and `Size` from `Area`.
	 * @pre Controller must be active.
	 * @post Hardware memory write pointer is repositioned to the window boundaries.
	 * @deprecated None.
	 */
	void setWindow(Area rect);

	/**
	 * @brief Enables and prepares the physical interface bus for register or data transmission.
	 * @param[in] type Bus transmission type (`TYPE_REG` for commands, `TYPE_DATA` for pixel stream).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Must be paired with `disable()`.
	 * @note Pure virtual method to be implemented by the physical bus driver.
	 * @pre Bus driver must be initialized.
	 * @post Bus is locked and Chip Select is asserted.
	 * @deprecated None.
	 */
	virtual void enable(type_t type = TYPE_REG) = 0;

	/**
	 * @brief Disables and releases the physical interface bus.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Must be called after every `enable()` sequence.
	 * @note Deasserts Chip Select and unlocks the communication peripheral.
	 * @pre Bus must currently be enabled.
	 * @post Bus is released.
	 * @deprecated None.
	 */
	virtual void disable(void) = 0;

	/**
	 * @brief Updates logical canvas dimensions when rotation mode changes.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Pure virtual method to be implemented by derived drawing classes.
	 * @note Called automatically by `setDirection()`.
	 * @pre None.
	 * @post Drawing buffer dimensions are synchronized with panel orientation.
	 * @deprecated None.
	 */
	virtual void updateLcdSize(void) = 0;

	/**
	 * @brief Sends a command byte (register address) to the ST7789V controller.
	 * @param[in] cmd Command register enum (`cmd_t`).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Interface must be enabled with `TYPE_REG`.
	 * @note Pure virtual transmission primitive.
	 * @pre Bus must be active.
	 * @post Command byte is written to the display.
	 * @deprecated None.
	 */
	virtual void sendReg(cmd_t cmd) = 0;

	/**
	 * @brief Sends a command byte followed by an 8-bit parameter data byte.
	 * @param[in] cmd  Command register enum (`cmd_t`).
	 * @param[in] data Single 8-bit parameter byte.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Interface must be configured appropriately.
	 * @note Writes command in command mode, then toggles to data mode for the parameter.
	 * @pre Bus must be active.
	 * @post Register and parameter are transferred.
	 * @deprecated None.
	 */
	virtual void sendReg(cmd_t cmd, uint8_t data) = 0;

	/**
	 * @brief Sends a command byte followed by multiple 8-bit parameter data bytes.
	 * @param[in] cmd   Command register enum (`cmd_t`).
	 * @param[in] data  Pointer to the array of parameter bytes.
	 * @param[in] count Number of parameter bytes to transmit.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Parameter buffer must remain valid during transfer.
	 * @note Transmits command byte, then streams parameter payload.
	 * @pre Bus must be active.
	 * @post Multi-byte register configuration is completed.
	 * @deprecated None.
	 */
	virtual void sendReg(cmd_t cmd, uint8_t *data, uint32_t count) = 0;
	
	/**
	 * @brief Streams 16-bit pixel data array directly into display memory.
	 * @param[in] data  Pointer to 16-bit RGB565 pixel buffer.
	 * @param[in] count Number of 16-bit pixels to transfer.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Window must be set beforehand with `setWindow()` and `MEMORY_WRITE`.
	 * @note Bus must be set to 16-bit data transmission mode.
	 * @pre Interface must be enabled with `TYPE_DATA`.
	 * @post Pixels are written to ST7789V display RAM.
	 * @deprecated None.
	 */
	virtual void sendData(uint16_t *data, uint32_t count) = 0;
};

#endif
