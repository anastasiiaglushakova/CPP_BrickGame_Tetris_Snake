CXX := g++
CC  := gcc

CXXFLAGS := -std=c++20 -Wall -Wextra -fPIC \
	-Ibrick_game/snake \
	-Ibrick_game/tetris \
	-Igui/desktop \
	-Igui/cli \
	$(shell pkg-config --cflags-only-I Qt5Core Qt5Widgets | sed 's/-I/-isystem /g')

CFLAGS := -Wall -Wextra -Wunused-function -Wunused-variable -Wunused-parameter -Wunused-but-set-variable -fPIC \
	-Ibrick_game/snake \
	-Ibrick_game/tetris \
	-Igui/cli

LDFLAGS := $(shell pkg-config --libs Qt5Core Qt5Widgets)

BUILD_DIR := build
BIN_DIR := bin

ifeq ($(COVERAGE),1)
	COVERAGE_FLAGS := -fprofile-arcs -ftest-coverage
	COVERAGE_LINK  := --coverage
else
	COVERAGE_FLAGS :=
	COVERAGE_LINK  :=
endif

LOGIC_SRCS := \
	brick_game/snake/snake_game.cpp \
	brick_game/tetris/tetris_game.cpp

LOGIC_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(LOGIC_SRCS))

SNAKE_LOGIC_SRCS := brick_game/snake/snake_game.cpp
SNAKE_LOGIC_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SNAKE_LOGIC_SRCS))

API_SRCS := \
	brick_game/snake/snake_c_api.cpp \
	brick_game/tetris/tetris_c_api.c

API_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(API_SRCS))) \
            $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(API_SRCS)))

DESKTOP_SRCS := \
	gui/desktop/main.cpp \
	gui/desktop/mainwindow.cpp \
	gui/desktop/snake/snake_controller.cpp \
	gui/desktop/snake/snake_view.cpp \
	gui/desktop/tetris/tetris_controller.cpp \
	gui/desktop/tetris/tetris_view.cpp

DESKTOP_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(DESKTOP_SRCS))

CLI_SRCS := \
	gui/cli/main.c \
	gui/cli/snake/snake_view_cli.c \
	gui/cli/tetris/tetris_view_cli.c

CLI_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(CLI_SRCS))

MOC_HEADERS := \
	gui/desktop/mainwindow.hpp \
	gui/desktop/snake/snake_controller.hpp \
	gui/desktop/snake/snake_view.hpp \
	gui/desktop/tetris/tetris_controller.hpp \
	gui/desktop/tetris/tetris_view.hpp

MOC_SRCS := $(patsubst gui/desktop/%.hpp,$(BUILD_DIR)/moc_gui/desktop/moc_%.cpp,$(MOC_HEADERS))
MOC_OBJS := $(patsubst %.cpp,%.o,$(MOC_SRCS))

TEST_SRCS := $(wildcard tests/*.cpp)
TEST_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_TARGET := $(BUILD_DIR)/snake_tests

GTEST_FLAGS := -isystem /usr/include/gtest -isystem /usr/include/gmock
GTEST_LIBS := -lgtest -lgtest_main -pthread

DESKTOP_TARGET := $(BIN_DIR)/snake_tetris_desktop
CONSOLE_TARGET := $(BIN_DIR)/snake_tetris_console

.PHONY: all rebuild moc clean test coverage install uninstall dist dvi checkformat correctformat valgrind_test

all: $(DESKTOP_TARGET) $(CONSOLE_TARGET)

rebuild: clean all

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/moc_gui/desktop/moc_%.cpp: gui/desktop/%.hpp
	mkdir -p $(dir $@)
	moc $< -o $@

moc: $(MOC_SRCS)

$(DESKTOP_TARGET): $(DESKTOP_OBJS) $(MOC_OBJS) $(LOGIC_OBJS) $(API_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(COVERAGE_LINK)

$(CONSOLE_TARGET): $(CLI_OBJS) $(LOGIC_OBJS) $(API_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses $(COVERAGE_LINK)

TEST_CXXFLAGS := $(CXXFLAGS) $(COVERAGE_FLAGS) $(GTEST_FLAGS)
TEST_LDFLAGS := $(COVERAGE_LINK) $(GTEST_LIBS)

$(TEST_TARGET): $(TEST_OBJS) $(SNAKE_LOGIC_OBJS)
	$(CXX) $(TEST_CXXFLAGS) -o $@ $^ $(TEST_LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

valgrind_test: rebuild test
	valgrind -s --leak-check=full --show-leak-kinds=all ./$(TEST_TARGET) 2>&1 | tee $(BUILD_DIR)/valgrind_full.log

coverage:
	$(MAKE) clean
	$(MAKE) test COVERAGE=1
	@mkdir -p $(BUILD_DIR)/coverage_report
	lcov --capture --directory $(BUILD_DIR) --output-file $(BUILD_DIR)/coverage_report/coverage.info \
		--include '*/brick_game/snake/*'
	genhtml $(BUILD_DIR)/coverage_report/coverage.info --output-directory $(BUILD_DIR)/coverage_report/html
	@echo "Coverage report generated at $(BUILD_DIR)/coverage_report/html/index.html"

PREFIX ?= $(PWD)
INSTALL_DIR := $(PREFIX)/bins

install: all
	mkdir -p $(INSTALL_DIR)
	install -m 755 $(DESKTOP_TARGET) $(INSTALL_DIR)/
	install -m 755 $(CONSOLE_TARGET) $(INSTALL_DIR)/

uninstall:
	rm -f $(INSTALL_DIR)/$(notdir $(DESKTOP_TARGET))
	rm -f $(INSTALL_DIR)/$(notdir $(CONSOLE_TARGET))

DIST_FILE = $(BUILD_DIR)/BrickGame_v2.0.tar.gz

dist:
	tar czvf $(DIST_FILE) \
		Makefile \
		brick_game \
		diagrams \
		gui \
		tests

clean:
	rm -rf $(BUILD_DIR)
	
dvi:
	@echo "BrickGame v2.0"
	@echo "Makefile location: $(CURDIR)"
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Install directory: $(INSTALL_DIR)"
	@echo
	@echo "Available targets:"
	@echo "  all             - build all binaries"
	@echo "  rebuild         - clean and build"
	@echo "  install         - install binaries to $(INSTALL_DIR)"
	@echo "                    (use PREFIX=/path to override)"
	@echo "  uninstall       - remove installed binaries"
	@echo "  clean           - remove build files"
	@echo "  test            - run unit tests"
	@echo "  valgrind_test   - run tests under valgrind"
	@echo "  coverage        - generate code coverage report"
	@echo "  moc             - generate Qt moc files"
	@echo "  dist            - create source tarball"
	@echo "  checkformat     - check code formatting with clang-format"
	@echo "  correctformat   - auto-format code with clang-format"
	@echo "  deleteall   - remove build files, binaries and installed binaries"

checkformat:
	clang-format -n -style=Google $$(find . -type f \( -name '*.cpp' -o -name '*.h' \))

correctformat:
	clang-format -i -style=Google $$(find . -type f \( -name '*.cpp' -o -name '*.h' \))

deleteall:
	rm -rf $(BUILD_DIR)
	rm -rf $(INSTALL_DIR)
	rm -rf $(BIN_DIR)