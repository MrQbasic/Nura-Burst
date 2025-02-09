#DIRS
SRC_DIR = ./src
BIN_DIR = ./bin
LIB_DIR = ./lib
BUILD_DIR = ./build
#COMPILER + FLAGS
GPP        = g++
GPP_FLAGAS = -lglfw -lGL -I $(LIB_DIR)/
#FILES
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
HPP_FILES = $(wildcard $(LIB_DIR)/*.hpp)
O_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(C_FILES))
EXE_FILE = $(BIN_DIR)/burst

#FINAL COMPILE / LINK
all: $(EXE_FILE)

$(EXE_FILE): $(O_FILES)
	$(GPP) $(GCC_FLAGAS) -lm -o $@ $^

#COMPILE C FILES
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(GPP) $(GPP_FLAGAS) -c -o $@ $^



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