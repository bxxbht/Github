CROSS_COMPILE=/usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
EXEC1 = ipc-system
OBJS1 = main.o

all: $(EXEC1)

$(EXEC1): $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $(OBJS1) $(LDLIBS) -lpthread
	$(STRIP) -R .comment -R .note $(EXEC1)

clean:
	-rm -f $(EXEC1) $(EXEC2) *.elf *.gdb *.o


