CC = clang++15
CFLAGS = -m32 -Wall -O2 -pipe -fno-exceptions -fno-rtti
CXXFLAGS = -std=c++17

BINDIR = .
BIN = $(BINDIR)/libpoly.a

INCDIR =
LIBDIR =
LIBS =

OBJS = Base.o Poly.o SymTable.o Symbol.o

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
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -MM *.cc > Depend

sinclude Depend
