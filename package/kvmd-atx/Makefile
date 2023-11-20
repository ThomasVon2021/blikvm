INCLUDE_PATH += ../../third_lib

SOURCE_FILES += ../../third_lib/GPIO/armbianio.c
SOURCE_FILES += ../../third_lib/GPIO/softPwm.c

CC = gcc
OFLAGS += -g -Wall 

IFLAGS += $(foreach path,$(INCLUDE_PATH),-I$(path))

CFLAGS += ${IFLAGS} ${OFLAGS}

.PHONY:clean

all:clean atx
# $(SOURCE_FILES)
atx:atx.c $(SOURCE_FILES)
	$(CC) -D_GNU_SOURCE ${CFLAGS} $^ -o $@
clean:
	@rm -f atx