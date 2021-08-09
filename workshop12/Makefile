# runs updates/installer.bash to install latest updates .zip
# then runs make using the latest Makefile

# make all make args PHONY 
.PHONY: $(MAKECMDGOALS)

default:=all
# make first arg depend on all
ifneq (,$(MAKECMDGOALS))

    # make first arg default and all other args after the first do nothing
    unique:=$(sort $(MAKECMDGOALS))
    default:=$(firstword $(unique))
    $(eval $(wordlist 2, $(words $(unique)), $(unique)):;@:)

endif

# level 0 all kinds of make allowed
ifeq ($(MAKELEVEL),0)

ifeq (install,$(MAKECMDGOALS))

# install - do not run updates/Makefile but force files to be installed / updated
install:
	@rm -f updates/*-installed
	@if [ -f updates/installer.bash ] ; then cp updates/installer.bash .installer.bash ; bash .installer.bash $(MAKECMDGOALS) ; fi
	@rm -f .ran-setup-aew
	@if [ -f setup-aew.bash ] ; then bash setup-aew.bash ; touch .ran-setup-aew ; fi
	@if [ -f setup-aew.bash -a -f updates/Makefile ] ; then make --no-print-directory -f updates/Makefile $(MAKECMDGOALS) ; fi
	@touch updates/log

else

# default - just run Make from latest zip
# we copy installer.bash first because it may be updated
$(default):
	@if [ -f updates/installer.bash ] ; then cp updates/installer.bash .installer.bash ; bash .installer.bash $(MAKECMDGOALS) ; fi
	@rm -f .ran-setup-aew
	@if [ -f setup-aew.bash ] ; then bash setup-aew.bash ; touch .ran-setup-aew ; fi
	@if [ -f updates/Makefile ] ; then make --no-print-directory -f updates/Makefile $(MAKECMDGOALS) ; fi
	@touch updates/log
endif

else

# level 1 only run setup-aew.bash
ifeq ($(MAKELEVEL),1)
$(default):
	@if [ -f setup-aew.bash -a ! -f .ran-setup-aew ] ; then bash setup-aew.bash ; fi

else
# level 2+ do nothing
$(default):
	@true
endif
endif
