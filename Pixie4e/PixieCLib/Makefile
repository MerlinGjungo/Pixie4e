
# -*- PIXIE-4 Makefile -*-

ifeq ($(shell uname), Linux) 
include Makefile.Linux
else
  ifeq ($(PROCESSOR_ARCHITECTURE), x86)
    ifeq ($(PROCESSOR_ARCHITEW6432), AMD64)
      include Makefile.amd64
    else
      include Makefile.x86
    endif
  endif
endif
