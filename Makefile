# The simplest way to compile TouchVG on MinGW, Cygwin, Linux or Mac OS X is:
#
# 1. `cd' to the directory containing the file of 'Makefile'.
#
# 2. Type `make and' to compile the swig projects for Android.
#    The program binaries files are outputed to './build/java'.
# 
# 3. You can remove the program object files from the source code
#    directory by typing `make java.clean'.
#
# Readme about variables: https://github.com/rhcad/x3py/wiki/MakeVars
#

SUBDIRS         =$(subst /,,$(dir $(wildcard */)))
CLEANDIRS       =$(addsuffix .clean, $(SUBDIRS))
INSTALLDIRS     =$(addsuffix .install, $(SUBDIRS))
SWIGS           =python perl5 java csharp ruby php lua r
CLEANSWIGS      =$(addsuffix .clean, $(SWIGS))
CLEANALLSWIGS   =$(addsuffix .cleanall, $(SWIGS))

.PHONY:     $(SUBDIRS) $(SWIGS) clean install touch and
all:        $(SUBDIRS)
clean:      $(CLEANDIRS)
install:    $(INSTALLDIRS)

$(SUBDIRS):
	@! test -e $@/Makefile || $(MAKE) -C $@

$(CLEANDIRS) $(INSTALLDIRS):
	@ ! test -e $(basename $@)/Makefile || \
	$(MAKE) -C $(basename $@) $(subst .,,$(suffix $@))

$(SWIGS):
	@test -d build || mkdir build
	@export SWIG_TYPE=$@; $(MAKE) -C core swig

$(CLEANSWIGS):
	@export SWIG_TYPE=$(basename $@); \
	export clean=1; $(MAKE) -C core clean

$(CLEANALLSWIGS):
	@export SWIG_TYPE=$(basename $@); export cleanall=1; \
	export clean=1; $(MAKE) -C core clean

touch:
	@export touch=1; $(MAKE) clean

and:
	@test -d build || mkdir build
	@export SWIG_TYPE=java; export clean=1; $(MAKE) -C core/src/skiaview -f Makefile.swig swigonly
	@test -d android/hello/libs || mkdir android/hello/libs
	@cp -v build/java/skiaview.jar android/hello/libs
	@cd android; python makecc.py
	ndk-build -C android/hello/jni
