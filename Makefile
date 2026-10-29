BUILD_DIR ?= build
BUILD_TYPE ?= Debug
CMAKE_FLAGS ?= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DASYNC_LOGGER_BUILD_EXAMPLES=ON

.PHONY: all build run clean install

all: build

$(BUILD_DIR)/Makefile:
	cmake -B $(BUILD_DIR) -S . $(CMAKE_FLAGS)

build: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) -j

run: build
	./$(BUILD_DIR)/async_logger_demo

clean:
	rm -rf $(BUILD_DIR)

install: build
	cmake --install $(BUILD_DIR)