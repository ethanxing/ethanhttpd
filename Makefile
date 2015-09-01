SOURCE_LIST := read_conf/read_conf.cpp  \
		main/test.cpp \
		mylog/mylog.cpp
all:

PROJECT_DIR := .
SHELL := /bin/bash
CXX := g++

OBJECT_NAME = $(patsubst %.cpp,%.o,$(1))
MAKE_TARGET_DIR = [[ -d $(dir $1) ]] || mkdir -p $(dir $1)

MODE := release
ifeq ($(MAKECMDGOALS), debug)
MODE := debug
endif

OBJ_DIR := $(PROJECT_DIR)/binary/$(MODE)
OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(call OBJECT_NAME, $(SOURCE_LIST)))
DEPENDENCY_LIST = $(patsubst %.o,%.d,$(OBJ_LIST))
SUB_LIST := $(shell echo $(SOURCE_LIST) \
		| tr ' ' '\n' \
		| cut -f1 -d/ \
		| sort | uniq) 

COMMON_FLAGS := -Wall -W -Wextra -Wno-sign-compare \
		-g -m64 \
		$(addprefix -I$(PROJECT_DIR)/, $(SUB_LIST)) 
LDFLAGS :=
debug_FLAGS := -O0 -DETHAN_DEBUG
release_FLAGS := -O2
CXXFLAGS := $(COMMON_FLAGS) $($(MODE)_FLAGS)

OUTPUT_DIR := $(PROJECT_DIR)/output
EXECUABLE_DIR := $(OUTPUT_DIR)/bin

ifeq ($(MODE), debug)
EXEC_NAME := ehttpd_debug
else
EXEC_NAME := ehttpd
endif

ifeq ($(subst clean,,$(MAKECMDGOALS)), $(MAKECMDGOALS))
    -include $(DEPENDENCY_LIST)
endif

MAIN_EXEC := $(EXECUABLE_DIR)/$(EXEC_NAME)

all: $(MAIN_EXEC)
release debug: all

$(MAIN_EXEC):$(OBJ_LIST)
	$(call MAKE_TARGET_DIR, $@)
	$(CXX) -o $@ $^ $(LDFLAGS)
	cp -rf $(PROJECT_DIR)/conf $(OUTPUT_DIR)
$(OBJ_DIR)/%.o: $(PROJECT_DIR)/%.cpp
	echo $(COMMON_FLAGS)
	$(call MAKE_TARGET_DIR, $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(OBJ_DIR)/%.d: $(PROJECT_DIR)/%.cpp
	$(call MAKE_TARGET_DIR, $@)
	$(CXX) $(CXXFLAGS) -MM -MT $(patsubst %d,%o,$@) $< -MF $@

.PHONY:clean
clean:
	-rm -rf $(OUTPUT_DIR) $(PROJECT_DIR)/binary
