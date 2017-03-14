/*******************************************************************************
* Copyright (c) 2015 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/
#include "tee_drv.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/cdev.h>			/* Char device regiseter / deregister */
#include <linux/device.h>		/* dynamic device creating */
#include <linux/init.h>
#include <linux/slab.h>			/* kmalloc etc.. */
#include <linux/mm.h>			/* mmap support */
#include <linux/time.h>			/* time functions */
#include <linux/stop_machine.h>

#include <linux/proc_fs.h>		/* for proc fs */
#include <linux/seq_file.h>		/* for sequence proc file support */

UINT32 device_open_cnt = 0;
#define TMSL_RUNNING_SIGNATURE_CORP 0x43544E49	/* "INTC", edx */
#define TMSL_RUNNING_SIGNATURE_VMM  0x4D4D5645	/* "EVMM", ecx */

static int device_open(struct inode *inode, struct file *filep);
static int device_close(struct inode *inode, struct file *filep);
static ssize_t device_read(struct file *filp, char __user * buffer,
						   size_t length, loff_t * offset);
static ssize_t device_write(struct file *filp, const char __user * buff,
							size_t len, loff_t * off);
long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

/* Char device creation functions */
static dev_t tee_drv_dev;		/* Device number */
static struct cdev tee_drv_cdev;	/* Char device structure */
static struct class *tee_drv_class;	/* Device class */

/* File operations structure */
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.compat_ioctl = device_ioctl,
	.open = device_open,
	.release = device_close,
};

/* Declarations related to tee debug proc file */
/* Forward Declarations */
static int tee_debug_proc_open(struct inode *inode, struct file *file);
static int show_tee_debug(struct seq_file *m, void *v);
static int init_tee_proc_fs(void);
static void clean_tee_proc_fs(void);

/* File operations for tee debug proc file */
static const struct file_operations tee_debug_buf_proc_fops = {
	.open = tee_debug_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

unsigned long g_sg_debug_gva = 0;	/* Start address of the debug buffer */
unsigned long g_sg_debug_gva_end = 0;	/* The end adderss of the Debug buffer */
static void send_debug_buffer(void);

/****************************************************************************
* Function: make_vmcall
* Purpose: Function to make fast vmcall
* Parameters:
*    Input: vmcall_id, arg1, arg2
*    Output: None
*****************************************************************************/

#define make_vmcall(vmcall_id, v_rdx, v_rsi) \
	__asm__ __volatile__( \
		"vmcall" \
		::"a"(vmcall_id), "d"(v_rdx), "S"(v_rsi))

/****************************************************************************
* Function: send_debug_buffer
* Purpose: Function to send second guest debug buffer address to TMSL
* Parameters:
*        Input:  None
*        Output: None
*****************************************************************************/
static void send_debug_buffer(void)
{
	unsigned long debug_gpa;
	int vmcall_id = TRUSTY_VMCALL_DEBUG_BUFFER;
	g_sg_debug_gva = (unsigned long)kmalloc(DEBUG_BUF_SIZE, GFP_KERNEL);
	if (!g_sg_debug_gva) {
		printk("Failed to allocate memory for debug_buf");
		return;
	}
	memset((char *)g_sg_debug_gva, 0, DEBUG_BUF_SIZE);
	/* The end addres of the debug buffer */
	g_sg_debug_gva_end = g_sg_debug_gva + DEBUG_BUF_SIZE;
	debug_gpa = virt_to_phys((void *)g_sg_debug_gva);
	printk("SG Debug Buffer: GVA=%lx, GPA=%lx, Size=%d\n", g_sg_debug_gva,
		   debug_gpa, DEBUG_BUF_SIZE);
	make_vmcall(vmcall_id, debug_gpa, DEBUG_BUF_SIZE);
}

/******************************************************************************
*
* VttRunningCheck API checks whether Guest OS is running on VTT
* Returns - TEE_COMM_SUCCESS if TMSL is running, else TEE_COMM_FAIL
*******************************************************************************/
static int VttRunningCheck(void)
{
	uint32_t cpu_info[4], i, a;

	for (i = 0; i < 4; i++)
		cpu_info[i] = 0;

	/* CPUID instruction with 3 as parameter */
	a = 3;
        __asm__ __volatile__(
                        "movl %4,%%eax  ;\n"
                        "cpuid  ;\n"
                        "movl %%eax,%0  ;\n"
                        "movl %%ebx,%1  ;\n"
                        "movl %%ecx,%2  ;\n"
                        "movl %%edx,%3  ;\n"
                        : "=g"(cpu_info[0]), "=g"(cpu_info[1]), "=g"(cpu_info[2]), "=g"(cpu_info[3])
                        : "g"(a)
                        :"%edx", "%ecx", "%ebx", "%eax"
        );
	printk(" cpuid result: cpu_info[0] = 0x%lx\r\n",
		   (long unsigned int)cpu_info[0]);
	printk(" cpuid result: cpu_info[1] = 0x%lx\r\n",
		   (long unsigned int)cpu_info[1]);
	printk(" cpuid result: cpu_info[2] = 0x%lx\r\n",
		   (long unsigned int)cpu_info[2]);
	printk(" cpuid result: cpu_info[3] = 0x%lx\r\n",
		   (long unsigned int)cpu_info[3]);

	/* If signature matches, then TMSL is running */
	if (TMSL_RUNNING_SIGNATURE_CORP == cpu_info[3] &&
		TMSL_RUNNING_SIGNATURE_VMM == cpu_info[2]) {
		return TEE_COMM_SUCCESS;
	}

	return TEE_COMM_FAIL;
}

/* Create a chr device */
static int create_device(void)
{

	/* Allocate char device */
	if (alloc_chrdev_region(&tee_drv_dev, 0, 1, DEVICE_NAME) < 0) {
		printk("Failed alloc_chardev_region\n");
		return -1;
	}

	/* Create a device class */
	if ((tee_drv_class = class_create(THIS_MODULE, "tee_drv_class")) == NULL) {
		unregister_chrdev_region(tee_drv_dev, 1);
		return -1;
	}

	/* Create the device */
	if (device_create(tee_drv_class, NULL, tee_drv_dev, NULL, DEVICE_NAME) ==
		NULL) {
		class_destroy(tee_drv_class);
		unregister_chrdev_region(tee_drv_dev, 1);
		return -1;
	}
	/* Add SHM char device */
	cdev_init(&tee_drv_cdev, &fops);
	if (cdev_add(&tee_drv_cdev, tee_drv_dev, 1) == -1) {
		device_destroy(tee_drv_class, tee_drv_dev);
		class_destroy(tee_drv_class);
		unregister_chrdev_region(tee_drv_dev, 1);
		return -1;
	}
	printk("\nTEE driver installed\n");
	return 0;
}

static void delete_device(void)
{
	printk("Deleting tee_drv device:%s \n", DEVICE_NAME);
	cdev_del(&tee_drv_cdev);
	device_destroy(tee_drv_class, tee_drv_dev);
	class_destroy(tee_drv_class);
	unregister_chrdev_region(tee_drv_dev, 1);
	return;
}

/******************************************************************************
* Function Name: tee_drv_init
* Purposes     : driver init
* Parameters   :
*    INPUT     : None
*    RETURN    : status
*******************************************************************************/
static int __init tee_drv_init(void)
{
	/* TMSL/VTT running check */
	if (VttRunningCheck() == TEE_COMM_FAIL) {
		printk("tee_drv: TEE isn't running !\r\b");
		return -1;
	}
	/* register and create device */
	if (create_device() != 0) {
		printk("Failed to create device:%s\n", DEVICE_NAME);
		return -1;
	}

	/* Initialize the proc file for tee debug messages */
	init_tee_proc_fs();
	/* Send debug buffer address to TMSL */
	send_debug_buffer();

	return (0);
}

/******************************************************************************^
* Function Name:  tee_drv_exit
* Purposes     :  exit driver
* Parameters   :
*    INPUT     : None
*    RETURN    : None
*******************************************************************************/
static void __exit tee_drv_exit(void)
{
	printk("Device exit ....\n");
	/* Unregister and delete the node */
	delete_device();

	/* Cleanup tee proc file */
	clean_tee_proc_fs();
	/* Free the debug buffer memory */
	if (g_sg_debug_gva) {
		kfree((void *)g_sg_debug_gva);
		g_sg_debug_gva = 0;
	}
}

static ssize_t device_read(struct file *filp, char __user * buffer,
						   size_t length, loff_t * offset)
{
	return 0;
}
static ssize_t device_write(struct file *filp, const char __user * buff,
							size_t len, loff_t * off)
{
	return 0;
}

long device_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int device_open(struct inode *inode, struct file *filep)
{
	return 0;
}

int device_close(struct inode *inode, struct file *filep)
{
	return 0;
}

/* Functions definitions related to tee debug log proc file */

/* Seqfile related defs */
/* This function is called when the proc file is read */
static int show_tee_debug(struct seq_file *m, void *v)
{
	char *ptr;
	char *str1 = NULL, *str2 = NULL;
	char buf_tail[16];
	unsigned long buffer_tail_addr_gpa;
	unsigned long buffer_tail_addr_gva;
	int ret;
	buffer_tail_addr_gva = buffer_tail_addr_gpa = 0;
	if (g_sg_debug_gva == 0 || g_sg_debug_gva_end == 0) {
		printk("Debug buffer is NULL\n");
		return 0;
	}
	ptr = (char *)g_sg_debug_gva;
	/* Get the buffer_tail address. The tail address is stored in the start of
	 * the debug buffer (first 64 bytes are reserved for tail address) The
	 * address is stored in this format "***:<address>:***" */
	str1 = strstr(ptr, "***:");
	str1 += 4;					/* Skip "***:" */
	if (str1) {
		str2 = strstr(ptr, ":***");
		if (str2) {
			strncpy(buf_tail, str1, str2 - str1);
			buf_tail[str2 - str1] = '\0';
			ret = strict_strtoul(buf_tail, 16, &buffer_tail_addr_gpa);
			if (ret) {
				printk("strict_strtol Faiiled with error code:%d\n", ret);
				return ret;
			} else {
				buffer_tail_addr_gva =
					(unsigned long)phys_to_virt(buffer_tail_addr_gpa);
			}
		}
	}
	/* The buffer has not rolled over yet and hence the messages are in the
	 * correct order */
	if (!str1 || !str2) {
		printk("Buffer tail not found\n");
		/* Dump all messages from the begining */
		ptr = (char *)(g_sg_debug_gva + 64);
		while (ptr < (char *)g_sg_debug_gva_end) {
			seq_printf(m, "%c", *ptr++);
		}
		return 0;
	}

	/* Buffer has been rolled over */
	/* First print from buffer tail to the end of debug buffer */
	if (buffer_tail_addr_gva && (buffer_tail_addr_gva > g_sg_debug_gva + 64)) {
		ptr = (char *)buffer_tail_addr_gva;
		while (ptr < (char *)g_sg_debug_gva_end) {
			seq_printf(m, "%c", *ptr++);
		}
	}

	/* Then print from begining of the buffer till the buffer tail */
	ptr = (char *)(g_sg_debug_gva + 64);
	while (ptr < (char *)buffer_tail_addr_gva) {
		seq_printf(m, "%c", *ptr++);
	}
	return 0;
}

/* Called in the init module function to open the proc file */
static int tee_debug_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, show_tee_debug, NULL);
}

static int init_tee_proc_fs(void)
{
	/* create the /proc file */
	printk(KERN_INFO "/proc/%s created\n", PROCFS_NAME);
	proc_create(PROCFS_NAME, 0, NULL, &tee_debug_buf_proc_fops);
	return 0;
}

static void clean_tee_proc_fs(void)
{
	printk("\nremove_proc_entry file called for %s\n", PROCFS_NAME);
	remove_proc_entry(PROCFS_NAME, NULL);
}

module_init(tee_drv_init);
module_exit(tee_drv_exit);
MODULE_LICENSE("GPL");
