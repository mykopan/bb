
CFLAGS=-Wall -Wextra -Ilib -Iinclude
CXXFLAGS=-std=c++17

.PHONY: all
all: build/bb.a build/test

build:
	mkdir -p build

build/rules:
	mkdir -p build/rules

build/test: build/bb.a examples/test.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -o build/test examples/test.cxx build/bb.a -lstdc++

BB_OBJS= \
  build/args.cxx.o \
  build/command.cxx.o \
  build/log.cxx.o \
  build/options.cxx.o \
  build/rule.cxx.o \
  build/run.cxx.o \
	build/rules/start.cxx.o \
	build/rules/file.cxx.o

build/bb.a : $(BB_OBJS) | build
	$(AR) -rc build/bb.a $(BB_OBJS)

build/args.cxx.o : lib/args.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/args.cxx.o lib/args.cxx

build/command.cxx.o : lib/command.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/command.cxx.o lib/command.cxx

build/log.cxx.o : lib/log.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/log.cxx.o lib/log.cxx

build/options.cxx.o : lib/options.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/options.cxx.o lib/options.cxx

build/rule.cxx.o : lib/rule.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/rule.cxx.o lib/rule.cxx

build/run.cxx.o : lib/run.cxx | build
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/run.cxx.o lib/run.cxx

build/rules/start.cxx.o : lib/rules/start.cxx | build/rules
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/rules/start.cxx.o lib/rules/start.cxx

build/rules/file.cxx.o : lib/rules/file.cxx | build/rules
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o build/rules/file.cxx.o lib/rules/file.cxx

