TARGET = mt19937ar.txt
PROGRAM = $(TARGET:.txt=)
SOURCE = $(PROGRAM:=.c)

# Compiler
COMPILER = gcc
COMPILER_OUTPUT_OPTION = -o

# Source getter
GETTER = wget
URL = http://math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/CODES/$(SOURCE)

all: $(TARGET)

clean:
	rm -f $(TARGET) $(PROGRAM) $(SOURCE)

rebuild: clean
	make

$(TARGET): $(PROGRAM)
	./$(PROGRAM) > $(TARGET)

$(PROGRAM): $(SOURCE)
	$(COMPILER) $^ $(COMPILER_OUTPUT_OPTION) $@

$(SOURCE):
	$(GETTER) $(URL)
	
