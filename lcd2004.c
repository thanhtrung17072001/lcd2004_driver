#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/delay.h>
#include "lcd2004.h"
const uint8_t cursor_position_table[4][20] = 
{   
    {0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u, 0x09u, 0x0Au, 0x0Bu, 0x0Cu, 0x0Du, 0x0Eu, 0x0Fu, 0x10u, 0x11u, 0x12u, 0x13u}, /* Row 1 */
    {0x40u, 0x41u, 0x42u, 0x43u, 0x44u, 0x45u, 0x46u, 0x47u, 0x48u, 0x49u, 0x4Au, 0x4Bu, 0x4Cu, 0x4Du, 0x4Eu, 0x4Fu, 0x50u, 0x51u, 0x52u, 0x53u}, /* Row 2 */
    {0x14u, 0x15u, 0x16u, 0x17u, 0x18u, 0x19u, 0x1Au, 0x1Bu, 0x1Cu, 0x1Du, 0x1Eu, 0x1Fu, 0x20u, 0x21u, 0x22u, 0x23u, 0x24u, 0x25u, 0x26u, 0x27u}, /* Row 3 */
    {0x54u, 0x55u, 0x56u, 0x57u, 0x58u, 0x59u, 0x5Au, 0x5Bu, 0x5Cu, 0x5Du, 0x5Eu, 0x5Fu, 0x60u, 0x61u, 0x62u, 0x63u, 0x64u, 0x65u, 0x66u, 0x67u}  /* Row 4 */
};

struct of_device_id lcd2004_dt_match[] =
{
    {
        .compatible = "ht,lcdi2c",
    },
    {}
};


int lcd2004_read(struct i2c_client *client, uint8_t *data)
{
    int ret = LCD_OK;

    if (i2c_master_recv(client, data, 1) < 0)
        ret = LCD_ERR;

    return ret;
}

int lcd2004_write(struct i2c_client *client, uint8_t data)
{
    int ret = LCD_OK;

    if(i2c_master_send(client, &data, 1) < 0)
        ret = LCD_ERR;

    return ret; 
}

int lcd2004_write_buff(struct i2c_client *client, uint8_t *buff, int size)
{
    int ret = LCD_OK; 
    int i;
    
    for(i = 0; i < size; i++)
    {
        ret = lcd2004_write(client, *(buff+i));
        if(ret != LCD_OK)
        {
            break;
        }
        mdelay(1);
    }

    return ret;
}

int lcd2004_send_ctrl_cmd(struct i2c_client *client, uint8_t cmd)
{
    uint8_t data[4] = {0};
    bool rs = 0; /* For sending control command */
    bool rw = 0; /* For write */
    bool en;
    int ret;

    /* Buffer for MSB of command with enable pin = 1 */
    en = 1;
    data[0] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | (cmd & 0xF0u);

    /* Buffer for MSB of command with enable pin = 0 */
    en = 0;
    data[1] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | (cmd & 0xF0u);

    /* Buffer for LSB of command with enable pin = 1 */
    en = 1;
    data[2] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | ((cmd & 0x0Fu) << 4);

    /* Buffer for LSB of command with enable pin = 0 */
    en = 0;
    data[3] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | ((cmd & 0x0Fu) << 4);

    /* Start  to LCD */
    ret = lcd2004_write_buff(client, data, 4);

    if(cmd == CLEAR_DISPLAY || cmd == RETURN_HOME)
    {
        mdelay(2);
    }
    
    return ret;
}

int lcd2004_set_cursor(struct i2c_client *client, uint8_t row, uint8_t col)
{
    int ret = LCD_OK;
    uint8_t cursor_pos;
    if(col >= 20 || row >= 4)
    {
        ret = LCD_ERR;
    }

    if(LCD_OK == ret)
    {
        cursor_pos = (1 << 7) | cursor_position_table[row][col];
        ret = lcd2004_send_ctrl_cmd(client, cursor_pos);
    }

    return ret;
}

int lcd2004_send_character(struct i2c_client *client, uint8_t character)
{
    int ret;
    bool en;
    bool rs = 1; /* For sending data */
    bool rw = 0;
    uint8_t data[4];

    en = 1; 
    data[0] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | (character & 0xF0u);

    en = 0;
    data[1] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | (character & 0xF0u);

    en = 1; 
    data[2] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | ((character & 0x0Fu) << 4);

    en = 0;
    data[3] = (rs << 0) | (rw << 1) | (en << 2) | (1 << 3) | ((character & 0x0Fu) << 4);

    ret = lcd2004_write_buff(client, data, sizeof(data));

    return ret;
}

int lcd2004_send_string(struct i2c_client *client, uint8_t *str, int size)
{
    int ret; 
    int i;

    for(i = 0; i < size; i++)
    {
        ret = lcd2004_send_character(client, *(str+i));
        if(ret != LCD_OK)
        {
            break;
        }
    }

    return ret;
} 

void lcd2004_initialization(struct i2c_client *client)
{
    int ret; 
    struct device *dev = &client->dev;

    mdelay(50);
    lcd2004_send_ctrl_cmd(client, 0x03);
    mdelay(5);
    lcd2004_send_ctrl_cmd(client, 0x03);
    mdelay(1);
    lcd2004_send_ctrl_cmd(client, 0x03);
    mdelay(1);
    lcd2004_send_ctrl_cmd(client, 0x02);

    ret = lcd2004_send_ctrl_cmd(client, LCD_4BIT_MODE_5x8_DOTS);
    if(ret)
        dev_info(dev,"Setup LCD 4bit mode 5x8 failed\n");

    ret = lcd2004_send_ctrl_cmd(client, CURSOR_ON);
    if(ret)
        dev_info(dev,"Cursor off failed\n");

    ret = lcd2004_send_ctrl_cmd(client, CLEAR_DISPLAY);
    if(ret)
        dev_info(dev, "Clear display failed\n");
    
}

int lcd2004_print_string(struct i2c_client *client, char *str)
{
    int ret; 
     
    ret = lcd2004_send_string(client, str, strlen(str));

    return ret;
}

int lcd2004_probe(struct i2c_client *client)
{
    struct device *dev = &client->dev;
    struct lcd2004_data *p_lcd_data = NULL;
    uint8_t row, col;
    int ret;

    dev_info(dev,"New lcd2004 device is detected\n");

    pr_info("I2C Address of LCD = 0x%x\n", client->addr);

    lcd2004_initialization(client);

    ret = lcd2004_set_cursor(client, 0, 0);
    if(ret)
        dev_info(dev, "Set cursor failed\n");

    (void)lcd2004_print_string(client, "Phan Thanh Trung");
    

    return 0;
}

int lcd2004_remove(struct i2c_client *client)
{
    pr_info("lcd2004 device is removed\n");
    return 0;
}

struct i2c_driver lcd2004_driver =
    {
        .driver =
            {
                .name = "lcd2004",
                .owner = THIS_MODULE,
                .of_match_table = lcd2004_dt_match,
            },
        .probe_new = lcd2004_probe,
        .remove = lcd2004_remove,
};

module_i2c_driver(lcd2004_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("trungthanhphan");
MODULE_DESCRIPTION("LCD 20x4 Driver");