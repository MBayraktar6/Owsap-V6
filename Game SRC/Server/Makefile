PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)
BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)

default: libthecore libpoly libgame liblua libsql libserverkey db game
	@echo "--------------------------------------"
	@echo "Build Done"
	@echo "--------------------------------------"

libserverkey: .
	@touch $@/Depend
	$(MAKE) -C $@ dep
	$(MAKE) -C $@ clean
	$(MAKE) -C $@

liblua: .
	$(MAKE) -C $@ clean
	$(MAKE) -C $@

libsql: .
	@touch $@/Depend
	$(MAKE) -C $@ dep
	$(MAKE) -C $@ clean
	$(MAKE) -C $@

libgame: .
	@touch $@/src/Depend
	$(MAKE) -C $@/src dep
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src

libpoly: .
	@touch $@/Depend
	$(MAKE) -C $@ dep
	$(MAKE) -C $@ clean
	$(MAKE) -C $@ 

libthecore: .
	@touch $@/src/Depend
	$(MAKE) -C $@/src dep
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src

db: .
	@touch $@/src/Depend
	$(MAKE) -C $@/src dep
	$(MAKE) -C $@/src clean
	#$(MAKE) -C $@/src

game: .
	@touch $@/src/Depend
	$(MAKE) -C $@/src dep
	$(MAKE) -C $@/src clean
	#$(MAKE) -C $@/src

clean:
	$(MAKE) -C liblua clean
	$(MAKE) -C libsql clean
	$(MAKE) -C libgame/src clean
	$(MAKE) -C libpoly clean
	$(MAKE) -C libthecore/src clean
	$(MAKE) -C libserverkey clean
	$(MAKE) -C db/src clean
	$(MAKE) -C game/src clean

libs: 
	@echo "--------------------------------------"
	@echo "Library Build Start"
	@echo "--------------------------------------"

	$(MAKE) -C liblua clean
	$(MAKE) -C liblua

	@touch libsql/Depend
	$(MAKE) -C libsql dep
	$(MAKE) -C libsql clean
	$(MAKE) -C libsql

	@touch libgame/src/Depend
	$(MAKE) -C libgame/src dep
	$(MAKE) -C libgame/src clean
	$(MAKE) -C libgame/src

	@touch libpoly/Depend
	$(MAKE) -C libpoly dep
	$(MAKE) -C libpoly clean
	$(MAKE) -C libpoly

	@touch libthecore/src/Depend
	$(MAKE) -C libthecore/src dep
	$(MAKE) -C libthecore/src clean
	$(MAKE) -C libthecore/src

	@touch libserverkey/Depend
	$(MAKE) -C libserverkey dep
	$(MAKE) -C libserverkey clean
	$(MAKE) -C libserverkey

	@echo "--------------------------------------"
	@echo "Full Library Build End"
	@echo "--------------------------------------"

all: 
	@echo "--------------------------------------"
	@echo "Full Build Start"
	@echo "--------------------------------------"

	$(MAKE) -C liblua clean
	$(MAKE) -C liblua

	@touch libsql/Depend
	$(MAKE) -C libsql dep
	$(MAKE) -C libsql clean
	$(MAKE) -C libsql

	@touch libgame/src/Depend
	$(MAKE) -C libgame/src dep
	$(MAKE) -C libgame/src clean
	$(MAKE) -C libgame/src

	@touch libpoly/Depend
	$(MAKE) -C libpoly dep
	$(MAKE) -C libpoly clean
	$(MAKE) -C libpoly

	@touch libthecore/src/Depend
	$(MAKE) -C libthecore/src dep
	$(MAKE) -C libthecore/src clean
	$(MAKE) -C libthecore/src

	@touch libserverkey/Depend
	$(MAKE) -C libserverkey dep
	$(MAKE) -C libserverkey clean
	$(MAKE) -C libserverkey

	@touch db/src/Depend
	$(MAKE) -C db/src dep
	$(MAKE) -C db/src clean
	$(MAKE) -C db/src

	@touch game/src/Depend
	$(MAKE) -C game/src dep
	$(MAKE) -C game/src clean
	$(MAKE) -C game/src

	@echo "--------------------------------------"
	@echo "Full Build End"
	@echo "--------------------------------------"
