#
# Makefile for the PVR DVBLink add-on for XBMC PVR
#
# See the README for copyright information and
# how to reach the author.
#

ADDONBINNAME    = XBMC_DVBLink
ADDONNAME       = pvr.dvblink
LIBNAME         = libdvblink-addon
lib_LTLIBRARIES = libdvblink-addon.la

LIBS            = @abs_top_srcdir@/lib/libdvblinkremote/libdvblinkremote.la

include ../Makefile.include.am

libdvblink_addon_la_SOURCES = src/client.cpp \
                              src/DVBLinkClient.cpp
libdvblink_addon_la_LDFLAGS = @TARGET_LDFLAGS@

