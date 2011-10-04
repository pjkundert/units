# 
# UNITS		-- Unit Conversion Framework
# 

VERSION		= 4.0.1
ARCH		= $(shell uname -m | sed -e s/i.86/i386/ -e s/9000.*/hppa/)
#DEBUG		= yes				# compile w/ DEBUG=yes make test
TARGET		= /usr/local/include

# We want to deduce these, unless they are explicitly set...
ifeq ($(origin CC),default)
    CC		=
endif
ifeq ($(origin CXX),default)
    CXX		=
endif

CXXFLAGS	+= -I.
ifneq ($(DEBUG),)
	CXXFLAGS += -DDEBUG
endif

# Default flags for building test executables.  Not required by
# default (only units.H is required by client programs)!  Figure out
# deguggable vs. optimized,
ifeq ($(ARCH),i386)
    ifneq ($(DEBUG),)
	CXXFLAGS += -g
    endif
    ifneq ($(OPT),)
	CXXFLAGS	+= -O$(OPT)
    else
	CXXFLAGS	+= -O3
    endif

    ifeq ($(CXX),icc)
        CXXFLAGS += -Wall
    else
	ifeq ($(CXX),)
	    CXX	 = g++
	endif
        CXXFLAGS += -pipe 				\
		    -W -Wall -Wwrite-strings		 \
		    -Wcast-align -Wpointer-arith	  \
		    -Wcast-qual -Wfloat-equal		   \
		    -Wnon-virtual-dtor -Woverloaded-virtual \
		    -Wsign-promo -Wshadow
    endif
endif
ifeq ($(ARCH),sun4v)
    ifeq ($(CXX),)
        CXX	 = CC
    endif
    CXXFLAGS 		+= +w
    ifneq ($(DEBUG),)
	CXXFLAGS	+= -g
    endif

    ifneq ($(OPT),)
	CXXFLAGS	+= -xO$(OPT)
    else
	CXXFLAGS	+= -xO4
    endif
endif
ifeq ($(ARCH),x86_64)
    ifneq ($(DEBUG),)
	CXXFLAGS += -g
    else
        CXXFLAGS += -O3
    endif

    ifeq ($(CXX),icc)
        CXXFLAGS += -Wall
    else
        CXX	 = g++
        CXXFLAGS += -pipe 				\
		    -W -Wall -Wwrite-strings		 \
		    -Wcast-align -Wpointer-arith	  \
		    -Wcast-qual -Wfloat-equal		   \
		    -Wnon-virtual-dtor -Woverloaded-virtual \
		    -Wsign-promo -Wshadow
    endif
endif
ifeq ($(ARCH),hppa)
    ifneq ($(DEBUG),)
	CXXFLAGS	+= -g
    else
        CXXFLAGS	+= +O3
    endif

    CXX		= aCC
    CXXFLAGS   += -AA +p		\
		  +DAportable		 \
		  -D_HPUX_SOURCE	  \
		  -D_POSIX_C_SOURCE=199506L\
		  -D_RWSTD_MULTI_THREAD
    CXXFLAGS   += -mt
endif

CXXFLAGS	+= -I../cut/

# 
# Basic Targets
# 
#     The default target 'all' builds (optional) unit test object, and
# source distribution package.  Neither of these are necessary to use
# UNITS.  The units-test.o object may optionally be linked into
# applications, to include UNITS' CUT unit tests.
# 
.PHONY: all configure test testboost install dist distclean clean

all:			units-test.o

configure:

test:			configure units-test units-test-disabled
	time ./units-test
	time ./units-test-disabled

install:
	cp units units.H $(TARGET)

dist:			distclean units-$(VERSION).tgz

distclean:		clean

clean:
	-rm -f  *.tgz 				\
		*.html 				 \
		*~ 	*/*~ 			  \
		*.o 	*/*.o 			   \
		core* 	*/core*			    \
		units-test units-test-disabled

# 
# Unit Tests
# 
headers		= 	units			\
			units.H

units-test.o:		units-test.C $(headers)
	$(CXX) $(CXXFLAGS) -c -DTEST units-test.C -o $@

units-test: 		$(headers) units-test.C
	$(CXX) $(CXXFLAGS) -DTESTSTANDALONE -DTEST units-test.C -o $@

units-test-disabled:	$(headers) units-test.C
	$(CXX) $(CXXFLAGS) -DTESTSTANDALONE -DTEST -DUNITS_DISABLED units-test.C -o $@


# 
# Force generation of HTML unit test output, by indicated to CUT that
# it is executing in a CGI environment (REQUEST_METHOD=...).
# 
test-html:		units-test.html

units-test.html:	units-test
	REQUEST_METHOD=true ./units-test > units-test.html

# 
# Distribution
# 
units-$(VERSION).tgz: 	configure		\
			GNUmakefile		 \
			units			  \
			units.H			   \
			COPYING			    \
			README			     \
	 		INSTALL			      \
			units-test.C		       \
			units_t.h
	rm -f   units-$(VERSION)
	ln -s . units-$(VERSION)
	tar czf $@ $(addprefix units-$(VERSION)/,$^)
	rm -f   units-$(VERSION)
