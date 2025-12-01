TARGET = adpcmrec.x adpcmplay.x

CROSS = m68k-xelf-
CC = $(CROSS)gcc
LD = $(CROSS)gcc
CFLAGS = -Wall -O2 -g
LDFLAGS =
LIBS = 

SRCS = adpcmrec.c adpcmplay.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

%.x: %.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f $(TARGET) $(OBJS) *.elf

.PHONY: all clean
