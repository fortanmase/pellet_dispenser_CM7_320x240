#include "stm32h7xx.h"
#include "st7789.h"
#include "config.h"

#define ST7789_PORCTRL 0xB2U   /**< Porch Setting */
#define ST7789_GCTRL 0xB7U     /**< Gate Control */
#define ST7789_VCOMS 0xBBU     /**< VCOM Setting */
#define ST7789_LCMCTRL 0xC0U   /**< LCM Control */
#define ST7789_VDVVRHEN 0xC2U  /**< VDV and VRH Command Enable */
#define ST7789_VRHS 0xC3U      /**< VRH Set */
#define ST7789_VDVS 0xC4U      /**< VDV Set */
#define ST7789_FRCTRL2 0xC6U   /**< Frame Rate Control in Normal Mode */
#define ST7789_PWCTRL1 0xD0U   /**< Power Control 1 */
#define ST7789_PVGAMCTRL 0xE0U /**< Positive Voltage Gamma Control */
#define ST7789_NVGAMCTRL 0xE1U /**< Negative Voltage Gamma Control */

static void writeCommand(uint8_t cmd);
static void writeData(uint8_t *data, size_t size);
static void setRotation(DisplayRotationType rotation);
static void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void swapEndian(uint8_t *s, int32_t l);

static inline void csLow(void);
static inline void csHigh(void);
static inline void dcLow(void);
static inline void dcHigh(void);

// clang-format off
static const uint8_t initCmd[] =
{
    ST7789_COLMOD, 1u, ST7789_COLOR_MODE_16BIT,
    ST7789_PORCTRL, 5u, 0x0Cu, 0x0Cu, 0x00u, 0x33u, 0x33u,
    ST7789_GCTRL, 1u, 0x35u,
    ST7789_VCOMS, 1u, 0x19u,
    ST7789_LCMCTRL, 1u, 0x2Cu,
    ST7789_VDVVRHEN, 1u, 0x01u,
    ST7789_VRHS, 1u, 0x12u,
    ST7789_VDVS, 1u, 0x20u,
    ST7789_FRCTRL2, 1u, 0x0Fu,
    ST7789_PWCTRL1, 2u, 0xA4u, 0xA1u,
    ST7789_PVGAMCTRL, 14u, 0xD0u, 0x04u, 0x0Du, 0x11u, 0x13u, 0x2Bu, 0x3Fu, 0x54u, 0x4Cu, 0x18u, 0x0Du, 0x0Bu, 0x1Fu, 0x23u,
    ST7789_NVGAMCTRL, 14u, 0xD0u, 0x04u, 0x0Cu, 0x11u, 0x13u, 0x2Cu, 0x3Fu, 0x44u, 0x51u, 0x2Fu, 0x1Fu, 0x1Fu, 0x20u, 0x23u,
    ST7789_INVON, 0u,
    ST7789_SLPOUT, 0u,
    ST7789_NORON, 0u, 
    ST7789_DISPON, 0x80u, /* Display on + delay(0x80u) */
    0x00u /* End of list */
};
// clang-format on

void ST7789_Init(void)
{
    uint8_t cmd, x, numArgs;
    const uint8_t *addr = initCmd;

    while ((cmd = *addr++) > 0)
    {
        x = *addr++;
        numArgs = x & 0x7Fu;
        writeCommand(cmd);
        while (numArgs--)
        {
            csLow();
            dcHigh();
            HAL_SPI_Transmit(&ST7789_SPI_HDLR, addr++, sizeof(*addr), HAL_MAX_DELAY);
            csHigh();
        }
        if (x & 0x80u)
        {
            HAL_Delay(50u);
        }
    }

    setRotation(GUI_ROTATION);
}

void ST7789_DrawBitmap(uint8_t *bitmap, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    DWT_ResetCyccnt();
    ST7789_SPI_HDLR.Init.DataSize = SPI_DATASIZE_8BIT; /* Set data size back to 8 bit after DMA transmission*/
    HAL_SPI_Init(&ST7789_SPI_HDLR);
    csHigh();

    setAddressWindow(x, y, x + w - 1u, y + h - 1u);
    csLow();
    dcHigh();

    /* Set data size to 16 so that the endianness of the buffer is swapped,
       note that the DMA peripheral must be configured to 16 bit as well. Also,
       swapEndian() can be used instead of reconfiguring the SPI but the conversion takes much longer */
    ST7789_SPI_HDLR.Init.DataSize = SPI_DATASIZE_16BIT;
    HAL_SPI_Init(&ST7789_SPI_HDLR);

    HAL_StatusTypeDef dma_err = HAL_SPI_Transmit_DMA(&ST7789_SPI_HDLR, (uint8_t *)bitmap, w * h);
    uint32_t usElapsed = DWT_GetElapsedUs();
    usElapsed++;
}

/**
 * @brief   Function to write a command
 * @param   cmd Command
 */
static void writeCommand(uint8_t cmd)
{
    csLow();
    dcLow();
    HAL_SPI_Transmit(&ST7789_SPI_HDLR, &cmd, sizeof(cmd), HAL_MAX_DELAY);
    csHigh();
}

/**
 * @brief   Function to write data
 * @param   data Data
 * @param   size Size of data
 */
static void writeData(uint8_t *data, size_t size)
{
    csLow();
    dcHigh();

    while (size > 0)
    {
        uint16_t chunk_size = size > UINT16_MAX ? UINT16_MAX : size;

        HAL_SPI_Transmit(&ST7789_SPI_HDLR, data, chunk_size, HAL_MAX_DELAY);

        data += chunk_size;
        size -= chunk_size;
    }

    csHigh();
}

/**
 * @brief   Set the rotation direction of the display
 * @param   rotation  rotation
 */
static void setRotation(DisplayRotationType rotation)
{
    uint8_t madctl = ST7789_MADCTL_RGB;

    writeCommand(ST7789_MADCTL);
    switch (rotation)
    {
    case 0:
        madctl = ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB;
        break;
    case 1:
        madctl = ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
        break;
    case 2:
        madctl = ST7789_MADCTL_RGB;
        break;
    case 3:
        madctl = ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
        break;
    default:
        break;
    }

    writeData(&madctl, sizeof(madctl));
}

/**
 * @brief   Sets the active display window for drawing
 * @param   x0  Start X coordinate
 * @param   y0  Start Y coordinate
 * @param   x1  End X coordinate
 * @param   y1  End Y coordinate
 * @note    Configures both column and row address windows and prepares for RAM write
 */
static void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    csLow();

    writeCommand(ST7789_CASET); /* Column Address set */
    {
        uint8_t data[] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
        writeData(data, sizeof(data));
    }

    writeCommand(ST7789_RASET); /* Row Address set */
    {
        uint8_t data[] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
        writeData(data, sizeof(data));
    }

    writeCommand(ST7789_RAMWR); /* Write to RAM */
    csHigh();
}

/**
 * @brief   Swaps endianness of data in memory
 * @param   s Pointer to data buffer
 * @param   l Length of data in bytes
 * @note    Uses unrolled loop optimization for 8 elements at a time
 */
static void swapEndian(uint8_t *s, int32_t l)
{
    uint32_t *ptr = (uint32_t *)s;

    if (l & 1)
    {
        uint8_t temp = *s;
        *s = *(s + l - 1);
        *(s + l - 1) = temp;
        l -= 1;
        s++;
    }

    /* Unrolled loop for 8 elements */
    while (l >= 8)
    {
        ptr[0] = __REV16(ptr[0]);
        ptr[1] = __REV16(ptr[1]);
        ptr[2] = __REV16(ptr[2]);
        ptr[3] = __REV16(ptr[3]);
        ptr[4] = __REV16(ptr[4]);
        ptr[5] = __REV16(ptr[5]);
        ptr[6] = __REV16(ptr[6]);
        ptr[7] = __REV16(ptr[7]);
        ptr += 8;
        l -= 32; /* Decrease by 32 bytes (8 * sizeof(uint32_t)) for 8 elements */
    }

    /* Handling remaining elements if any */
    while (l > 0)
    {
        *ptr = __REV16(*ptr);
        ptr++;
        l -= 4;
    }
}

/**
 * @brief Sets CS line low
 */
static inline void csLow(void)
{
    HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Sets CS line high
 */
static inline void csHigh(void)
{
    HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief Sets DC line low
 */
static inline void dcLow(void)
{
    HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Sets DC line high
 */
static inline void dcHigh(void)
{
    HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_SET);
}
