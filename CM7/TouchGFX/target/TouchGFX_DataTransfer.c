// #include "ili9341.h"
#include "st7789.h"
#include "TouchGFX_DataTransfer.h"

extern void DisplayDriver_TransferCompleteCallback(void);

uint8_t isTransmittingData = 0;

uint32_t touchgfxDisplayDriverTransmitActive(void)
{
    return isTransmittingData;
}

void touchgfxDisplayDriverTransmitBlock(uint8_t *pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    isTransmittingData = 1;
    ST7789_DrawBitmap(pixels, x, y, w, h);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        isTransmittingData = 0;
        DisplayDriver_TransferCompleteCallback();
    }
}
