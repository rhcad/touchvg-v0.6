SUBDIRS         =$(subst /,,$(dir $(wildcard */)))
CLEANDIRS       =$(addsuffix .clean, $(SUBDIRS))
INSTALLDIRS     =$(addsuffix .install, $(SUBDIRS))
SWIGDIRS        =$(addsuffix .swig, $(SUBDIRS))

.PHONY:     $(SUBDIRS) clean install
all:        $(SUBDIRS)
clean:      $(CLEANDIRS)
install:    $(INSTALLDIRS)
swig:       $(SWIGDIRS)

$(SUBDIRS):
	@! test -e $@/Makefile || $(MAKE) -C $@

$(SWIGDIRS):
	@ ! test -e $(basename $@)/Makefile || \
	$(MAKE) -C $(basename $@) swig

$(CLEANDIRS) $(INSTALLDIRS):
	@ ! test -e $(basename $@)/Makefile || \
	$(MAKE) -C $(basename $@) $(subst .,,$(suffix $@))
