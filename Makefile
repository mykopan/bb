
CFLAGS = -O2 -Wall -Wextra -Ilib -Iinclude
CXXFLAGS = -std=c++17
LFLAGS = -Lbuild/lib -lstdc++

.PHONY: all
all: build/lib/libbb.a build/bin/test build/bin/tar

###############################################################################
# common

define bb_compile
$(2)/$(1).o: $(1)
	@mkdir -p $$(dir $$@)
	@echo "# compiling ( $$<, $$@ )"
	@$(CC) $(CFLAGS) $(CXXFLAGS) -c -o $$@ $$< -MMD -MF $$@.d
-include $(2)/$(1).o.d
endef

###############################################################################
# libbb.a

BB_ARCHIVE_INTER_DIR = build/inter/bb.archive
BB_ARCHIVE_SRCS = $(shell find lib -type f -name "*.cxx")
BB_ARCHIVE_OBJS = $(foreach src,$(BB_ARCHIVE_SRCS),$(BB_ARCHIVE_INTER_DIR)/$(src).o)

$(foreach src,$(BB_ARCHIVE_SRCS),$(eval $(call bb_compile,$(src),$(BB_ARCHIVE_INTER_DIR))))

build/lib/libbb.a: $(BB_ARCHIVE_OBJS)
	@mkdir -p $(dir $@)
	@echo "# archiving ( $@ )"
	@$(AR) -rc $@ $(BB_ARCHIVE_OBJS)

###############################################################################
# examples

BB_EXAMPLES = tar

BB_EXAMPLE_INTER_DIR = build/inter/$(1).prog
BB_EXAMPLE_SRCS = $(shell find examples/$(1) -type f -name "*.cxx")
BB_EXAMPLE_OBJS = $(foreach src,$(call BB_EXAMPLE_SRCS,$(1)),$(call BB_EXAMPLE_INTER_DIR,$(1))/$(src).o)

define bb_example
$(foreach src,$(call BB_EXAMPLE_SRCS,$(1)),$(eval $(call bb_compile,$(src),$(call BB_EXAMPLE_INTER_DIR,$(1)))))

build/bin/$(1): $(call BB_EXAMPLE_OBJS,$(1)) build/lib/libbb.a
	@mkdir -p $$(dir $$@)
	@echo "# linking ( $$@ )"
	@$(CC) $(CFLAGS) -Ilib $(CXXFLAGS) $(LFLAGS) -o $$@ $(call BB_EXAMPLE_OBJS,$(1)) -lbb
endef

$(foreach exmpl,$(BB_EXAMPLES),$(eval $(call bb_example,$(exmpl))))

###############################################################################
# test

BB_TEST_INTER_DIR = build/inter/test.prog
BB_TEST_SRCS = $(shell find test -type f -name "*.cxx")
BB_TEST_OBJS = $(foreach src,$(BB_TEST_SRCS),$(BB_TEST_INTER_DIR)/$(src).o)

$(foreach src,$(BB_TEST_SRCS),$(eval $(call bb_compile,$(src),$(BB_TEST_INTER_DIR))))

build/bin/test: $(BB_TEST_OBJS) build/lib/libbb.a
	@mkdir -p $(dir $@)
	@echo "# linking ( $@ )"
	@$(CC) $(CFLAGS) -Ilib $(CXXFLAGS) $(LFLAGS) -o $@ $(BB_TEST_OBJS) -lbb

