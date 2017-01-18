KERN_DIR = /work/system/Kernel_3.0.8_TQ210_for_Linux_v2.1

DRV_FILE = usbmouse_as_key

all:
	make -C $(KERN_DIR) M=`pwd` modules
	cp $(DRV_FILE).ko /work/nfs_root/rootfs 
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean

obj-m	+= $(DRV_FILE).o
