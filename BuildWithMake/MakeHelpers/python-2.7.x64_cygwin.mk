ifeq ($(CLUSTER), x64_cygwin)
    PYTHON_BASE   = Python-2.7
    PYTHON_SRCDIR = 
    PYTHON_BINDIR = C:/OpenSource/$(PYTHON_BASE)/bin
    SV_PYTHON_SO_PATH = $(PYTHON_BINDIR)
    PYTHON_INCDIR = -IC:/OpenSource/$(PYTHON_BASE)/include
    PYTHON_LIBDIR = C:/OpenSource/$(PYTHON_BASE)/libs
    PYTHON_LIB    = $(LIBPATH_COMPILER_FLAG)$(PYTHON_LIBDIR) $(LIBFLAG)python27$(LIBLINKEXT)
endif