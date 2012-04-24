JOBS=4
GMP=yes
WARD=../ward
SCONS=bin/scons

all opt: $(WARD)/bin/ward
	$(SCONS) -j $(JOBS) debug=0 ward=$(WARD) gmp=$(GMP)

debug: $(WARD)
	$(SCONS) -j $(JOBS) debug=1 ward=$(WARD) gmp=$(GMP) debugguards=1

opt32 32: $(WARD)
	$(SCONS) -j $(JOBS) debug=0 abi=32 ward=$(WARD) gmp=$(GMP)

debug32: $(WARD)
	$(SCONS) -j $(JOBS) debug=1 abi=32 ward=$(WARD) gmp=$(GMP) debugguards=1

opt64 64: $(WARD)
	$(SCONS) -j $(JOBS) debug=0 abi=64 ward=$(WARD) gmp=$(GMP)

debug64: $(WARD)
	$(SCONS) -j $(JOBS) debug=1 abi=64 ward=$(WARD) gmp=$(GMP) debugguards=1

clean:
	$(SCONS) -c preprocess=dummy

distclean:
	$(SCONS) -c preprocess=dummy; rm -rf extern/lib/* extern/include/* extern/32bit extern/64bit bin/current/*

$(WARD)/bin/ward:
	@echo "Building Ward."
	@cd ward; sh build.sh >/dev/null 2>/dev/null
	@echo "Ward build completed."

