APPDIRS = $(shell ls -d */ | sed -e "s/\///g")
APPS = $(APPDIRS:=.elf)

all: $(APPS)

clean: $(APPDIRS:=.clean)

rebuild: clean
	make

%.clean:
	make clean -C $(@:.clean=)

%.elf:
	make -C $(@:.elf=)
	cp $(@:.elf=)/$@ .

