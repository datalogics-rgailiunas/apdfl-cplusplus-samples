#  Makefile to build DL samples 

ifeq ($(PLATFORM), )
  PLATFORM = $(shell uname)
endif

do_pdfl_samples: build_pdfl_samples run_pdfl_samples
	$(REDIRECT2)

SAMPLES = AddAttachments

build_pdfl_samples:
	set -e; \
	export; \
	for target in $(SAMPLES); do \
		(cd ../$$target && gmake); \
	done;


# Library paths for running samples
ifeq ($(OS), i80386linux)
  ifeq ($(BUILD_64_BIT), true)
    LIBPATHS=../../Libs:/opt/gcc-4.1.2/lib64
  else
    LIBPATHS=../../Libs:/opt/gcc-4.1.2/lib
  endif
endif

ifeq ($(LIBPATHS),)
    LIBPATHS=../../Libs
endif

run_pdfl_samples:
	set -e; \
	export LD_LIBRARY_PATH=$(LIBPATHS):$$LD_LIBRARY_PATH; \
	export LIBPATH=$(LIBPATHS):$$LIBPATH; \
	export SHLIB_PATH=$(LIBPATHS):$$SHLIB_PATH; \
	export; \
	#cd ..;\
	for target in $(SAMPLES); do \
		echo "Running DL Sample $$target"; \
		(cd ../$$target && (./$$target $(REDIRECT))) ;\
		#cd $$target;./$$target $(REDIRECT) ;cd ..;\
		echo ; \
	done; \
	echo ; \
	#cd All

clean:
	rm -f *.o *.out out.* ../dlutils/*.o; \
	for target in $(SAMPLES); do \
		(cd ../$$target && gmake clean); \
	done;

