CC = clang++15
CFLAGS = -m32 -Wall -O2 -pipe -fno-exceptions
CXXFLAGS = -std=c++17

BINDIR = ../lib
BIN = $(BINDIR)/libgame.a

INCDIR = -I../include
LIBDIR =
LIBS =

OBJS = grid.o attribute.o targa.o

default: $(BIN)

$(BIN): $(OBJS)
	@echo "Linking $(BIN)"
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
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
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -MM *.cc > Depend

sinclude Depend
