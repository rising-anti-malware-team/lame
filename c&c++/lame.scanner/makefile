include ../../../../mkpublic

OBJS = lame.scanner.o stdafx.o

TARGET = $(BINDIR)/lame.scanner

CFLAGS += -I../../include

x86 x64 b32 b64 l32 l64: $(TARGET)

$(TARGET) : $(OBJS)
	$(GCXC) $(LDSOOPT) -o $@ $(OBJS) -llame $(LINK_RAME) $(LINK_COMM) -L$(BINDIR)

.cpp.o :
	$(CXC) $(CFLAGS) -I.
.c.o :
	$(CC) $(CFLAGS) -I.

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
