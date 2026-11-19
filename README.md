# ST7789V

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Doxygen Build](https://github.com/mymy49/ST7789V/actions/workflows/doxygen.yml/badge.svg)](https://github.com/mymy49/ST7789V/actions)
[![Doxygen API Docs](https://img.shields.io/badge/docs-API%20Reference-brightgreen.svg)](https://mymy49.github.io/ST7789V/)

Hardware controller driver and integrated 2D graphics drawing engine for the **Sitronix ST7789V** 240x320 single-chip TFT LCD controller, built for the **yss OS** embedded framework.

---

## 1. Overview & Key Features

The **ST7789V** module provides comprehensive hardware control and high-performance graphics primitives for TFT LCD panels driven by the Sitronix ST7789V controller. Designed for resource-constrained microcontrollers, it couples hardware register abstraction with direct 2D vector drawing via the [TurtleShip](https://github.com/mymy49/TurtleShip) graphics engine.

### Key Highlights
- **Hardware Display Control (`ST7789V`)**:
  - Inherits from `TftLcdDriver` (`TurtleShip/TftLcdDriver.h`) in the TurtleShip submodule.
  - Complete control over ST7789V register sequences, power modes, sleep states, and display on/off.
  - Dynamic hardware windowing (`setWindow()`) and hardware screen rotation/mirroring (`setDirection()`).
- **RAM-Efficient 2D Graphics Engine (`ST7789V_with_Brush_RGB565`)**:
  - Bridges the ST7789V hardware controller with `BrushTftLcdRgb565LE` (`TurtleShip/BrushTftLcdRgb565LE.h`).
  - Executes vector rendering primitives (`drawDot()`, `fillRectBase()`, `fillDotArray()`, `drawBitmapBase()`) directly to the LCD controller memory.
  - Eliminates the need for a full-screen frame buffer in MCU RAM, saving over 150 KB of internal SRAM.
  - Supports streaming partial off-screen working framebuffers (`setFrameBuffer()`).
- **4-Wire High-Speed SPI Specialization (`ST7789V_spi_with_Brush_RGB565`)**:
  - Hardware pin abstraction for Chip Select (CS), Data/Command (DC), and Reset lines.
  - Thread-safe SPI transmission with bus mutex locking and high-frequency communication support (up to 15 MHz).
- **Submodule Architecture & Cross-Referencing**:
  - Includes `TurtleShip` as a standalone git submodule.
  - Fully integrated Doxygen cross-referencing via tag files (`TurtleShip.tag`), linking API documentation between driver and GUI engine.

---

## 2. System Specifications & Hardware Environment

### Supported Targets & Tools
- **Target Microcontrollers & Development Boards**:
  - Any 32-bit MCU supported by the **yss OS** framework.
- **Display Shields & Modules**:
  - Displays driven by the Sitronix ST7789V controller (e.g., SPI or parallel TFT LCD panels).
- **Required Software & Toolchains**:
  - Toolchain: CrossWorks for ARM (v5.x or higher).
  - Operating System: [yss OS](https://github.com/mymy49/yss) (embedded RTOS & HAL).
  - Graphics Engine: [TurtleShip](https://github.com/mymy49/TurtleShip) (included as submodule).
  - Language: C++11 or higher.

### Pin Map & Hardware Connections (Example)
The table below illustrates an example hardware pin configuration connecting an SPI interface to the ST7789V controller:

| Signal Name | ST7789V Pin | MCU Function | Description |
| :--- | :--- | :--- | :--- |
| **MOSI** | SDI / MOSI | SPI Master MOSI | SPI Master Output / Slave Input |
| **MISO** | SDO / MISO | SPI Master MISO | SPI Master Input / Slave Output (Optional) |
| **CLK** | SCK / CLK | SPI Serial Clock | SPI Serial Clock |
| **CS** | /CS (Active Low) | GPIO Output | LCD Controller Chip Select |
| **BL** | Backlight | GPIO Output | Display Backlight Control (High: ON) |
| **DC** | D/C (RS) | GPIO Output | Data/Command Mode (Low: Command, High: Data) |
| **RESET** | /RESET | GPIO / Unused | Hardware Reset Pin |

---

## 3. Installation & Project Setup

### Adding ST7789V as a Submodule
To add this repository as a submodule into an existing project:
```bash
git submodule add --recursive https://github.com/mymy49/ST7789V.git module/ST7789V
```

To clone an existing project with submodules recursively:
```bash
git clone --recursive <repository-url>
```

### Build and Integration
1. Add the ST7789V driver files and the `TurtleShip` submodule directory to your project include and build paths.
2. Initialize **yss OS** and configure the communication peripheral (SPI, 8080 parallel, etc.) in your board support layer.
3. Build the application using CrossWorks for ARM.
4. Download the firmware to the target MCU using your debug probe.

---

## 4. Quick Start & Example Code

Below is a complete real-world initialization and drawing example demonstrating how to configure the SPI0 peripheral, initialize the display driver, and draw 2D vector shapes under yss OS:

### 1. Hardware Initialization (`bsp.cpp`)
```cpp
#include <bsp.h>
#include <yss/instance.h>
#include <module/2.8inch_TFT_Touch_Shield_Rev2.1/Touch_LCD_Shield_for_Arduino_2_8_inch.h>
#include <TurtleShip.h>

// Global LCD display instance (inherits ST7789V_spi_with_Brush_RGB565)
Touch_LCD_Shield_for_Arduino_2_8_inch lcd;
FrameBufferRgb565LE brush;

void initializeBoard(void)
{
    // 1. Configure SPI0 GPIO alternate functions
    gpioA.setAsAltFunc(0, Gpio::PA0_SPI0_MOSI);
    gpioA.setAsAltFunc(1, Gpio::PA1_SPI0_MISO);
    gpioA.setAsAltFunc(2, Gpio::PA2_SPI0_CLK);

    // 2. Initialize SPI0 peripheral
    Spi::config_t spi0Config = 
    {
        Spi::MODE_MAIN
    };

    spi0.enableClock();
    spi0.initialize(spi0Config);
    spi0.enableInterrupt();

    // 3. Configure GPIO control pins (CS, Backlight, DC)
    gpioA.setAsOutput(3); // CS
    gpioA.setAsOutput(4); // BL
    gpioA.setAsOutput(6); // DC

    // Turn on backlight
    gpioA.setOutput(4, true);

    // 4. Initialize ST7789V LCD Controller
    Touch_LCD_Shield_for_Arduino_2_8_inch::config_t lcdConfig =
    {
        spi0,           // Spi &spi
        {&gpioA, 3},    // pin_t chipSelect
        {&gpioA, 6},    // pin_t dataCommand
        {0, 0}          // pin_t reset (unused)
    };

    // 5. Allocate working off-screen brush buffer and initialize panel
    brush.malloc(5000);
    lcd.initialize(lcdConfig);
    lcd.setFrameBuffer(brush);
    lcd.setBackgroundColor({0x00, 0x00, 0x00});
    lcd.clear();
}
```

### 2. Direct 2D Drawing onto the Display Surface
Because `ST7789V_with_Brush_RGB565` inherits from `BrushTftLcdRgb565LE`, drawing operations can be called directly on the `lcd` instance:
```cpp
void drawGraphicsDemo(void)
{
    // Acquire bus mutex
    lcd.lock();

    // 1. Clear display with dark blue background
    lcd.setBackgroundColor({0x00, 0x00, 0x20});
    lcd.clear();

    // 2. Draw direct vector shapes (no full-screen RAM buffer required)
    lcd.setBrushColor({0xFF, 0x00, 0x00}); // Red line
    lcd.drawLine({10, 10}, {230, 10});

    lcd.setBrushColor({0x00, 0xFF, 0x00}); // Green circle
    lcd.drawCircle({120, 160}, 50);

    lcd.setBrushColor({0x00, 0x80, 0xFF}); // Blue filled rectangle
    lcd.fillRectangular({40, 240, 160, 40});

    // Release bus mutex
    lcd.unlock();
}
```

---

## 5. Documentation & References

- **Doxygen API Reference**: [https://mymy49.github.io/ST7789V/](https://mymy49.github.io/ST7789V/)
- **Submodule Cross-Reference (TurtleShip)**: [https://mymy49.github.io/TurtleShip/](https://mymy49.github.io/TurtleShip/)
- **ST7789V Datasheet**: [Sitronix ST7789V Datasheet](https://www.sitronix.com.tw/)
- **Doxygen Main Page**: This `README.md` file serves as the main page of the generated Doxygen documentation (`USE_MDFILE_AS_MAINPAGE = README.md`).

---

## 6. License & Author

- **Author**: Yoon-Ki Hong
- **License**: MIT License. See [LICENSE](LICENSE) for full terms and conditions.
