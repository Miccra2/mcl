CC := cc
LD := ld

CCFLAGS :=
LDFLAGS := -lc

PWD := $(shell pwd)
SRCDIR := $(PWD)/src
BUILDDIR := $(PWD)/build
TARGET := $(PWD)/mcl

CSOURCES := $(wildcard $(SRCDIR)/c/*.c)
CSOURCES := $(filter-out $(SRCDIR)/interpreter.c,$(CSOURCES))
$(info CSOURCES=$(CSOURCES))

CPPSOURCES := $(wildcard $(SRCDIR)/cpp/*.cpp)
$(info CPPSOURCES=$(CPPSOURCES))

OBJECTS := $(CSOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
$(info OBJECTS=$(OBJECTS))

all: rebuild
rebuild: clean new $(TARGET)

.SECONDEXPANSION:

$(OBJECTS) : $$(patsubst $(BUILDDIR)/%.o,$(SRCDIR)/%.c,$$@)
	$(CC) -g -c -o $@ $(CCFLAGS) $<

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -L $(LDFLAGS) -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)

run: all
	./mcl

test: all
	valgrind ./mcl

debug: all
	gdb --batch --command=./debug/test.gdb --args ./mcl

new:
	mkdir -p $(BUILDDIR)/c $(BUILDDIR)/cpp
