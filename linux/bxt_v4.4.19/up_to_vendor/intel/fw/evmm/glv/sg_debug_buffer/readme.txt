tee_drv is a Loadable Kernel Module used to provide the /proc 
file support for dumping the messages from the TEE (second container). 

Building and running the driver
-------------------------------

1. For Linux, run "make clean all" to build the driver and launcher.
   For Android,
       copy the folder to linux/driver under Android kernel repo, update
       the Makefile to add "obj-y += sguest_launch/", then rebuild the
       Android kernel.

2. If the build is successful, it creates tee_drv.ko 
3. To install tee_drv.ko run "sudo insmod tee_drv.ko"

/proc file interface for the second container messages
------------------------------------------------------
When the tee_drv.ko is installed, the /proc/ file interface
is created under /proc/tee_messages and the TEE messages will be
dumped every time you read this file. Ex. cat /proc/tee_messages.
By Default, 16MB of buffer is used as ring buffer for the messages.
The buffer size can be changes via DEBUG_BUF_SIZE macro in tee_drv.h


To Uninstall the driver remove the module using the 
command "sudo rmmod tee_drv"


