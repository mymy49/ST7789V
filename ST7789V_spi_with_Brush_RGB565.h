/*
 * Copyright (c) 2015 Yoon-Ki Hong
 *
 * This file is subject to the terms and conditions of the MIT License.
 * See the file "LICENSE" in the main directory of this archive for more details.
 */

/**
 * @file ST7789V_spi_with_Brush_RGB565.h
 * @brief 4-wire SPI interface specialization of the ST7789V TFT LCD controller driver.
 * @author Yoon-Ki Hong
 * @date 2015
 */

#ifndef YSS_MOD_TFT_LCD_DRIVER_ST7796_SPI__H_
#define YSS_MOD_TFT_LCD_DRIVER_ST7796_SPI__H_

#include <drv/Gpio.h>
#include <drv/Spi.h>
#include "ST7789V_with_Brush_RGB565.h"

/**
 * @class ST7789V_spi_with_Brush_RGB565
 * @brief SPI bus specialization of the `ST7789V_with_Brush_RGB565` LCD driver.
 *
 * @details
 * `ST7789V_spi_with_Brush_RGB565` inherits `ST7789V_with_Brush_RGB565` and implements physical communication
 * over a standard 4-wire SPI bus:
 * - **SPI peripheral** (`Spi &spi`): Handles serial data serialization for both 8-bit command registers and 16-bit pixel streams.
 * - **Chip Select (`chipSelect`)**: Controls SPI slave select framing.
 * - **Data/Command (`dataCommand`)**: D/C pin toggling (Low: register index, High: parameter/display RAM).
 * - **Hardware Reset (`reset`)**: Optional GPIO reset pin pulsing.
 *
 * It overrides pure virtual communication routines (`sendReg()`, `sendData()`, `enable()`, `disable()`)
 * and leaves `initialize()` as pure virtual for panel-specific initialization sequences (e.g. gamma and power curves)
 * implemented in board drivers such as `Touch_LCD_Shield_for_Arduino_2_8_inch`.
 *
 * ### Example Usage in Board Initialization
 * @code
 * #include <2.8inch_TFT_Touch_Shield_Rev2.1/Touch_LCD_Shield_for_Arduino_2_8_inch.h>
 *
 * extern Touch_LCD_Shield_for_Arduino_2_8_inch lcd;
 * extern FrameBufferRgb565LE brush;
 *
 * void initDisplay(void)
 * {
 *     Touch_LCD_Shield_for_Arduino_2_8_inch::config_t config =
 *     {
 *         spi0,           // Spi &spi
 *         {&gpioA, 3},    // pin_t chipSelect
 *         {&gpioA, 6},    // pin_t dataCommand
 *         {0, 0}          // pin_t reset (unconnected/tied high)
 *     };
 *
 *     brush.malloc(5000);
 *     lcd.initialize(config);
 *     lcd.setFrameBuffer(brush);
 *     lcd.setBackgroundColor({0x00, 0x00, 0x00});
 *     lcd.clear();
 * }
 * @endcode
 */
class ST7789V_spi_with_Brush_RGB565 : public ST7789V_with_Brush_RGB565
{
public :
	/**
	 * @struct config_t
	 * @brief Hardware connection descriptor for the SPI bus and GPIO control lines.
	 */
	typedef struct 
	{
		Spi &spi;           ///< Reference to the hardware SPI peripheral.
		pin_t chipSelect;   ///< GPIO output pin connected to LCD CS (Chip Select).
		pin_t dataCommand;  ///< GPIO output pin connected to LCD DC (Data/Command).
		pin_t reset;        ///< GPIO output pin connected to LCD RESET (optional, set null if tied to VDD).
	}config_t;

	/**
	 * @brief Constructs an `ST7789V_spi_with_Brush_RGB565` instance with default 15 MHz SPI bus specifications.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning None.
	 * @note None.
	 * @pre None.
	 * @post Default bus specifications are assigned.
	 * @deprecated None.
	 */
	ST7789V_spi_with_Brush_RGB565(void);

	/**
	 * @brief Initializes the LCD panel hardware and controller register sequence.
	 * @param[in] config Hardware configuration containing SPI peripheral and GPIO pins.
	 * @return `error_t::ERROR_NONE` on success, or appropriate error code on failure.
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Pure virtual method; derived board classes must supply panel-specific init commands.
	 * @note Sets up GPIO modes, performs reset, and configures panel power/gamma.
	 * @pre Peripherals (SPI, GPIO clock) must be enabled in BSP.
	 * @post Display panel is powered on and ready for drawing.
	 * @deprecated None.
	 */
	virtual error_t initialize(config_t config) = 0;

	/**
	 * @brief Overrides default SPI clock speeds and transmission modes for register and pixel data transfers.
	 * @param[in] regSpec  SPI specification (clock mode, max frequency, 8-bit width) for commands.
	 * @param[in] dataSpec SPI specification (clock mode, max frequency, 16-bit width) for pixel streams.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Bus frequencies must not exceed the maximum ratings of both the MCU and ST7789V controller.
	 * @note Default clock speed is 15 MHz in Mode 0.
	 * @pre None.
	 * @post Custom SPI transfer profiles are registered.
	 * @deprecated None.
	 */
	void setSpiSpecification(const Spi::specification_t &regSpec, const Spi::specification_t &dataSpec);

	/**
	 * @brief Performs a hardware reset pulse via the `reset` GPIO pin.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Delays thread execution by 200 ms during the low-pulse and recovery period.
	 * @note No-op if `mRstPin.port` is `nullptr`.
	 * @pre RESET pin must be configured as a GPIO output.
	 * @post ST7789V controller undergoes hardware power-on reset.
	 * @deprecated None.
	 */
	void reset(void);

protected:
	Spi *mSpi;                                       ///< Pointer to active SPI peripheral driver.
	pin_t mCsPin;                                    ///< Cached Chip Select GPIO pin descriptor.
	pin_t mDcPin;                                    ///< Cached Data/Command GPIO pin descriptor.
	pin_t mRstPin;                                   ///< Cached Reset GPIO pin descriptor.
	const Spi::specification_t *mRegSpec, *mDataSpec;///< Pointers to command (8-bit) and data (16-bit) SPI transfer profiles.

	/**
	 * @brief Caches SPI and GPIO configuration pins and drives CS high (idle).
	 * @param[in] config Hardware connection configuration structure.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning None.
	 * @note Called by derived class `initialize()`.
	 * @pre GPIO output configurations should be set.
	 * @post Internal pin and SPI references are initialized.
	 * @deprecated None.
	 */
	void setConfig(const config_t &config);

	/**
	 * @brief Transmits a single command byte over SPI with DC driven low.
	 * @param[in] cmd Command register enum (`cmd_t`).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::sendReg()`.
	 * @note Drives DC low, asserts CS low, sends byte, and deasserts CS high.
	 * @pre SPI peripheral must be locked (`enable()`).
	 * @post Command is transmitted to the controller.
	 * @deprecated None.
	 */
	void sendReg(cmd_t cmd) override;

	/**
	 * @brief Transmits a command byte with DC low, followed by an 8-bit parameter with DC high.
	 * @param[in] cmd  Command register enum (`cmd_t`).
	 * @param[in] data Single 8-bit parameter byte.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::sendReg()`.
	 * @note Drives DC low for command, switches DC high for parameter, then raises CS.
	 * @pre SPI peripheral must be locked (`enable()`).
	 * @post Register and parameter are written.
	 * @deprecated None.
	 */
	void sendReg(cmd_t cmd, uint8_t data) override;
	
	/**
	 * @brief Transmits a command byte followed by an array of 8-bit parameters over SPI.
	 * @param[in] cmd   Command register enum (`cmd_t`).
	 * @param[in] data  Pointer to the parameter byte array.
	 * @param[in] count Number of parameter bytes to transmit.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::sendReg()`.
	 * @note Streams multi-byte payload with DC driven high.
	 * @pre SPI peripheral must be locked.
	 * @post All parameter bytes are transferred.
	 * @deprecated None.
	 */
	void sendReg(cmd_t cmd, uint8_t *data, uint32_t count) override;

	/**
	 * @brief Streams 16-bit RGB565 pixel buffer over SPI with DC driven high.
	 * @param[in] data  Pointer to 16-bit pixel data array.
	 * @param[in] count Number of 16-bit words to stream.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::sendData()`. SPI must be configured for 16-bit mode.
	 * @note Asserts CS, streams 16-bit words via SPI, then raises CS.
	 * @pre Interface must be enabled with `TYPE_DATA`.
	 * @post Pixel data is transferred to display RAM.
	 * @deprecated None.
	 */
	void sendData(uint16_t *data, uint32_t count) override;

	/**
	 * @brief Locks SPI peripheral and applies 8-bit command or 16-bit data bus transfer specification.
	 * @param[in] type Transmission phase (`TYPE_REG` for 8-bit commands, `TYPE_DATA` for 16-bit pixels).
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::enable()`. Must be paired with `disable()`.
	 * @note Acquires mutex lock on SPI peripheral.
	 * @pre None.
	 * @post SPI bus is locked and configured.
	 * @deprecated None.
	 */
	void enable(type_t type) override;

	/**
	 * @brief Releases mutex lock on SPI peripheral.
	 * @param[in] none
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Overrides `ST7789V::disable()`.
	 * @note Releases mutex on SPI peripheral.
	 * @pre SPI must be locked.
	 * @post SPI bus lock is released.
	 * @deprecated None.
	 */
	void disable(void) override;

	/**
	 * @brief Reads a single byte response from the ST7789V controller over SPI.
	 * @param[in]  cmd Command register to query.
	 * @param[out] des Destination reference where received byte will be stored.
	 * @return none
	 * @author Yoon-Ki Hong
	 * @date 2015
	 * @warning Requires bidirectional SPI (MISO) or half-duplex 3-wire support.
	 * @note Not supported on standard 4-wire write-only display boards.
	 * @pre Bus must be active.
	 * @post Status or register value is stored in `des`.
	 * @deprecated None.
	 */
	void read(uint8_t cmd, uint8_t &des);
};

#endif


