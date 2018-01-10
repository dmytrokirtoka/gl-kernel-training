
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define CMD_ADDR			0

#define SET_LOW_COLUMN			0x00
#define SET_HIGH_COLUMN			0x10
#define COLUMN_ADDR			0x21
#define PAGE_ADDR			0x22
#define SET_START_PAGE			0xB0
#define CHARGE_PUMP			0x8D
#define DISPLAY_OFF			0xAE
#define DISPLAY_ON			0xAF

#define MEMORY_MODE			0x20
#define SET_CONTRAST			0x81
#define SET_NORMAL_DISPLAY		0xA6
#define SET_INVERT_DISPLAY		0xA7
#define COM_SCAN_INC			0xC0
#define COM_SCAN_DEC			0xC8
#define SET_DISPLAY_OFFSET		0xD3
#define SET_DISPLAY_CLOCK_DIV		0xD5
#define SET_PRECHARGE			0xD9
#define SET_COM_PINS			0xDA
#define SET_VCOM_DETECT			0xDB
#define SET_START_LINE			0x40

#define CHECK_STATUS_REG_VAL		0x43

#define MYDEVNAME    "i2c_ssd1306"

static const struct i2c_device_id i2c_ssd1306_idtable[] = {
	{ MYDEVNAME, 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, i2c_ssd1306_idtable);

static int i2c_ssd1306_init(struct i2c_client *drv_client)
{
	s32 val;

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, DISPLAY_OFF);
	val = i2c_smbus_read_byte_data(drv_client, CMD_ADDR);

	dev_info(&drv_client->dev, "%s: status reg: %d\n", __func__, val);
	if (val != CHECK_STATUS_REG_VAL)
		return -1;

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, MEMORY_MODE);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0);
	/* Set column start / end */
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, COLUMN_ADDR);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, (LCD_WIDTH - 1));

	/* Set page start / end */
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, PAGE_ADDR);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, (LCD_HEIGHT / 8 - 1));

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, COM_SCAN_DEC);

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_CONTRAST);
	/* Max contrast */
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0xFF);

	/* set segment re-map 0 to 127 */
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0xA1);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_NORMAL_DISPLAY);
	/* set multiplex ratio(1 to 64) */
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0xA8);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, (LCD_HEIGHT - 1));
	/* 0xA4 => follows RAM content */
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0xA4);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_DISPLAY_OFFSET);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0x00);

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_DISPLAY_CLOCK_DIV);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0x80);

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_PRECHARGE);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0x22);

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_COM_PINS);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0x12);

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, SET_VCOM_DETECT);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0x20);


	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, CHARGE_PUMP);
	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, 0x14);

	i2c_smbus_write_byte_data(drv_client, CMD_ADDR, DISPLAY_ON);

	return 0;
}

static int i2c_ssd1306_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	dev_info(&drv_client->dev, "init I2C driver client address %d\n",
			drv_client->addr);

	if (i2c_ssd1306_init(drv_client)) {
		dev_info(&drv_client->dev, "I2C device not found\n");
		return -1;
	}

	return 0;
}

static int i2c_ssd1306_remove(struct i2c_client *drv_client)
{
	struct my_device *md = i2c_get_clientdata(drv_client);

	kthread_stop(md->th);
	unregister_framebuffer(md->fb_info);

	dev_info(&drv_client->dev, "ssd1306 driver successfully removed\n");
	return 0;
}

static struct i2c_driver i2c_ssd1306_driver = {
	.driver = {
		.name = MYDEVNAME,
	},

	.probe = i2c_ssd1306_probe,
	.remove = i2c_ssd1306_remove,
	.id_table = i2c_ssd1306_idtable,
};

module_i2c_driver(i2c_ssd1306_driver);

MODULE_AUTHOR("Dmytro Kirtoka <dimk334@gmail.com>");
MODULE_DESCRIPTION("tft lcd b&w screen driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
