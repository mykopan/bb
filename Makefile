
CFLAGS = -Wall -Wextra -Ilib -Iinclude
CXXFLAGS = -std=c++17
LFLAGS = -Lbuild/lib -lstdc++

.PHONY: all
all: build/lib/libbb.a build/bin/test

build:
	@mkdir -p build

build/bin: | build
	@mkdir -p build/bin

build/lib: | build
	@mkdir -p build/lib

build/inter: | build
	@mkdir -p build/inter

###############################################################################
# generic

define bb_compile
$(2)/$(1).o: $(1) | $(dir $(2)/$(1))
	@echo "# compiling ( $$<, $$@ )"
	@$(CC) $(CFLAGS) $(CXXFLAGS) -c -o $$@ $$< -MMD -MF $$@.d
-include $(2)/$(1).o.d
endef

###############################################################################
# libbb.a

BB_ARCHIVE_INTER_DIR = build/inter/bb.archive
BB_ARCHIVE_SRCS = $(shell find lib -type f -name "*.cxx")
BB_ARCHIVE_OBJS = $(foreach src,$(BB_ARCHIVE_SRCS),$(BB_ARCHIVE_INTER_DIR)/$(src).o)

$(BB_ARCHIVE_INTER_DIR): | $(dir $(BB_ARCHIVE_INTER_DIR))
	@mkdir -p $@

$(BB_ARCHIVE_INTER_DIR)/lib: | $(BB_ARCHIVE_INTER_DIR)
	@mkdir -p $@

$(BB_ARCHIVE_INTER_DIR)/lib/rules: | $(BB_ARCHIVE_INTER_DIR)/lib
	@mkdir -p $@

$(foreach src,$(BB_ARCHIVE_SRCS),$(eval $(call bb_compile,$(src),$(BB_ARCHIVE_INTER_DIR))))

build/lib/libbb.a: $(BB_ARCHIVE_OBJS) | build/lib
	@echo "# archiving ( $@ )"
	@$(AR) -rc $@ $(BB_ARCHIVE_OBJS)

###############################################################################
# test

BB_TEST_INTER_DIR = build/inter/test.prog
BB_TEST_SRCS = $(shell find test -type f -name "*.cxx")
BB_TEST_OBJS = $(foreach src,$(BB_TEST_SRCS),$(BB_TEST_INTER_DIR)/$(src).o)

$(BB_TEST_INTER_DIR): | $(dir $(BB_TEST_INTER_DIR))
	@mkdir -p $(BB_TEST_INTER_DIR)

$(BB_TEST_INTER_DIR)/test: | $(BB_TEST_INTER_DIR)
	@mkdir -p $(BB_TEST_INTER_DIR)/test

$(foreach src,$(BB_TEST_SRCS),$(eval $(call bb_compile,$(src),$(BB_TEST_INTER_DIR))))

build/bin/test: $(BB_TEST_OBJS) build/lib/libbb.a | build/bin
	@echo "# linking ( $@ )"
	@$(CC) $(CFLAGS) $(CXXFLAGS) $(LFLAGS) -o $@ $(BB_TEST_OBJS) -lbb

