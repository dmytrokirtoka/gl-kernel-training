
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define MYDEVNAME    "i2c_ssd1306"

static const struct i2c_device_id i2c_ssd1306_idtable[] = {
	{ MYDEVNAME, 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, i2c_ssd1306_idtable);

static int i2c_ssd1306_probe(struct i2c_client *drv_client,
			 const struct i2c_device_id *id)
{
	dev_info(&drv_client->dev, "init I2C driver client address %d\n",
			drv_client->addr);
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
