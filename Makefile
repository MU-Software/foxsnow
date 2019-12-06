CC		:= gcc
C_FLAGS := -g -m32 -w #-Wall -Wextra

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= foxsnow.exe
else
EXECUTABLE	:= foxsnow
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	-$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.* $(SRC)/loader/*.* $(SRC)/datatype/*.* $(SRC)/python_support/*.* $(SRC)/GL_support/*.* $(SRC)/nuklear_support/*.*
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -lmingw32 -lSDL2main -lSDL2 -lglew32 -lglew32mx -lopengl32 -lPython37 -o $@ $(LIBRARIES)