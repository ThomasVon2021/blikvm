TEST_PATH=$(PWD)

INCLUDE_PATH += ./
INCLUDE_PATH += ./include

SOURCE_FILES += ./common/blikvm_log/blikvm_log.c
SOURCE_FILES += ./blikvm_server.c
SOURCE_FILES += ./kvmd/blikvm_fan/blikvm_fan.c
SOURCE_FILES += ./kvmd/blikvm_atx/blikvm_atx.c
SOURCE_FILES += ./kvmd/blikvm_oled/blikvm_oled.c

CC = g++


OFLAGS = -g -Wall

LIBS     =  -lpthread -lwiringPi

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