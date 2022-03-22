COMPILER = cl
COMPILER_DONT_LINK_OPTION = /c
COMPILER_ENCODING_OPTION = /execution-charset:utf-8 /source-charset:utf-8
COMPILER_OUTPUT_OPTION = /Fo

LINKER = link
LINKER_OUTPUT_OPTION = /OUT:

TARGET = serialserver.exe
OBJECT = $(TARGET:.exe=.o)
SOURCE = $(OBJECT:.o=.c)

WINAPI_LIBS = "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" "ws2_32.lib"

all: $(TARGET)

clean:
	del $(TARGET) $(OBJECT)

rebuild: clean
	make

$(TARGET): $(OBJECT)
	$(LINKER) $^ $(WINAPI_LIBS) $(LINKER_OUTPUT_OPTION)$@

$(OBJECT): $(SOURCE)
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_ENCODING_OPTION) $(COMPILER_OUTPUT_OPTION)$@

