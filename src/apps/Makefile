APPDIRS = $(shell ls -d */ | sed -e "s/\///g")
APPS = $(APPDIRS:=.com)

all: $(APPS)

clean: $(APPDIRS:=.clean)
	rm -f *.com

rebuild: clean
	make

%.clean:
	make clean -C $(@:.clean=)

%.com:
	make -C $(@:.com=)
	cp $(@:.com=)/$@ .

