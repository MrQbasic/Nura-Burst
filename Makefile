#DIRS
SRC_DIR = ./src
BIN_DIR = ./bin
LIB_DIR = ./lib
BUILD_DIR = ./build
#COMPILER + FLAGS
GCC        = gcc
GCC_FLAGAS = -I $(LIB_DIR)/
#FILES
C_FILES = $(wildcard $(SRC_DIR)/*.c)
H_FILES = $(wildcard $(LIB_DIR)/*.h)
O_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_FILES))
EXE_FILE = $(BIN_DIR)/burst

#FINAL COMPILE / LINK
all: $(EXE_FILE)

$(EXE_FILE): $(O_FILES)
	$(GCC) $(GCC_FLAGAS) -lm -o $@ $^

#COMPILE C FILES
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(GCC) $(GCC_FLAGAS) -c -o $@ $^



#CLEAN
clean:
	rm -f $(O_FILES)
	rm -f $(EXE_FILE)
	clear

#RUN
run: clean $(EXE_FILE)
	rm -f output.csv
	touch output.csv
	$(EXE_FILE)