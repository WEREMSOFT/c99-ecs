SRC_F := $(shell find src -name *.c) $(shell find libs -name *.c)
OBJ_FOR_CLEAN_F := $(shell find ./src -name *.o)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))

LIBS := 

FLAGS_DEBUG := -g -O0 -w
FLAGS__DEBUG := -O3
FLAGS := -Wall -Wextra -Ilibs/include

TARGET := bin/main.bin
.PONY: clean web statistics

all: clean $(SRC_O) $(SRC_CPP_O)
	gcc $(FLAGS_DEBUG) $(FLAGS) $(SRC_O) $(SRC_CPP_O) -o $(TARGET) $(LIBS)

run: all
	$(TARGET)

%.o: %.c
	gcc -c $(FLAGS_DEBUG) $(FLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_FOR_CLEAN_F)
	rm -rf $(TARGET)
	rm -rf bin/assets

	