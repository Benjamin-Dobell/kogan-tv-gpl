/*
 *  A mstar dummy rtc driver
 *  ToDo : Implement the real rtc...
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>

static int mstar_get_time(struct device *dev, struct rtc_time *tm)
{
	struct timeval time;
	do_gettimeofday(&time);

    // if the time before jan 1 2007, for it to jan 1 2007 to avoid android reboot.
    if(time.tv_sec < 1167652800)
        time.tv_sec = 1167652800;

    rtc_time_to_tm(time.tv_sec, tm);
	return rtc_valid_tm(tm);
}

static int mstar_set_time(struct device *dev, struct rtc_time *tm)
{
    unsigned long time;
    rtc_tm_to_time(tm, &time);
    do_settimeofday((struct timespec *)&time);  //!?
	return 0;
}

static const struct rtc_class_ops mstar_rtc_ops = {
	.read_time = mstar_get_time,
	.set_time = mstar_set_time,
};

static int __init mstar_rtc_probe(struct platform_device *dev)
{
	struct rtc_device *rtc;

	rtc = rtc_device_register("rtc-mstar", &dev->dev, &mstar_rtc_ops,
				  THIS_MODULE);
	if (IS_ERR(rtc))
	{
		return PTR_ERR(rtc);
    }
	platform_set_drvdata(dev, rtc);

	return 0;
}

static int __exit mstar_rtc_remove(struct platform_device *dev)
{
	struct rtc_device *rtc = platform_get_drvdata(dev);

	rtc_device_unregister(rtc);

	return 0;
}

static struct platform_driver mstar_rtc_driver = {
	.driver = {
		.name = "rtc-mstar",
		.owner = THIS_MODULE,
	},
	.remove = __exit_p(mstar_rtc_remove),
};

static int __init mstar_rtc_init(void)
{
    int ret;
    ret = platform_driver_probe(&mstar_rtc_driver, mstar_rtc_probe);
	return ret;
}

static void __exit mstar_rtc_fini(void)
{
	platform_driver_unregister(&mstar_rtc_driver);
}

module_init(mstar_rtc_init);
module_exit(mstar_rtc_fini);

MODULE_AUTHOR("Mstarsemi");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("mstar RTC driver");
MODULE_ALIAS("platform:rtc-mstar");
