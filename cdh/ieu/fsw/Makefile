##############################################################################
#
# Makefile
#
# This Makefile compiles flight software into one executable file by 
# creating independent object files and linking them together. It permits 
# putting source files, headers, and inline files in subfolders, and 
# subfolders of subfolders and so-forth, and will automatically generate a 
# dependency file for each object This means that modification of headers and 
# inline files will trigger recompilation of files which are dependent.
#
# Source files in src/ are detected via shell find command source files from 
# a directory are built into one object file placed in a mirrored directory 
# path under obj/. The target binary program is created and placed in bin/.
# 
# Only edit directory paths and flags, libraries, and includes. There should
# not be a reason to edit the rules (below the variable definitions).
#
# -------------------------------------------------------------------------- #
#
# Benjamin Spencer
# ASEN 4018
# Project HEPCATS
# Subsystem: C&DH
# Created: January 7, 2018
# 
###############################################################################

# Compiler:
CC := gcc

# Target binary program:
TARGET := program

# Root directories:
ROOT        := /home/xenomai/fsw
XENO_CONFIG := /usr/xenomai/bin/xeno-config

# Directories:
SRCDIR    := $(ROOT)/src
INCDIR    := $(ROOT)/include
BUILDDIR  := $(ROOT)/obj
TARGETDIR := $(ROOT)/bin
RESDIR    := $(ROOT)/res

# Extensions:
SRCEXT := c
DEPEXT := d
OBJEXT := o

# Flags, Libraries and Includes:
CFLAGS   := $(shell $(XENO_CONFIG) --posix --alchemy --cflags)
LDFLAGS  := $(shell $(XENO_CONFIG) --posix --alchemy --ldflags)

LIB     := 
INC     := -I$(INCDIR) -I/usr/local/include
INCDEP  := -I$(INCDIR)

# Find source and object files:
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,\
	$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# -------------------------------------------------------------------------- #
# Rules
# -------------------------------------------------------------------------- #

# Default make:
all: $(TARGET)

# Remake:
remake: cleaner all

# Make the directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# Clean objects only:
clean:
	@$(RM) -rf $(BUILDDIR)

# Full clean (objects and binaries):
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

# Pull in dependency info for *existing* .o files:
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Link:
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB) $(CFLAGS) $(LDFLAGS)

# Compile:
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > \
		$(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' \
		< $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp \
		| fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# Non-file targets:
.PHONY: all remake clean cleaner