PATH=$(shell pwd)
FILENAME=mcl
FILE=$(PATH)/$(FILENAME)
TESTFILE=$(PATH)/test.mcs
MAINFILE=$(PATH)/main.c

all: run
test: mem_test

clean:
ifeq ($(shell ls -al | grep -w "\$(FILENAME)\b" | echo $$?),1)
	echo removed mcl
	rm mcl
endif

mcl: clean
	cc -g -O3 $(MAINFILE) -o $(FILE) -I.

run: mcl
	$(FILE) $(TESTFILE)

mem_test: mcl
	valgrind $(FILE) $(TESTFILE)

dbg: mcl
	gdb $(FILE)
