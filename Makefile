# contrib/rusmorph/Makefile

MODULE_big = rusmorph
OBJS = rusmorph.o libdict/chartype.o libdict/mlmamain.o xmorph/wildscan.o xmorph/capsheme.o xmorph/lemmatiz.o dictree/lidstree.o dictree/mixTypes.o dictree/stemtree.o dictree/flexTree.o dictree/classmap.o dictree/mxTables.o $(WIN32RES)

EXTENSION = rusmorph
DATA = rusmorph--1.0.sql 
PGFILEDESC = "rusmorph - add-on dictionary template for full-text search"

REGRESS = rusmorph

CUSTOM_COPT = -Iinclude 

CXXFLAGS = -Wall -g -O2 -fpic -std=c++11 -Ilibdict

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/rusmorph
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

override COMPILER := g++
