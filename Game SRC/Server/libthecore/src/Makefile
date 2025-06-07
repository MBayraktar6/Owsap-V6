CC = clang++15
CFLAGS = -m32 -Wall -O2 -pipe -g
CXXFLAGS = -std=c++17 -Wno-invalid-source-encoding -Wno-deprecated

BINDIR = ../lib
BIN = $(BINDIR)/libthecore.a

INCDIR = -I../include
LIBDIR =
LIBS =

OBJS = socket.o fdwatch.o buffer.o signal.o log.o utils.o \
	kstbl.o hangul.o heart.o main.o tea.o des.o gost.o memcpy.o

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
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -MM *.c > Depend

memcpy: memcpy.o utils.o log.o
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c -D__MAIN__ memcpy.c
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -o memcpy memcpy.o utils.o log.o

sinclude Depend
