#ifndef LCD2004_H
#define LCD2004_H

#define LCD_OK  0
#define LCD_ERR 1

#define LCD2004_ADDR 0x27u

#define PRINT_ERROR(...) {pr_info("[%s:%d]: ",__func__, __LINE__); pr_info(__VA_ARGS__);}

typedef enum {
    CLEAR_DISPLAY           = 0x01u,
    RETURN_HOME             = 0x02u,
    CURSOR_ON               = 0x0Fu,
    CURSOR_OFF              = 0x0Cu,
    LCD_4BIT_M0DE_5x11_DOTS = 0x2Cu,
}e_lcd_ctrl_cmd_t;

#endif