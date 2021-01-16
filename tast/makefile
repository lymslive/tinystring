CXX = g++

TAST_CPP = $(wildcard *.cpp)
TAST_OUT = $(patsubst %.cpp, %.out, $(TAST_CPP))
TAST_EXE = $(patsubst %.cpp, %.exe, $(TAST_CPP))
TAST_COUT = $(patsubst %.cpp, %.cout, $(TAST_CPP))
TAST_BOUT = $(patsubst %.cpp, %.bout, $(TAST_CPP))

ifeq ($(tee), 1)
    TEE = | tee
else
    TEE = >
endif

ifeq ($(gdb), 1)
    CXXFLAG += -g
endif

.PHONY : cout bout clean echo

cout : $(TAST_COUT)
bout : $(TAST_BOUT)

%.out : %.cpp
	$(CXX) -o $@ $(CXXFLAG) $<

%.cout : %.out
	./$< $(TEE) $@
	diff $@ $(patsubst %.cout, %.bout, $@)

%.bout : %.cout
	cp $< $@

clean :
	rm -rf $(TAST_COUT)

echo:
	@echo TAST_CPP:  $(TAST_CPP)
	@echo TAST_OUT:  $(TAST_OUT)
	@echo TAST_COUT: $(TAST_COUT)
	@echo TAST_BOUT: $(TAST_BOUT)
	@echo CXXFLAG: $(CXXFLAG)
	@echo TEE: "$(TEE)"

