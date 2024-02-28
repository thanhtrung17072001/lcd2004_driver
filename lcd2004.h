#ifndef LCD2004_H
#define LCD2004_H

#define LCD_OK  0
#define LCD_ERR 1

#define LCD2004_ADDR 0x27u

#define PRINT_ERROR(...) {pr_info("[%s:%d]: ",__func__, __LINE__); pr_info(__VA_ARGS__);}

typedef enum {
    CLEAR_DISPLAY           = 0x01,
    RETURN_HOME             = 0x02,
    CURSOR_ON               = 0x0F,
    CURSOR_OFF              = 0x0C,
    LCD_4BIT_M0DE_5x11_DOTS = 0x2C,
    LCD_4BIT_MODE_5x8_DOTS  = 0x28,
    LCD_4BIT                = 0x20,
    DISPLAY_OFF             = 0x08,
    DISPLAY_ON              = 0X0C,
    CURSOR_INCREMENT        = 0x06
}e_lcd_ctrl_cmd_t;

#endif