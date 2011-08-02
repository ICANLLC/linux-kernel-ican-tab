/*
 *
 * Touch Screen I2C Driver for EETI Controller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

// Release Date: 2010/12/27
// Based on 2010/11/08 and add loopback function 

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/freezer.h>
#include <linux/proc_fs.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/poll.h>
#include <linux/kfifo.h>
#include <linux/version.h>
#include <linux/input.h>
#include <linux/irq.h>

//#define DEBUG
#ifdef DEBUG
	#define TS_DEBUG(fmt,args...)  printk( KERN_ERR "[ms2032_i2c]: " fmt, ## args)
	#define DBG() printk("[%s]:%d => \n",__FUNCTION__,__LINE__)
#else
	#define TS_DEBUG(fmt,args...)
	#define DBG()
#endif

//#define _NON_INPUT_DEV // define this to disable register input device	

static int global_major = 0; // dynamic major by default 
static int global_minor = 0;
extern int imap_iokey_keyled(int, int);
extern int imap_iokey_motor(int, int);
unsigned int ms2032_int;
unsigned int ms2032_irq_no;

#define MAX_I2C_LEN		8
#define FIFO_SIZE		PAGE_SIZE
#define MAX_SUPPORT_POINT	5
#define REPORTID_MOUSE		0x01
#define REPORTID_VENDOR		0x52
#define REPORTID_MTOUCH		0x04

/// ioctl command ///
#define EGALAX_IOC_MAGIC	0x72
#define	EGALAX_IOCWAKEUP	_IO(EGALAX_IOC_MAGIC, 1)
#define EGALAX_IOC_MAXNR	1

/// delays ///
#define EGALAX_JUNK_DELAY	22222222
#define EGALAX_POINT_DELAY	0

struct point_data {
	short Status;
	short X;
	short Y;
};

struct _ms2032_i2c {
	struct workqueue_struct *ktouch_wq;
	struct delayed_work work;
	struct mutex mutex_wq;
	struct i2c_client *client;
	struct hrtimer timer;
	char work_state;
	char skip_packet;
};

struct ms2032_char_dev
{
	int OpenCnts;
	struct cdev cdev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	struct kfifo* pDataKFiFo;
#else
	struct kfifo DataKFiFo;
#endif
	unsigned char *pFiFoBuf;
	spinlock_t FiFoLock;
	struct semaphore sem;
	wait_queue_head_t fifo_inq;
};

static long unsigned int g_pcnt = 0;
static uint32_t g_pjmp = 0;
static struct _ms2032_i2c *p_ms2032_i2c_dev = NULL;	// allocated in ms2032_i2c_probe
static struct ms2032_char_dev *p_char_dev = NULL;	// allocated in init_module
static atomic_t ms2032_char_available = ATOMIC_INIT(1);
static struct class *ms2032_class;
#ifndef _NON_INPUT_DEV
static struct input_dev *input_dev = NULL;
static struct point_data PointBuf[MAX_SUPPORT_POINT];
#endif //#ifndef _NON_INPUT_DEV

/* code for imapx220 */
static int ms2032_int_out(void) {
	/* not supported for EINT1 */
	return 0;
}

static int ms2032_int_in(void) {
	/* not supported for EINT1 */
	return 0;
}

static int ms2032_int_val(void) {
	return !!(imapx_gpio_getpin(ms2032_int, IG_NORMAL));
}

static void ms2032_int_init(void) {
	ms2032_int  = __imapx_name_to_gpio(CONFIG_CTP_MSG2032_INT);
	if(ms2032_int == IMAPX_GPIO_ERROR) {
		printk(KERN_ERR "failed to get ms2032_int pin.\n");
		return -1;
	}  
	ms2032_irq_no = imapx_gpio_to_irq(ms2032_int);	
	imapx_gpio_setcfg(ms2032_int, IG_INPUT, IG_NORMAL);
	imapx_gpio_setirq(ms2032_int, FILTER_MAX, IG_RSIE, 1);
}

static int ms2032_new_imapdev(void)
{
	struct i2c_client *client;
	struct i2c_adapter *adapter;
	struct i2c_board_info info = {
		.addr	= 0x60,
		//.irq	= IRQ_GPIO,
		.type	= "ms2032_i2c",
	};

	adapter = i2c_get_adapter(CONFIG_CTP_MSG2032_I2C + 1);

	if(!adapter)
	{
		printk(KERN_ERR "ms2032 get i2c bus 0 failed.\n");
		return -1;
	}

	/* init interrupt */
	ms2032_int_init();

	client = i2c_new_device(adapter, &info);
	if(!client)
	{
		printk(KERN_ERR "Can't add i2c device at 0x%x\n", (uint32_t)info.addr);
		return -1;
	}
	client->irq = ms2032_irq_no; 
	return 0;
}
/* END: code for imapx220 */

static int ms2032_cdev_open(struct inode *inode, struct file *filp)
{
	struct ms2032_char_dev *cdev;

	DBG();

	cdev = container_of(inode->i_cdev, struct ms2032_char_dev, cdev);
	if( cdev == NULL )
	{
        	TS_DEBUG(" No such char device node \n");
		return -ENODEV;
	}
	
	if( !atomic_dec_and_test(&ms2032_char_available) )
	{
		atomic_inc(&ms2032_char_available);
		return -EBUSY; /* already open */
	}

	cdev->OpenCnts++;
	filp->private_data = cdev;// Used by the read and write metheds

	TS_DEBUG(" ms2032_cdev_open done \n");
	try_module_get(THIS_MODULE);
	return 0;
}

static int ms2032_cdev_release(struct inode *inode, struct file *filp)
{
	struct ms2032_char_dev *cdev; // device information

	DBG();

	cdev = container_of(inode->i_cdev, struct ms2032_char_dev, cdev);
        if( cdev == NULL )
        {
                TS_DEBUG(" No such char device node \n");
                return -ENODEV;
        }

	atomic_inc(&ms2032_char_available); /* release the device */

	filp->private_data = NULL;
	cdev->OpenCnts--;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	kfifo_reset( cdev->pDataKFiFo );
#else
	kfifo_reset( &cdev->DataKFiFo );
#endif

	TS_DEBUG(" ms2032_cdev_release done \n");
	module_put(THIS_MODULE);
	return 0;
}

#define MAX_READ_BUF_LEN	50
static char fifo_read_buf[MAX_READ_BUF_LEN];
static ssize_t ms2032_cdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	int read_cnt, ret, fifoLen;
	struct ms2032_char_dev *cdev = file->private_data;

	DBG();
	
	if( down_interruptible(&cdev->sem) )
		return -ERESTARTSYS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	fifoLen = kfifo_len(cdev->pDataKFiFo);
#else
	fifoLen = kfifo_len(&cdev->DataKFiFo);
#endif

	while( fifoLen<1 ) /* nothing to read */
	{
		up(&cdev->sem); /* release the lock */
		if( file->f_flags & O_NONBLOCK )
			return -EAGAIN;

	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
		if( wait_event_interruptible(cdev->fifo_inq, kfifo_len( cdev->pDataKFiFo )>0) )
	#else
		if( wait_event_interruptible(cdev->fifo_inq, kfifo_len( &cdev->DataKFiFo )>0) )
	#endif
		{
			return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
		}

		if( down_interruptible(&cdev->sem) )
			return -ERESTARTSYS;
	}

	if(count > MAX_READ_BUF_LEN)
		count = MAX_READ_BUF_LEN;

	TS_DEBUG("\"%s\" reading: real fifo data\n", current->comm);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	read_cnt = kfifo_get(cdev->pDataKFiFo, fifo_read_buf, count);
#else
	read_cnt = kfifo_out_locked(&cdev->DataKFiFo, fifo_read_buf, count, &cdev->FiFoLock);
#endif

	ret = copy_to_user(buf, fifo_read_buf, read_cnt)?-EFAULT:read_cnt;

	up(&cdev->sem);
	
	return ret;
}

static ssize_t ms2032_cdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	char *tmp;
	struct ms2032_char_dev *cdev = file->private_data;
	int ret=0;

	DBG();

	if( down_interruptible(&cdev->sem) )
		return -ERESTARTSYS;

	if (count > MAX_I2C_LEN)
		count = MAX_I2C_LEN;

	tmp = kmalloc(count,GFP_KERNEL);
	if(tmp==NULL)
	{
		up(&cdev->sem);
		return -ENOMEM;
	}

	if(copy_from_user(tmp, buf, count))
	{
		up(&cdev->sem);
		kfree(tmp);
		return -EFAULT;
	}
	
	ret = i2c_master_send(p_ms2032_i2c_dev->client, tmp, count);
	TS_DEBUG("I2C writing %zu bytes.\n", count);

	kfree(tmp);

	up(&cdev->sem);

	return ret;
}

static int wakeup_controller(void)
{
	int ret=0;

	ms2032_int_out();
	udelay(10);
	ms2032_int_in();
	printk(KERN_ERR "[ms2032_i2c]: INT wakeup touch controller done\n");
	
	return ret;
}

static int ms2032_cdev_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long args)
{	
	//struct ms2032_char_dev *cdev = file->private_data;
	int ret=0;

	if(_IOC_TYPE(cmd) != EGALAX_IOC_MAGIC)
		return -ENOTTY;
	if(_IOC_NR(cmd) > EGALAX_IOC_MAXNR)
		return -ENOTTY;

	if(_IOC_DIR(cmd) & _IOC_READ)
		ret = !access_ok(VERIFY_WRITE, (void __user*)args, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		ret = !access_ok(VERIFY_READ, (void __user*)args, _IOC_SIZE(cmd));

	if(ret)
		return -EFAULT;

	//printk(KERN_ERR "Handle device ioctl command\n");
	switch (cmd)
	{
		case EGALAX_IOCWAKEUP:
			ret = wakeup_controller();
			break;
		default:
			ret = -ENOTTY;
			break;
	}

	return ret;
}

static unsigned int ms2032_cdev_poll(struct file *filp, struct poll_table_struct *wait)
{
	struct ms2032_char_dev *cdev = filp->private_data;
	unsigned int mask = 0;
	int fifoLen;
	
	down(&cdev->sem);
	poll_wait(filp, &cdev->fifo_inq,  wait);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	fifoLen = kfifo_len(cdev->pDataKFiFo);
#else
	fifoLen = kfifo_len(&cdev->DataKFiFo);
#endif

	if( fifoLen > 0 )
		mask |= POLLIN | POLLRDNORM;    /* readable */
	if( (FIFO_SIZE - fifoLen) > MAX_I2C_LEN )
		mask |= POLLOUT | POLLWRNORM;   /* writable */

	up(&cdev->sem);
	return mask;
}

#ifndef _NON_INPUT_DEV
static int LastUpdateID = 0;
static void ProcessReport(unsigned char *buf, int buflen)
{
	int i;
	short X=0, Y=0, dst_x=0, dst_y=0, status;
	static uint32_t key_code[4] = { KEY_HOME, KEY_MENU, KEY_BACK, KEY_SEARCH };

	X     = ((buf[1] & 0xf0) << 4) | buf[2];
	Y     = ((buf[1] & 0x0f) << 8) | buf[3];
	dst_x = ((buf[4] & 0xf0) << 4) | buf[5];
	dst_y = ((buf[4] & 0x0f) << 8) | buf[6];
	dst_y <<= 4;
	dst_y /= 16;

	if(buf[0] == 0x53)
	{
		if(buf[1] > 3)
		{
			printk(KERN_ERR "wrong button\n");
			return ;
		}

		/* this is a virtual key */
		input_event(input_dev, EV_KEY, key_code[buf[1]], buf[2]);
		if(buf[2]) {
			imap_iokey_keyled(1, 10000);
			imap_iokey_motor(1, 40);
		}
		return ;
	}

	status = !((buf[1] & buf[2] & buf[3] & buf[4] & buf[5] & buf[6]) == 0xff);

	/* Only report 1/4 points or finger ups*/
#if 0
	g_pjmp = (g_pjmp + 1) & 0x3;
	if((g_pjmp != 1) && Status)
	{
//		TS_DEBUG("point jumped\n");
		return ;
	}
#endif

	g_pcnt++;
	if(!(g_pcnt & 0xff))
	  TS_DEBUG("p cnt: %lu\n", g_pcnt);

	input_report_abs(input_dev, ABS_MT_TRACKING_ID, 0);			
	input_report_abs(input_dev, ABS_MT_WIDTH_MAJOR, 0);
	input_report_abs(input_dev, ABS_MT_TOUCH_MAJOR, status);
	input_report_abs(input_dev, ABS_MT_POSITION_X, X);
	input_report_abs(input_dev, ABS_MT_POSITION_Y, Y);
	TS_DEBUG("(%d, %d)%d\n", X, Y, status);
	input_mt_sync(input_dev);

	if(dst_x || dst_y)
	{
		input_report_abs(input_dev, ABS_MT_TRACKING_ID, 1);			
		input_report_abs(input_dev, ABS_MT_WIDTH_MAJOR, 0);
		input_report_abs(input_dev, ABS_MT_TOUCH_MAJOR, status);
		input_report_abs(input_dev, ABS_MT_POSITION_X, X + dst_x);
		input_report_abs(input_dev, ABS_MT_POSITION_Y, Y + dst_y);
		TS_DEBUG("mt(%d, %d)%d\n", X + dst_x, Y + dst_y, status);
//		TS_DEBUG("i:%d,x:%d,y:%d)%d\n", i, dst_x, dst_y);
		input_mt_sync(input_dev);
	}

	input_sync(input_dev);
//	TS_DEBUG("Input sync point data done!\n");
}

static struct input_dev * allocate_Input_Dev(void)
{
	int ret;
	struct input_dev *pInputDev=NULL;

	pInputDev = input_allocate_device();
	if(pInputDev == NULL)
	{
		TS_DEBUG("Failed to allocate input device\n");
		return NULL;//-ENOMEM;
	}

	pInputDev->name = "eGalax Touch Screen";
	pInputDev->phys = "I2C";
	pInputDev->id.bustype = BUS_I2C;
	pInputDev->id.vendor = 0x0EEF;
	pInputDev->id.product = 0x0020;
	
	set_bit(EV_ABS, pInputDev->evbit);
	set_bit(EV_KEY, pInputDev->evbit);
	set_bit(KEY_HOME, pInputDev->keybit);
	set_bit(KEY_SEARCH, pInputDev->keybit);
	set_bit(KEY_BACK, pInputDev->keybit);
	set_bit(KEY_MENU, pInputDev->keybit);


	input_set_abs_params(pInputDev, ABS_MT_POSITION_X, 0, 2047, 0, 0);
	input_set_abs_params(pInputDev, ABS_MT_POSITION_Y, 0, 2047, 0, 0);
	input_set_abs_params(pInputDev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(pInputDev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(pInputDev, ABS_MT_TRACKING_ID, 0, 10, 0, 0);

	ret = input_register_device(pInputDev);
	if(ret) 
	{
		TS_DEBUG("Unable to register input device.\n");
		input_free_device(pInputDev);
		return NULL;
	}
	
	return pInputDev;
}
#endif //#ifndef _NON_INPUT_DEV

static int ms2032_checksum(uint8_t *buf, int len)
{
	int i, sum = 0;

	for(i = 0; i < len; i++)
	  sum += buf[i];
	
	return (uint8_t)((-sum) & 0xff);
}


static int ms2032_i2c_measure(struct i2c_client *client, char skip_packet)
{
	u8 x_buf[MAX_I2C_LEN];
	int count, loop=3;
	
//	DBG();

	do{
		count = i2c_master_recv(client, x_buf, MAX_I2C_LEN);
	}while(count == -EAGAIN && --loop);

#if 1
	if( count<0 || (x_buf[0]!=REPORTID_VENDOR))
	{
		TS_DEBUG("I2C read error data with Len=%d hedaer=%d\n", count, x_buf[0]);
		return -1;
	}
#endif

//	TS_DEBUG("ms2032_i2c read data with Len=%d header=%x\n", count, x_buf[0]);
#if 0
	if(x_buf[0]==REPORTID_VENDOR)
		TS_DEBUG("ms2032_i2c get command packet, packet=> "
		   "0x%x|0x%x|0x%x|0x%x|0x%x|0x%x|0x%x|0x%x|0x%x|0x%x\n", x_buf[0],
		   x_buf[1], x_buf[2], x_buf[3], x_buf[4], x_buf[5],
		   x_buf[6], x_buf[7]);
#endif
	if(ms2032_checksum(x_buf, MAX_I2C_LEN - 1) != x_buf[7])
	{
		TS_DEBUG("packet checksum failed.\n");
		return -1;
	}

	if( skip_packet > 0 )
	{
		TS_DEBUG("skiping package: %d\n", skip_packet);
		return count;
	}

#ifndef _NON_INPUT_DEV
	if( count>0 && x_buf[0]==REPORTID_VENDOR)
	{
		ProcessReport(x_buf, count);

		return count;
	}
#endif //#ifndef _NON_INPUT_DEV

	if( count>0 && p_char_dev->OpenCnts>0 ) // If someone reading now! put the data into the buffer!
	{
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
		kfifo_put(p_char_dev->pDataKFiFo, x_buf, count);
	#else
		kfifo_in_locked(&p_char_dev->DataKFiFo, x_buf, count, &p_char_dev->FiFoLock);
	#endif
	 	wake_up_interruptible( &p_char_dev->fifo_inq );
	}

	return count;
}

static enum hrtimer_restart
ms2032_i2c_timer(struct hrtimer *handle)
{
	struct i2c_client *client = p_ms2032_i2c_dev->client;
	if( p_ms2032_i2c_dev->work_state > 0 )
		enable_irq(client->irq);

	return HRTIMER_NORESTART;
}

static void ms2032_i2c_wq(struct work_struct *work)
{
	struct _ms2032_i2c *ms2032_i2c = container_of(work, struct _ms2032_i2c, work);
	struct i2c_client *client = ms2032_i2c->client;
	int ret = 0;

//	TS_DEBUG("ms2032_i2c_wq run\n");

	mutex_lock(&ms2032_i2c->mutex_wq);

	/*continue recv data*/
	ret = ms2032_i2c_measure(client, ms2032_i2c->skip_packet);

#if 0
	/* junk data, delay 20ms */
	hrtimer_start(&ms2032_i2c->timer, ktime_set(0, EGALAX_POINT_DELAY),
	   HRTIMER_MODE_REL);
#endif
	
	if( ms2032_i2c->skip_packet > 0 )
		ms2032_i2c->skip_packet = 0;

	mutex_unlock(&ms2032_i2c->mutex_wq);

#if 0
	if(!ms2032_int_val() && ms2032_i2c->work_state > 0)
	{
		queue_delayed_work(ms2032_i2c->ktouch_wq, &ms2032_i2c->work, 25);
		return ;
	}
#endif
//	TS_DEBUG("ms2032_i2c_wq leave\n");
}

static irqreturn_t ms2032_i2c_interrupt(int irq, void *dev_id)
{
	struct _ms2032_i2c *ms2032_i2c = (struct _ms2032_i2c *)dev_id;
	uint32_t tmp, pnd;

//	TS_DEBUG("ms2032_i2c_interrupt with irq:%d\n", irq);

//	TS_DEBUG("irq: %d\n", ms2032_int_val());
	//pnd = readl(rEINTG4PEND);
	//if(!(pnd & (1 << 3)))      
	//  return IRQ_HANDLED;
	if(!(imapx_gpio_is_pending(ms2032_int, 0)))
		return IRQ_HANDLED;

	/* mask the interrupt temperory out */      
	//writel(readl(rEINTG4MASK) | (1 << 3), rEINTG4MASK);
	imapx_gpio_mask_irq(ms2032_int);

	/* clear pend */       
	//writel((1 << 3), rEINTG4PEND);
	imapx_gpio_clear_pend(ms2032_int);

	queue_delayed_work(ms2032_i2c->ktouch_wq, &ms2032_i2c->work, 0);

	/* unmask the interrupt temperory out */
	//writel(readl(rEINTG4MASK) & ~(1 << 3), rEINTG4MASK);
	imapx_gpio_unmask_irq(ms2032_int);

	return IRQ_HANDLED;
}

#ifdef CONFIG_PM
static int ms2032_i2c_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct _ms2032_i2c *ms2032_i2c = i2c_get_clientdata(client);
	u8 cmdbuf[MAX_I2C_LEN]={0x03, 0x05, 0x0A, 0x03, 0x36, 0x3F, 0x02, 0, 0, 0};
	
	i2c_master_send(client, cmdbuf, MAX_I2C_LEN);

	ms2032_i2c->work_state = 0;
	disable_irq(client->irq);
	cancel_work_sync(&ms2032_i2c->work);

	printk(KERN_DEBUG "[ms2032_i2c]: device suspend done\n");	

	if(device_may_wakeup(&client->dev)) 
	{
		enable_irq_wake(client->irq);
	}
	else 
	{
		printk(KERN_DEBUG "[ms2032_i2c]: device_may_wakeup false\n");
	}

	return 0;
}

static int ms2032_i2c_resume(struct i2c_client *client)
{
	struct _ms2032_i2c *ms2032_i2c = i2c_get_clientdata(client);
	
	if(device_may_wakeup(&client->dev)) 
	{
		disable_irq_wake(client->irq);
	}
	else 
	{
		printk(KERN_DEBUG "[ms2032_i2c]: device_may_wakeup false\n");
	}

	wakeup_controller();
	ms2032_i2c->work_state = 1;
	enable_irq(client->irq);

	printk(KERN_DEBUG "[ms2032_i2c]: device wakeup done\n");

	return 0;
}
#else
#define ms2032_i2c_suspend       NULL
#define ms2032_i2c_resume        NULL
#endif

static void sendLoopback(struct i2c_client *client)
{
	u8 cmdbuf[MAX_I2C_LEN]={0x03, 0x03, 0x0A, 0x01, 0x44, 0, 0, 0, 0, 0};
	i2c_master_send(client, cmdbuf, MAX_I2C_LEN);
}

static int __devinit ms2032_i2c_probe(struct i2c_client *client,
   const struct i2c_device_id *id)
{
	int ret;

	DBG();
	printk(KERN_DEBUG "[ms2032_i2c]: start probe\n");

	p_ms2032_i2c_dev = (struct _ms2032_i2c *)kzalloc(sizeof(struct _ms2032_i2c), GFP_KERNEL);
	if (!p_ms2032_i2c_dev) 
	{
		printk(KERN_ERR "[ms2032_i2c]: request memory failed\n");
		ret = -ENOMEM;
		goto fail1;
	}

#ifndef _NON_INPUT_DEV
	input_dev = allocate_Input_Dev();
	if(input_dev==NULL)
	{
		printk(KERN_ERR "[ms2032_i2c]: allocate_Input_Dev failed\n");
		ret = -EINVAL; 
		goto fail2;
	}
	TS_DEBUG("ms2032_i2c register input device done\n");
	memset(PointBuf, 0, sizeof(struct point_data)*MAX_SUPPORT_POINT);
#endif //#ifndef _NON_INPUT_DEV

	p_ms2032_i2c_dev->client = client;
	mutex_init(&p_ms2032_i2c_dev->mutex_wq);

	p_ms2032_i2c_dev->ktouch_wq = create_workqueue("ms2032_touch_wq"); 
	INIT_DELAYED_WORK(&p_ms2032_i2c_dev->work, ms2032_i2c_wq);
	
	hrtimer_init(&p_ms2032_i2c_dev->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	p_ms2032_i2c_dev->timer.function = ms2032_i2c_timer;

	i2c_set_clientdata(client, p_ms2032_i2c_dev);

	sendLoopback(client);

	if( ms2032_int_val() )
		p_ms2032_i2c_dev->skip_packet = 0;
	else
		p_ms2032_i2c_dev->skip_packet = 0;

	p_ms2032_i2c_dev->work_state = 1;
	ret = request_irq(client->irq, ms2032_i2c_interrupt, IRQF_DISABLED,
		 client->name, p_ms2032_i2c_dev);
	if( ret ) 
	{
		printk(KERN_ERR "[ms2032_i2c]: request irq(%d) failed\n", client->irq);
		goto fail3;
	}
	TS_DEBUG("ms2032_i2c request irq(%d) with result:%d\n", client->irq, ret);

#ifdef CONFIG_PM
	device_init_wakeup(&client->dev, 1);
#endif

	printk(KERN_DEBUG "[ms2032_i2c]: probe done\n");
	return 0;

fail3:
	i2c_set_clientdata(client, NULL);
	destroy_workqueue(p_ms2032_i2c_dev->ktouch_wq); 
	free_irq(client->irq, p_ms2032_i2c_dev);
#ifndef _NON_INPUT_DEV
	input_unregister_device(input_dev);
	input_free_device(input_dev);
	input_dev = NULL;
#endif //#ifndef _NON_INPUT_DEV
fail2:
fail1:
	kfree(p_ms2032_i2c_dev);
	p_ms2032_i2c_dev = NULL;

	printk(KERN_DEBUG "[ms2032_i2c]: probe failed\n");
	return ret;
}

static int __devexit ms2032_i2c_remove(struct i2c_client *client)
{
	struct _ms2032_i2c *ms2032_i2c = i2c_get_clientdata(client);

	DBG();

#ifndef _NON_INPUT_DEV
	if(input_dev)
	{
		TS_DEBUG("unregister input device\n");
		input_unregister_device(input_dev);
		input_free_device(input_dev);
		input_dev = NULL;
	}
#endif //#ifndef _NON_INPUT_DEV

	if(p_ms2032_i2c_dev->ktouch_wq) 
	{
		destroy_workqueue(p_ms2032_i2c_dev->ktouch_wq); 
	}

	if(client->irq)
	{
		free_irq(client->irq, ms2032_i2c);
	}

	i2c_set_clientdata(client, NULL);
	kfree(ms2032_i2c);
	p_ms2032_i2c_dev = NULL;

	return 0;
}

static struct i2c_device_id ms2032_i2c_idtable[] = { 
	{ "ms2032_i2c", 0 }, 
	{ } 
}; 

MODULE_DEVICE_TABLE(i2c, ms2032_i2c_idtable);

static struct i2c_driver ms2032_i2c_driver = {
	.driver = {
		.name 	= "ms2032_i2c",
	},
	.id_table	= ms2032_i2c_idtable,
	.probe		= ms2032_i2c_probe,
	.remove		= __devexit_p(ms2032_i2c_remove),
	.suspend	= ms2032_i2c_suspend,
	.resume		= ms2032_i2c_resume,
};

static const struct file_operations ms2032_cdev_fops = {
	.owner	= THIS_MODULE,
	.read	= ms2032_cdev_read,
	.write	= ms2032_cdev_write,
	.ioctl	= ms2032_cdev_ioctl,
	.poll	= ms2032_cdev_poll,
	.open	= ms2032_cdev_open,
	.release= ms2032_cdev_release,
};

static void ms2032_i2c_ts_exit(void)
{
	dev_t devno = MKDEV(global_major, global_minor);
	DBG();

	if(p_char_dev)
	{
		TS_DEBUG("unregister character device\n");
		if( p_char_dev->pFiFoBuf )
			kfree(p_char_dev->pFiFoBuf);
	
		cdev_del(&p_char_dev->cdev);
		kfree(p_char_dev);
		p_char_dev = NULL;
	}

	unregister_chrdev_region( devno, 1);

	if(!IS_ERR(ms2032_class))
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
		class_device_destroy(ms2032_class, devno);
#else
		device_destroy(ms2032_class, devno);
#endif 
		class_destroy(ms2032_class);
	}
	
	i2c_del_driver(&ms2032_i2c_driver);

	printk(KERN_DEBUG "[ms2032_i2c]: driver exit\n");
}

static struct ms2032_char_dev* setup_chardev(dev_t dev)
{
	struct ms2032_char_dev *pCharDev;
	int result;

	pCharDev = kmalloc(1*sizeof(struct ms2032_char_dev), GFP_KERNEL);
	if(!pCharDev) 
		goto fail_cdev;
	memset(pCharDev, 0, sizeof(struct ms2032_char_dev));

	spin_lock_init( &pCharDev->FiFoLock );
	pCharDev->pFiFoBuf = kmalloc(sizeof(unsigned char)*FIFO_SIZE, GFP_KERNEL);
	if(!pCharDev->pFiFoBuf)
		goto fail_fifobuf;
	memset(pCharDev->pFiFoBuf, 0, sizeof(unsigned char)*FIFO_SIZE);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
	pCharDev->pDataKFiFo = kfifo_init(pCharDev->pFiFoBuf, FIFO_SIZE, GFP_KERNEL, &pCharDev->FiFoLock);
	if( pCharDev->pDataKFiFo==NULL )
		goto fail_kfifo;
#else
	kfifo_init(&pCharDev->DataKFiFo, pCharDev->pFiFoBuf, FIFO_SIZE);
	if( !kfifo_initialized(&pCharDev->DataKFiFo) )
		goto fail_kfifo;
#endif
	
	pCharDev->OpenCnts = 0;
	cdev_init(&pCharDev->cdev, &ms2032_cdev_fops);
	pCharDev->cdev.owner = THIS_MODULE;
	sema_init(&pCharDev->sem, 1);
	init_waitqueue_head(&pCharDev->fifo_inq);

	result = cdev_add(&pCharDev->cdev, dev, 1);
	if(result)
	{
		TS_DEBUG(KERN_ERR "Error cdev ioctldev added\n");
		goto fail_kfifo;
	}

	return pCharDev; 

fail_kfifo:
	kfree(pCharDev->pFiFoBuf);
fail_fifobuf:
	kfree(pCharDev);
fail_cdev:
	return NULL;
}

static int ms2032_i2c_ts_init(void)
{
	int result;
	dev_t devno = 0;

	DBG();

	// Asking for a dynamic major unless directed otherwise at load time.
	if(global_major) 
	{
		devno = MKDEV(global_major, global_minor);
		result = register_chrdev_region(devno, 1, "ms2032_i2c");
	} 
	else 
	{
		result = alloc_chrdev_region(&devno, global_minor, 1, "ms2032_i2c");
		global_major = MAJOR(devno);
	}

	if (result < 0)
	{
		TS_DEBUG(" ms2032_i2c cdev can't get major number\n");
		return 0;
	}

	// allocate the character device
	p_char_dev = setup_chardev(devno);
	if(!p_char_dev) 
	{
		result = -ENOMEM;
		goto fail;
	}

	ms2032_class = class_create(THIS_MODULE, "ms2032_i2c");
	if(IS_ERR(ms2032_class))
	{
		TS_DEBUG("Err: failed in creating class.\n");
		result = -EFAULT;
		goto fail;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	class_device_create(ms2032_class, NULL, devno, NULL, "ms2032_i2c");
#else
	device_create(ms2032_class, NULL, devno, NULL, "ms2032_i2c");
#endif
	TS_DEBUG("register ms2032_i2c cdev, major: %d \n",global_major);

	printk(KERN_DEBUG "[ms2032_i2c]: init done\n");

	/* Here add the device and driver */
	ms2032_new_imapdev();
	return i2c_add_driver(&ms2032_i2c_driver);

fail:	
	ms2032_i2c_ts_exit();
	return result;
}

module_init(ms2032_i2c_ts_init);
module_exit(ms2032_i2c_ts_exit);

MODULE_DESCRIPTION("ms2032 touch screen i2c driver");
MODULE_LICENSE("GPL");

