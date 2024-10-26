#ifndef CST328_H
#define CST328_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

    /**
     * @brief   Function to initialize touchscreen controller
     * @return  True - Touchscreen successfully initialized/ False - Otherwise
     */
    bool CST328_Init(void);

    /**
     * @brief   Callback function to process touchscreen data
     * @return  True - Data processed successfully/ False - Otherwise
     */
    bool CST328_ProcessCallback(void);

    /**
     * @brief   Function to reset the touch pressed flag after processing the data(also reset interrupt counter)
     */
    void CST328_ResetTouchPressed(void);

    /**
     * @brief   Function to get the touch pressed flag
     */
    bool CST328_GetTouchPressed(void);

    /**
     * @brief   Gets interrupt counter
     * @return  number of interrupts occurred
     */
    uint32_t CST328_GetIrqCounter(void);

    /**
     * @brief   Function to get the data from the touchscreen
     * @param   numOfFingerNumber - Number of finger to get the data from
     * @param   x - Pointer to X coordinate
     * @param   y - Pointer to Y coordinate
     * @param   pressure - Pointer to pressure value
     * @return  True - Data valid/False - Otherwise
     */
    bool CST328_GetTouchData(uint8_t numOfFinger, uint16_t *x, uint16_t *y, uint8_t *pressure);

#ifdef __cplusplus
}
#endif

#endif /* CST328_H */
