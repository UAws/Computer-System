# check that we are on a 64-bit OS
ifneq (64,$(shell getconf LONG_BIT))
    $(error CSTools will only run on a 64-bit Linux operating system)
endif

# look for a suitable compiler to use if CSCOMPILER is not already set
# if CSCOMPILER is set, just trust it
ifeq (x$(CSCOMPILER),x)
    Compiler=$(shell bash bin/Compiler-check.bash)
    ifeq (,$(Compiler))
        $(info Please set environment variable CSCOMPILER to the name of your C++ compiler.)
        $(error Cannot find a suitable C++ compiler.)
    else
        export CXX=$(Compiler)
    endif
else
    $(info CSCOMPILER is set to $(CXX))
    export CXX=$(CSCOMPILER)
endif

# set CS_ARCH for the OS are we running on
System:=$(shell test -f /usr/bin/uname && /usr/bin/uname -s)

# MacOS is reported as Darwin
ifeq (Darwin,$(System))
    export CS_ARCH=macos
    export MACOSX_DEPLOYMENT_TARGET=10.12
    export MACOS_ARCHS=$(shell bash bin/Compiler-archs.bash)
else
	THIS_OS:=$(shell test -f /etc/os-release && (grep "^ID=" /etc/os-release | sed 's/ID="\([^"]*\).*/\1/'))
	THIS_OSV:=$(shell test -f /etc/os-release && (grep "^VERSION_ID=" /etc/os-release | sed 's/VERSION_ID="\([^\."]*\).*/\1/'))

    # CATS is currently RHEL7.7
    ifeq (rhel7,$(THIS_OS)$(THIS_OSV))
        export CS_ARCH=cats

    # CentOS7 is close enough RHEL7
    else ifeq (centos7,$(THIS_OS)$(THIS_OSV))
        export CS_ARCH=cats

    # assume everything else is equiv to CentOS 8 for now
    else
        export CS_ARCH=centos8
    endif
endif

# C++ 17 just in case
CXXFLAGS=--std=c++17 -I. -Iincludes -Wall ${MACOS_ARCHS}

# capture goal1 then make the remainder PHONY and depend on donothing
goal1:=$(firstword $(MAKECMDGOALS))
unique_goals:=$(filter-out $(goal1),$(sort $(MAKECMDGOALS)))
ifneq ($(words $(unique_goals)),0)
    .PHONY: $(unique_goals)
    $(eval $(unique_goals):donothing ;@true)
endif

# default target is all
all: test 
	@true

# donothing
donothing:
	@true

live test Test: compiled
	@bash bin/run-tests.bash $(MAKECMDGOALS)

less Less regenerate show Show working: 
	@bash bin/run-tests.bash $(MAKECMDGOALS)

notest: compiled
	@true

clean:
	rm -f lib/*/[^bl]*

lib/$(CS_ARCH)/%: %.cpp lib/$(CS_ARCH)/libcs*.a
	${CXX} ${CXXFLAGS} -o $@ $^

lib/$(CS_ARCH)/%: %.cpp %-*.cpp lib/$(CS_ARCH)/libcs*.a
	${CXX} ${CXXFLAGS} -o $@ $^

compiled: lib/$(CS_ARCH)/parser
compiled: lib/$(CS_ARCH)/codegen
compiled: lib/$(CS_ARCH)/pretty
compiled: lib/$(CS_ARCH)/optimiser-e
compiled: lib/$(CS_ARCH)/optimiser-r
