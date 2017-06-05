# -Wall == all warnings enabled
#  # -Werror == treat warnings as ERRORS!
client_TARGET = client
server_TARGET = server
CC = gcc
CUR_DIR = $(shell pwd)
CFLAGS = --std=c99 -Wall -Wextra -O0 -g -pthread
INCLUDES = -I"$(CUR_DIR)/src" -I"$(CUR_DIR)/tests"
LINKFLAGS = --std=c99
APPLFLAGS = -pthread -lm -lrt
TESTLFLAGS = `pkg-config --libs check` $(APPLFLAGS)

common_SOURCES = $(shell find src/ -name *.c -not -path "*/client/*" -not -path "*/server/*")

client_SOURCES = $(shell find src/client -name *.c) $(common_SOURCES)
client_OBJECTS = $(client_SOURCES:%.c=bin/%.o)

server_SOURCES = $(shell find src/server -name *.c) $(common_SOURCES)
server_OBJECTS = $(server_SOURCES:%.c=bin/%.o)

tests_SOURCES = $(shell find tests/ src/ -name *.c -not -name *main.c) tests/tests_main.c
tests_OBJECTS = $(tests_SOURCES:%.c=bin/%.o)
tests_EXECUTABLE = bin/tests_bin

DEPS := $(app_OBJECTS:.o=.d) $(tests_OBJECTS:.o=.d)

all: directories $(client_TARGET) $(server_TARGET) tests

-include $(DEPS)

directories:
	@mkdir -p bin

bin/%.d : %.c
	@$(CC) $(CFLAGS) -MM -MT '$(patsubst %.c,bin/%.o,$<)' $< -MF $@ $(INCLUDES) 2>/dev/null

bin/%.o : %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $< $(INCLUDES)

$(tests_EXECUTABLE): $(tests_OBJECTS)
	@mkdir -p $(@D)
	$(CC) -o $@ $(tests_OBJECTS) $(LINKFLAGS) $(TESTLFLAGS) $(INCLUDES)

tests: $(tests_EXECUTABLE)
	@./$(tests_EXECUTABLE)

run: $(TARGET)
	@./$(TARGET)

$(client_TARGET): $(client_OBJECTS)
	@mkdir -p $(@D)
	$(CC) -o $@ $(client_OBJECTS) $(LINKFLAGS) $(APPLFLAGS) $(INCLUDES)

$(server_TARGET): $(server_OBJECTS)
	@mkdir -p $(@D)
	$(CC) -o $@ $(server_OBJECTS) $(LINKFLAGS) $(APPLFLAGS) $(INCLUDES)

clean:
	@find . -name "*.o" -delete -or -name "*.d" -delete || true
	@find bin -type d -empty -delete || true

cleaner: clean
	@rm $(TARGET) $(tests_EXECUTABLE) || true

-include $(DEPS)

.PHONY: clean cleaner tests directories run
