#!/bin/bash

#get make args
MAKE_TARGET = $(MAKECMDGOALS)
EMPTY =
ifeq ($(MAKECMDGOALS), $(EMPTY))  #默认编译
	MAKE_TARGET = SERVER_SHARE
endif

#SERVER_SHARE编译相关参数配置
ifeq ($(MAKE_TARGET), SERVER_SHARE)
	TARTGET_MAKEFILE       = Makefile.Master
	export PRODUCT_SERIAS  = PRODUCT_SERVER
	export DEV_ARCH        = SERVER
	#暂用rv1126进行编译
	#export MAKE_COMPILE    = /opt/toolchains-IFPD/Rockchip/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
	export MAKE_COMPILE
	#编译库
	#export DIR_LIB         = ./module/
	#生成目标路径
	export HICORE_DEST_DIR = ../package/home
endif

#$(error $(TARTGET_MAKEFILE))
OPTIONS = $(options)
ops:
	$(MAKE) $(MAKECMDGOALS) --no-print-directory -f $(TARTGET_MAKEFILE) options=$(OPTIONS)
	
update:
	#@svn up
	
clean:
	@find . -name "*.o"     | xargs rm -f
	@find . -name "*.flint" | xargs rm -f
	@find . -name "*.d"     | xargs rm -f
	@find . -name "*.d.*"   | xargs rm -f
	@find . -name "*.gcno"  | xargs rm -f

