TARGET_EXEC ?= border

# Pick the compiler with e.g. `make release CXX=clang++` (defaults to g++).
# Each compiler gets its own build directory so objects never mix.
BUILD_ROOT ?= ./build
BUILD_DIR ?= $(BUILD_ROOT)/$(notdir $(firstword $(CXX)))
SRC_DIRS ?= src/
LDFLAGS = -std=c++17 -lsfml-graphics -lsfml-system

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
#OBJS := $(SRCS:%=%.o)
#DEPS := $(OBJS:.o=.d)

DEPS = $(shell find $(BUILD_DIR) -name *.d 2>/dev/null)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -std=c++17 -MMD -MD -MP -Wall

#
# Debug build settings
#
DBGDIR := $(BUILD_DIR)/debug
DBGEXE := $(DBGDIR)/$(TARGET_EXEC)
DBGOBJS := $(SRCS:%=$(DBGDIR)/%.o)
DBGCFLAGS := $(CPPFLAGS) -g -O0 -ftrapv -Wall -Wextra -DDEBUG

#
# Release build settings
#
RELDIR := $(BUILD_DIR)/release
RELEXE := $(RELDIR)/$(TARGET_EXEC)
RELOBJS := $(SRCS:%=$(RELDIR)/%.o)
RELCFLAGS := $(CPPFLAGS) -O3 -Ofast -ffast-math -DRELEASE -DNDEBUG

.PHONY: clean release debug run rund all remake

all: release debug
remake: clean all

release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(RELOBJS) -o $@ $(LDFLAGS)

$(RELDIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(RELCFLAGS) -c $< -o $@


debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(DBGOBJS) -o $@ $(LDFLAGS)

$(DBGDIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(DBGCFLAGS) -c $< -o $@


clean:
	$(RM) -r $(BUILD_ROOT)

run:
	$(RELEXE)

rund:
	$(DBGEXE)

test:
	valgrind --leak-check=full $(RELEXE)

-include $(DEPS)

MKDIR_P ?= mkdir -p
