#ifndef ST7789_H
#define ST7789_H

#include "stm32h7xx_hal.h"
#include "main.h"

#define ST7789_SPI_HDLR hspi1 /**< SPI handler for ST7789 display */
extern SPI_HandleTypeDef ST7789_SPI_HDLR;

#define ST7789_CS_PORT TFT_CS_GPIO_Port /**< Chip-Select Port */
#define ST7789_CS_PIN TFT_CS_Pin        /**< Chip-Select Pin */
#define ST7789_DC_PORT TFT_DC_GPIO_Port /**< Data/Command Port */
#define ST7789_DC_PIN TFT_DC_Pin        /**< Data/Command Pin */

#define X_SHIFT 0 /**< X-axis offset for display positioning */
#define Y_SHIFT 0 /**< Y-axis offset for display positioning */

/* Control Registers and constant codes */
#define ST7789_NOP 0x00     /**< No operation command */
#define ST7789_SWRESET 0x01 /**< Software reset command */
#define ST7789_RDDID 0x04   /**< Read display ID command */
#define ST7789_RDDST 0x09   /**< Read display status command */

#define ST7789_SLPIN 0x10  /**< Sleep in command */
#define ST7789_SLPOUT 0x11 /**< Sleep out command */
#define ST7789_PTLON 0x12  /**< Partial mode on command */
#define ST7789_NORON 0x13  /**< Normal display mode on command */

#define ST7789_INVOFF 0x20  /**< Display inversion off command */
#define ST7789_INVON 0x21   /**< Display inversion on command */
#define ST7789_DISPOFF 0x28 /**< Display off command */
#define ST7789_DISPON 0x29  /**< Display on command */
#define ST7789_CASET 0x2A   /**< Column address set command */
#define ST7789_RASET 0x2B   /**< Row address set command */
#define ST7789_RAMWR 0x2C   /**< Memory write command */
#define ST7789_RAMRD 0x2E   /**< Memory read command */

#define ST7789_PTLAR 0x30  /**< Partial area command */
#define ST7789_COLMOD 0x3A /**< Interface pixel format command */
#define ST7789_MADCTL 0x36 /**< Memory data access control command */

#define ST7789_MADCTL_MY 0x80  /**< Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MX 0x40  /**< Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MV 0x20  /**< Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_ML 0x10  /**< Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_RGB 0x01 /**< RGB/BGR Order ('0' = RGB, '1' = BGR) */

#define ST7789_RDID1 0xDA /**< Read ID1 command - returns LCD module's manufacturer ID */
#define ST7789_RDID2 0xDB /**< Read ID2 command - returns LCD module's version number */
#define ST7789_RDID3 0xDC /**< Read ID3 command - returns LCD module's driver version */
#define ST7789_RDID4 0xDD /**< Read ID4 command - returns LCD module's ID information */

#define ST7789_COLOR_MODE_16BIT 0x55 /**<  RGB565 (16bit) color mode */
#define ST7789_COLOR_MODE_18bit 0x66 /**<  RGB666 (18bit) color mode */

#define ST7789_BLACK 0x0000U       /**< Basic black color (RGB: 0, 0, 0) */
#define ST7789_NAVY 0x000FU        /**< Dark blue color (RGB: 0, 0, 123) */
#define ST7789_DARKGREEN 0x03E0U   /**< Dark green color (RGB: 0, 125, 0) */
#define ST7789_DARKCYAN 0x03EFU    /**< Dark cyan - mix of blue and green (RGB: 0, 125, 123) */
#define ST7789_MAROON 0x7800U      /**< Dark red color (RGB: 123, 0, 0) */
#define ST7789_PURPLE 0x780FU      /**< Purple - mix of red and blue (RGB: 123, 0, 123) */
#define ST7789_OLIVE 0x7BE0U       /**< Olive - dark yellow-green (RGB: 123, 125, 0) */
#define ST7789_LIGHTGREY 0xC618U   /**< Light grey color (RGB: 198, 195, 198) */
#define ST7789_DARKGREY 0x7BEFU    /**< Dark grey color (RGB: 123, 125, 123) */
#define ST7789_BLUE 0x001FU        /**< Pure blue color (RGB: 0, 0, 255) */
#define ST7789_GREEN 0x07E0U       /**< Pure green color (RGB: 0, 255, 0) */
#define ST7789_CYAN 0x07FFU        /**< Cyan - bright blue-green (RGB: 0, 255, 255) */
#define ST7789_RED 0xF800U         /**< Pure red color (RGB: 255, 0, 0) */
#define ST7789_MAGENTA 0xF81FU     /**< Magenta - bright purple (RGB: 255, 0, 255) */
#define ST7789_YELLOW 0xFFE0U      /**< Pure yellow color (RGB: 255, 255, 0) */
#define ST7789_WHITE 0xFFFFU       /**< Pure white color (RGB: 255, 255, 255) */
#define ST7789_ORANGE 0xFD20U      /**< Orange - mix of red and yellow (RGB: 255, 165, 0) */
#define ST7789_GREENYELLOW 0xAFE5U /**< Yellow-green mix (RGB: 173, 255, 41) */
#define ST7789_PINK 0xFC18U        /**< Pink - light red (RGB: 255, 130, 198) */
#define ST7789_LIGHTGREEN 0X841F   /**< Light green color in RGB565 format */
#define ST7789_LGRAY 0XC618        /**< Alternative light gray in RGB565 format */
#define ST7789_LGRAYBLUE 0XA651    /**< Light gray-blue mix in RGB565 format */
#define ST7789_LBBLUE 0X2B12       /**< Light bright blue in RGB565 format */

/**
 * @brief Initialize the ST7789 display
 */
void ST7789_Init(void);

/**
 * @brief   Draw bitmap on the display
 * @param   bitmap  Pointer to bitmap data
 * @param   x       X coordinate
 * @param   y       Y coordinate
 * @param   w       Width of bitmap
 * @param   h       Height of bitmap
 */
void ST7789_DrawBitmap(uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif /* ST7789_H */