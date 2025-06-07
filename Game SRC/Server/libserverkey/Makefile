CC = clang++15
CFLAGS = -m32 -Wall -O2 -pipe -D_THREAD_SAFE -fno-exceptions
CXXFLAGS = -std=c++17 -Wno-deprecated

BINDIR = .
BIN = $(BINDIR)/libserverkey.a

INCDIR =
LIBDIR =
LIBS =

OBJS = base64_ssl.o RSACrypto.o

default: $(BIN)

$(BIN): $(OBJS)
	@echo "Linking $(BIN)"
	@ar cru $(BIN) $(OBJS) $(LIBS)
	@ranlib $(BIN)
	@chmod 700 $(BIN)

$(OBJS):
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c -o $@ $<

clean:
	@rm -f *.o
	@rm -f $(BIN)

dep:
	@touch Depend
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -MM *.cpp > Depend

sinclude Depend
