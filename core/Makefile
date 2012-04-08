SUBDIRS         =$(subst /,,$(dir $(wildcard */)))
CLEANDIRS       =$(addsuffix .clean, $(SUBDIRS))
INSTALLDIRS     =$(addsuffix .install, $(SUBDIRS))

.PHONY:     $(SUBDIRS)
all:        $(SUBDIRS)
clean:      $(CLEANDIRS)
install:    $(INSTALLDIRS)

$(SUBDIRS):
	@! test -e $@/Makefile || $(MAKE) -C $@

$(CLEANDIRS) $(INSTALLDIRS):
	@ ! test -e $(basename $@)/Makefile || \
	$(MAKE) -C $(basename $@) $(subst .,,$(suffix $@))
