BITMAP = bitmap.txt
COMPILER = gcc
COMPILER_OUTPUT_OPTION = -o
COMPILER_WARNING_OPTION = -Wall -Wextra
OUTPUT = fontdata.c
TRANSLATOR = ./translator

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TRANSLATOR)

rebuild: clean
	make

$(OUTPUT): $(BITMAP) $(TRANSLATOR)
	$(TRANSLATOR) $(BITMAP) $@

$(TRANSLATOR): $(addsuffix .c, $(TRANSLATOR))
	$(COMPILER) $^ $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

