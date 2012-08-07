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

AndJarPath  =._java
AndJavaPath =$(AndJarPath)/touchvg/skiaview

and:
	@cd android/skiaview/jni; \
	    test -d $(AndJarPath) || mkdir $(AndJarPath); \
	    test -d $(AndJarPath)/touchvg || mkdir $(AndJarPath)/touchvg; \
	    test -d $(AndJavaPath) || mkdir -v $(AndJavaPath); \
	    rm -rf $(AndJarPath)/*.* $(AndJavaPath)/*.*; \
	"$(SWIG_BIN)swig" -c++ -java -package touchvg.skiaview \
	    -outdir $(AndJavaPath) -o skiaview_java_wrap.cpp \
	    -I../../../core/include/geom -I../../../core/include/graph \
	    -I../../../core/include/shape -I../../../core/include/skiaview \
	    -I../../../android/skiaview/jni -I"$(JAVA_INCLUDE)" skiaview.swig;
	@cd android/skiaview/jni/$(AndJavaPath); "javac" *.java;
	@cd android/skiaview/jni/$(AndJarPath); "jar" cfv skiaview.jar touchvg/skiaview/*.class;
	@test -d android/skiaview/libs || mkdir android/skiaview/libs
	@cp -v android/skiaview/jni/$(AndJarPath)/skiaview.jar android/skiaview/libs
