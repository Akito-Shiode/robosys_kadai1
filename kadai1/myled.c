// ヘッダファイル読み込み
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

// ライセンス表示
MODULE_AUTHOR("Akito Shiode");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL; 
static volatile u32 *gpio_base = NULL;

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
    int size = 0;
     char sushi[] = {0xF0,0x9F,0x8D,0xA3,0x0A}; //寿司の絵文字のバイナリ
     if(copy_to_user(buf+size,(const char *)sushi, sizeof(sushi))){
        printk( KERN_INFO "sushi : copy_to_user failed\n" );
     return -EFAULT;
     }
     size += sizeof(sushi);
    return size;
}

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
    char c;
    if(copy_from_user(&c,buf,sizeof(char)))
        return -EFAULT;

    if(c == '0')
        {
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(300);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(300);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(300);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(600);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(600);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(600);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(300);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(300);
          gpio_base[10] = 1 << 25;
          msleep(300);
          gpio_base[7] = 1 << 25;
          msleep(300);
          gpio_base[10] = 1 << 25;
        }
    else if(c == '1')
        gpio_base[7] = 1 << 25;
    return 1;
}

static struct file_operations led_fops = {
     .owner = THIS_MODULE,
     .write = led_write,
     .read = sushi_read
};

static int __init init_mod(void)
{
    int retval;
    
    const u32 led = 25;//pin番号を変えたい場合ここを変える
    const u32 index = led/10;//GPFSEL2
    const u32 shift = (led%10)*3;//15bit
    const u32 mask = ~(0x7 << shift);//11111111111111000111111111111111

    gpio_base = ioremap_nocache(0x3f200000, 0xA0);
    gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);//001: output flag
    //11111111111111001111111111111111

    retval =  alloc_chrdev_region(&dev, 0, 1, "myled");
    if(retval < 0){
        printk(KERN_ERR "alloc_chrdev_region failed.\n");
        return retval;
    }
    printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
    
    cdev_init(&cdv, &led_fops);
    retval = cdev_add(&cdv, dev, 1);
    if(retval < 0){
        printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
        return retval;
    }

    cls = class_create(THIS_MODULE,"myled");
    if(IS_ERR(cls)){
        printk(KERN_ERR "class_create failed.");
        return PTR_ERR(cls);
    }
    
    device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

    return 0;
}

static void __exit cleanup_mod(void)
{
    cdev_del(&cdv);
    device_destroy(cls, dev);
    class_destroy(cls);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod); //マクロで関数登録
module_exit(cleanup_mod); //同上

