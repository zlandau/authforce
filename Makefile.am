# $Author: kapheine $
# $Date: 2001/02/09 23:44:29 $
# $Revision: 1.3 $

EXTRA_DIST = BUGS TODO THANKS ChangeLog \
	authforce.spec.in authforce.spec \
	authforcerc.sample

SUBDIRS = intl data src doc po


distbz2: distdir
	-chmod -R a+r $(distdir)
	$(TAR) choyf $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)
