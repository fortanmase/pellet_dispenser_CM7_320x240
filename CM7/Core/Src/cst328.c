#include "stm32h7xx_hal.h"

#include "cst328.h"
#include "config.h"

#define CST328_I2C_HDLR hi2c1 /**< CST328 I2C Handler */

#define CST328_ADDR ((0x1AU << 1U) & 0xFEU) /**< Device address (0x1A), address shall be 7-bit, thus shifted */

#define CST328_CMD_ENUM_MODE_DEBUG_INFO 0xD101U  /**< Command to enter the reading firmware information mode */
#define CST328_CMD_SYSTEM_RESET 0xD102U          /**< Command to perform a system reset */
#define CST328_CMD_ENUM_MODE_NORMAL 0xD109U      /**< Command to enter the normal reporting mode */
#define CST328_CMD_ENUM_MODE_DEBUG_WRITE 0xD10BU /**< Command to enter debug write mode */
#define CST328_CMD_ENUM_MODE_FACTORY 0xD119U     /**< Command to enter factory mode */

#define CST328_REG_XY_RESOLUTION 0xD1F8U             /**< X/Y resolution register address */
#define CST328_REG_FW_VER_CODE_AND_BOOT_TIME 0xD1FCU /**< Firmware verification code, bootloader time register address */

#define CST328_MAX_TOUCH_POINTS 5U                     /**< Maximum number of simultaneous touches */
#define CST328_TOUCH_INFO_REG_NUM 27U                  /**< Number of registers containing touch information */
#define CST328_1ST_FING_ID_AND_STATUS_REG_ADDR 0xD000U /**< 1st Finger ID and status register address */

#define CST328_NUM_OF_FING_REG_SHIFT 5U /**< Number of fingers register shift */

typedef struct
{
    uint16_t x;       /**< X coordinate */
    uint16_t y;       /**< Y coordinate */
    uint8_t pressure; /**< Pressure value */
} CST328_CoordType;   /**< CST328 coordinate type*/

typedef struct
{
    bool touchPressed;                                /**< Flag indicating touchscreen was pressed */
    uint8_t touchNum;                                 /**< Number of simultaneous touches */
    uint32_t irqCtr;                                  /**< Interrupt counter */
    CST328_CoordType coords[CST328_MAX_TOUCH_POINTS]; /**< Coordinates */
} CST328_TouchType;                                   /**< CST328 touch type */

static bool readTouchData(void);
static void setRotation(DisplayRotationType value);
static void transformCoordinates(uint16_t raw_x, uint16_t raw_y, uint16_t *transformed_x, uint16_t *transformed_y);
static HAL_StatusTypeDef writeRegister(uint16_t regAddr, uint8_t *regData, uint16_t size);
static HAL_StatusTypeDef readRegister(uint16_t regAddr, uint8_t *regData, uint16_t size);

extern I2C_HandleTypeDef CST328_I2C_HDLR;         /**< I2C handler */
static CST328_TouchType touchData;                /**< Touch data */
static DisplayRotationType rotation = ROTATION_0; /**< Display rotation */

// TODO: Find a way to repgrogram X/Y resolution register if needed
bool CST328_Init(void)
{
    uint8_t readBuffer[4];
    bool flag = false;
    bool ret = false;

    HAL_StatusTypeDef status = writeRegister(CST328_CMD_ENUM_MODE_DEBUG_INFO, NULL, 0u); /* Put the touch controller into debug info mode */
    status |= readRegister(CST328_REG_FW_VER_CODE_AND_BOOT_TIME, readBuffer, sizeof(readBuffer));
    ret = (readBuffer[3] << 8U | readBuffer[2]) == 0xCACAU; /* Check if FW version is correct */

    status |= writeRegister(CST328_CMD_ENUM_MODE_NORMAL, NULL, 0U); /* Put the touch controller into normal mode */
    setRotation(GUI_ROTATION);

    if (status != HAL_OK)
    {
        ret = false;
    }

    return ret;
}

bool CST328_ProcessCallback(void)
{
    touchData.touchPressed = readTouchData(); /* If the read data is valid, indicate a touch has occurred */
    touchData.irqCtr++;

    return touchData.touchPressed;
}

void CST328_ResetTouchPressed(void)
{
    touchData.irqCtr = 0U;
    touchData.touchPressed = false;
}

bool CST328_GetTouchPressed(void)
{
    return touchData.touchPressed;
}

uint32_t CST328_GetIrqCounter(void)
{
    return touchData.irqCtr;
}

bool CST328_GetTouchData(uint8_t numOfFinger, uint16_t *x, uint16_t *y, uint8_t *pressure)
{
    bool ret = true;

    if (x != NULL && y != NULL && pressure != NULL && numOfFinger <= touchData.touchNum)
    {
        numOfFinger--;
        *x = touchData.coords[numOfFinger].x;
        *y = touchData.coords[numOfFinger].y;
        *pressure = touchData.coords[numOfFinger].pressure;
    }
    else
    {
        ret = false;
    }

    return ret;
}

/**
 * @brief   Function to read touchscreen data
 * @return  True - Data read successfully/ False - Otherwise
 */
static bool readTouchData(void)
{
    bool ret = true;
    uint8_t data[CST328_TOUCH_INFO_REG_NUM];

    HAL_StatusTypeDef status = readRegister(CST328_1ST_FING_ID_AND_STATUS_REG_ADDR, data, CST328_TOUCH_INFO_REG_NUM); /* Read all touch information registers */

    if (status == HAL_OK)
    {
        touchData.touchNum = data[CST328_NUM_OF_FING_REG_SHIFT]; /* Extract the number of touches */

        for (uint8_t i = 0U; i < touchData.touchNum; i++)
        {
            uint8_t regOffset = 0U;
            if (i > 0U)
            {
                regOffset = 2U; /* An offset of 2 to skip registers 0xD005 and 0xD006 when there are multiple touches */
            }

            uint16_t raw_x = (data[(i * 5U) + 1U + regOffset] << 4) + ((data[(i * 5U) + 3U + regOffset] & 0xF0U) >> 4U);
            uint16_t raw_y = (data[(i * 5U) + 2U + regOffset] << 4) + (data[(i * 5U) + 3U + regOffset] & 0x0FU);

            transformCoordinates(raw_x, raw_y, &touchData.coords[i].x, &touchData.coords[i].y);
            touchData.coords[i].pressure = data[(i * 5U) + 4U + regOffset];
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

/**
 * @brief   Function to write a register
 * @note    Pass NULL pointer to regData in order to issue a command
 * @param   regAddr Register Address
 * @param   regData Pointer to the data to be written
 * @param   size    Size of the data in bytes
 * @return  Status of transmission
 */
static HAL_StatusTypeDef writeRegister(uint16_t regAddr, uint8_t *regData, uint16_t size)
{
    uint8_t u8RegAddr[2] = {(uint8_t)(regAddr >> 8U), (uint8_t)regAddr};

    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&CST328_I2C_HDLR, CST328_ADDR, u8RegAddr, 2U, HAL_MAX_DELAY); /* Transmit register address */

    if (ret == HAL_OK && regData != NULL && size > 0U)
    {
        ret = HAL_I2C_Master_Transmit(&CST328_I2C_HDLR, CST328_ADDR, regData, size, HAL_MAX_DELAY); /* Transmit data */
    }

    return ret;
}

/**
 * @brief   Function to write a register
 * @param   regAddr Register Address
 * @param   regData Pointer to the data read from touchscreen
 * @param   size    Size of the data in bytes
 * @return  Status of transmission
 */
static HAL_StatusTypeDef readRegister(uint16_t regAddr, uint8_t *regData, uint16_t size)
{
    uint8_t u8RegAddr[2] = {(uint8_t)(regAddr >> 8U), (uint8_t)regAddr};

    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(&CST328_I2C_HDLR, CST328_ADDR, u8RegAddr, 2U, HAL_MAX_DELAY); /* Transmit register address */

    if (ret == HAL_OK)
    {
        ret = HAL_I2C_Master_Receive(&CST328_I2C_HDLR, CST328_ADDR, regData, size, HAL_MAX_DELAY); /* Receive data */
    }

    return ret;
}

/**
 * @brief   Sets the display rotation for coordinate transformation
 * @param   value   Display rotation value (0, 90, 180 or 270 degrees)
 */
static void setRotation(DisplayRotationType value)
{
    rotation = value;
}

/**
 * @brief   Transforms raw touch coordinates based on current display rotation
 * @param   raw_x           Raw X coordinate from touch controller
 * @param   raw_y           Raw Y coordinate from touch controller
 * @param   transformed_x   Pointer to store transformed X coordinate
 * @param   transformed_y   Pointer to store transformed Y coordinate
 */
static void transformCoordinates(uint16_t raw_x, uint16_t raw_y, uint16_t *transformed_x, uint16_t *transformed_y)
{
    switch (rotation)
    {
    case ROTATION_0:
        *transformed_x = raw_x;
        *transformed_y = raw_y;
        break;

    case ROTATION_90:
        *transformed_x = GUI_WIDTH - raw_y;
        *transformed_y = raw_x;
        break;

    case ROTATION_180:
        *transformed_x = raw_y;
        *transformed_y = GUI_HEIGHT - raw_x;
        break;
    case ROTATION_270:
        *transformed_x = GUI_HEIGHT - raw_x;
        *transformed_y = GUI_WIDTH - raw_y;
        break;
    }
}