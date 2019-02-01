config = redist/makefile

all: $(config)
	$(MAKE) -C redist all

install: $(config)
	$(MAKE) -C redist install

clean: $(config)
	$(MAKE) -C redist clean

$(config): redist/makefile.in
	@ echo "WARNING: You haven't run configure yet. Running default configure ..."
	@ cd redist && ./configure && cd -

