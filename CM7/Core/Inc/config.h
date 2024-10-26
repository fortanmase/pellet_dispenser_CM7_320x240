#ifndef CONFIG_H
#define CONFIG_H

typedef enum
{
    ROTATION_0 = 0U,   /**< Normal orientation (0 degrees) */
    ROTATION_90 = 1U,  /**< Rotated 90 degrees clockwise */
    ROTATION_180 = 2U, /**< Rotated 180 degrees */
    ROTATION_270 = 3U  /**< Rotated 270 degrees clockwise */
} DisplayRotationType; /**< Display rotation configuration type */

#define GUI_WIDTH 320U           /**< Graphical user interface width in pixels */
#define GUI_HEIGHT 240U          /**< Graphical user interface height in pixels */
#define GUI_ROTATION ROTATION_90 /**< Graphical interface rotation (note: if the screen draws something wrong, then most probably you need to adjust tthis parameter) */

#endif /* CONFIG_H */
