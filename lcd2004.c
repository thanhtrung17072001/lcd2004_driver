#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include "lcd2004.h"

struct lcd2004_data
{
    uint8_t row;      /* Row of cursor */
    uint8_t col;      /* Column of cursor */
};

struct of_device_id lcd2004_dt_match[] =
{
    {
        .compatible = "ht,lcdi2c",
    },
    {}
};

int lcd2004_write(struct i2c_client *client, uint8_t data)
{
    int ret = LCD_OK;

    if (i2c_master_send(client, (char *)&data, 1) < 0)
        ret = LCD_ERR;

    return ret;
}

int lcd2004_read(struct i2c_client *client, uint8_t data)
{
    int ret = LCD_OK;

    if (i2c_master_recv(client, (char *)&data, 1) < 0)
        ret = LCD_ERR;

    return ret;
}

int lcd2004_write_buff(struct i2c_client *client, uint8_t *buff, int size)
{
    int ret = LCD_OK;
    int i;
    for (i = 0; i < size; i++)
    {
        ret = lcd2004_write(client, *(buff + i));
        if (ret)
            PRINT_ERROR("Write to lcd failed\n");
        break;
    }
    return ret;
}

int lcd2004_send_ctrl_cmd(struct i2c_client *client, e_lcd_ctrl_cmd_t cmd)
{
    uint8_t data[4] = {0};
    bool rs = 0; /* For sending control command */
    bool rw = 0; /* For write */
    bool en;
    int ret;

    /* Buffer for MSB of command with enable pin = 1 */
    en = 1;
    data[0] = (rs << 0) | (rw << 1) | (en << 2) | (cmd & 0xF0);

    /* Buffer for MSB of command with enable pin = 0 */
    en = 0;
    data[1] = (rs << 0) | (rw << 1) | (en << 2) | (cmd & 0xF0);

    /* Buffer for LSB of command with enable pin = 1 */
    en = 1;
    data[2] = (rs << 0) | (rw << 1) | (en << 2) | ((cmd & 0x0F) << 4);

    /* Buffer for LSB of command with enable pin = 0 */
    en = 0;
    data[3] = (rs << 0) | (rw << 1) | (en << 2) | ((cmd & 0x0F) << 4);

    /* Start  to LCD */
    ret = lcd2004_write_buff(client, data, 4);
    
    return ret;
}

#if 0
int lcd2004_send_data(char *data, int size)
{
     
}
#endif

int lcd2004_probe(struct i2c_client *client)
{
    struct device *dev = &client->dev;
    struct lcd2004_data *p_lcd_data = NULL;
    int ret;
    
    dev_info(dev,"New lcd2004 device is detected\n");

    /* Initialize data for lcd2004 */
    p_lcd_data = devm_kzalloc(dev, sizeof(struct lcd2004_data), GFP_KERNEL);
    if (!p_lcd_data)
    {
        dev_info(dev, "Cannot allocate memory for lcd2004's data");
        return ERR_PTR(-ENOMEM);
    }

    p_lcd_data->col = 0;
    p_lcd_data->row = 0;
    i2c_set_clientdata(client, p_lcd_data);

    ret = lcd2004_send_ctrl_cmd(client, LCD_4BIT_M0DE_5x11_DOTS);
    if(ret)
        dev_info(dev,"Setup LCD 4bit mode failed\n");
    ret = lcd2004_send_ctrl_cmd(client, CURSOR_ON);
    if(ret)
        dev_info(dev,"Cursor on failed\n");
 
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