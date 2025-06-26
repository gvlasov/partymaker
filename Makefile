CC = gcc
CFLAGS = -O2 -Wall -lm -lasound
BUILD_DIR = build
TARGETS = tone attack djent noise brown
SOURCES = $(addsuffix .c, $(TARGETS))
BINARIES = $(addprefix $(BUILD_DIR)/, $(TARGETS))

all: $(BUILD_DIR) $(BINARIES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/tone: tone.c
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/attack: attack.c
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/djent: djent.c
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/noise: noise.c
	$(CC) $(CFLAGS) -lfftw3 -o $@ $^

$(BUILD_DIR)/brown: brown.c
	$(CC) $(CFLAGS) -lfftw3 -o $@ $^

clean:
	rm -rf $(BUILD_DIR)
