TEST_PATH=$(PWD)
PLAT=$(PLATFORM)

INCLUDE_PATH += ./
INCLUDE_PATH += ./include

ifeq ($(PLAT),x86)
INCLUDE_PATH += ./extern
SOURCE_FILES += ./extern/softPwm_mock.c
SOURCE_FILES += ./extern/wiringPi_mock.c
else
LIBS    +=  -lwiringPi
endif


SOURCE_FILES += ./common/blikvm_log/blikvm_log.c
SOURCE_FILES += ./common/blikvm_uart/blikvm_uart.c
SOURCE_FILES += ./common/blikvm_util/blikvm_util.c
SOURCE_FILES += ./blikvm_server.c
SOURCE_FILES += ./kvmd/blikvm_fan/blikvm_fan.c
SOURCE_FILES += ./kvmd/blikvm_atx/blikvm_atx.c
SOURCE_FILES += ./kvmd/blikvm_switch/blikvm_switch.c
# SOURCE_FILES += ./kvmd/blikvm_oled/blikvm_oled.c

CC = g++


OFLAGS = -g -Wall

LIBS    +=  -lpthread 

# LIBPATH += $(foreach path,$(DEPS_LIB_PATH),-L$(path))
# LIBPATH += -L${SDK_ROOT_PATH}

IFLAGS += $(foreach path,$(INCLUDE_PATH),-I$(path))

CFLAGS = ${IFLAGS} ${OFLAGS}

.PHONY:clean

all:clean demo.bin
# $(SOURCE_FILES)
demo.bin:demo.c $(SOURCE_FILES)
	$(CC) ${CFLAGS} $^ -o $@ -Wl,-rpath=./lib -Llib ${LIBS}
clean:
	@rm -f demo.bin