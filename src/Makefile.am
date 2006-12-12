## Process this file with automake to produce Makefile.in
# $Author: kapheine $
# $Date: 2001/04/28 20:32:46 $
# $Revision: 1.3 $

localedir = $(datadir)/locale
INCLUDES = \
	-I$(top_srcdir)/intl -DLOCALEDIR=\"$(localedir)\"

# @prefix@

bin_PROGRAMS = authforce

authforce_SOURCES = \
	config.c \
	debug.c \
	extract.c \
	files.c \
	http.c \
	main.c \
	methods.c \
	misc.c \
	signal.c \
	extern.h

authforce_LDADD = @INTLLIBS@
