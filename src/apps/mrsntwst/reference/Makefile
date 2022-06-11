TARGET = mt19937ar
SOURCE = $(TARGET:=.c)

# Compiler
COMPILER = gcc
COMPILER_OUTPUT_OPTION = -o

# Source getter
GETTER = wget
URL = http://math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/CODES/$(SOURCE)

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(COMPILER) $^ $(COMPILER_OUTPUT_OPTION) $@

$(SOURCE):
	$(GETTER) $(URL)
	
