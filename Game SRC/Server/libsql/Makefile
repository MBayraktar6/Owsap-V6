CC = /usr/local/llvm15/bin/clang++
CFLAGS = -m32 -Wall -O2 -pipe -D_THREAD_SAFE -fno-exceptions
CXXFLAGS = -std=c++17

BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)
PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)

BINDIR = .
BIN = $(BINDIR)/libsql.a

ifeq ($(BSD_VERSION), 14)
INCDIR = -I../libmysql/8.0.33/14.0
else ifeq ($(BSD_VERSION), 13)
INCDIR = -I../libmysql/8.0.33/13.0
else
INCDIR = -I../libmysql/8.0.33
endif
LIBDIR =
LIBS =

OBJS = AsyncSQL.o Semaphore.o Tellwait.o Statement.o

default: $(BIN)

$(BIN): $(OBJS)
	@echo "Linking $(BIN)"
	@ar cru $(BIN) $(OBJS) $(LIBS)
	@ranlib $(BIN)
	@chmod 700 $(BIN)

AsyncSQL.o: AsyncSQL.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c -o $@ $<

Semaphore.o: Semaphore.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c -o $@ $<

Tellwait.o: Tellwait.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c -o $@ $<

Statement.o: Statement.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c -o $@ $<

clean:
	@rm -f *.o
	@rm -f $(BIN)

dep:
	@touch Depend
	$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -MM *.cpp > Depend

sinclude Depend