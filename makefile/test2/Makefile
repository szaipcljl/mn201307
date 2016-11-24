#当前工程的根目录路径
ROOT_DIR=$(shell pwd)
#最终bin文件的名字
TARGET=myapp
#bin文件所在的目录
BIN_DIR=debug/bin
#目标文件所在的目录
OBJS_DIR=debug/obj
INC_DIR=./include

#设置编译器
CC=gcc
CFLAGS=-g -Wall -I ${INC_DIR}

#debug文件夹的makefile最后执行，需要执行的子目录使用awk排除debug文件夹，读取剩下的文件夹
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
#无需下一行注释的代码，因为我们已经知道debug的makefile最后执行，所以最后直接去debug目录下执行指定的makefile
#DEBUG=$(shell ls -l | grep ^d | awk '{if($$9 == "debug") print $$9}')

#获取当前目录下的c文件集，放在变量CUR_SOURCE中
CUR_SRCS=${wildcard *.c}
#将对应的c文件名转为o文件后放在下面的CUR_OBJS变量中
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SRCS)}
#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中

export CC TARGET OBJS_DIR BIN_DIR ROOT_DIR

#先执行SUBDIRS,最后才能是DEBUG
all:$(SUBDIRS) $(CUR_OBJS) DEBUG
#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
	make -C $@

#直接去debug目录下执行makefile文件
DEBUG:ECHO
	make -C debug
ECHO:
	@echo $(SUBDIRS)
#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@
CLEAN:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*
