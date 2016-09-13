#CPPFLAGS += -DNO_PROTOTYPES
#CC=$(TARGET_CROSS)#aarch64-marvell-linux-gnu-gcc
#CFLAGS+=$(TARGET_CFLAGS) $(HOST_CFLAGS)
#LDLIBS+=$(TARGET_LD)
LDLIBS += -lm -lpthread

all: dmtest

dmtest: dmtest.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
clean:
	rm -f *.o dmtest

.PHONY: all clean
