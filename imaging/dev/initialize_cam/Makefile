################################################################################
# Exposure_C Makefile
################################################################################
PROJECT_ROOT=../
OPT_INC = ${PROJECT_ROOT}/common/make/common_spin.mk
-include ${OPT_INC}

################################################################################
# Key paths and settings
################################################################################
ifeq ($(wildcard ${OPT_INC}),)
CFLAGS += -std=c++11
CXX = g++ ${CFLAGS}
ODIR  = .obj/build${D}
SDIR  = .
MKDIR = mkdir -p
endif

OUTPUTNAME = Exposure_C${D}
OUTDIR = ../bin

################################################################################
# Dependencies
################################################################################
# Spinnaker deps
SPINNAKER_LIB = -L../../lib -lSpinnaker${D} -lSpinnaker_C${D} ${SPIN_DEPS}

################################################################################
# Master inc/lib/obj/dep settings
################################################################################
_OBJ = Exposure_C.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
INC = -I../../include/spinc -I/usr/include/spinnaker/spinc
LIB += -Wl,-Bdynamic ${SPINNAKER_LIB} 
LIB += -Wl,-rpath-link=../../lib 

################################################################################
# Rules/recipes
################################################################################
# Final binary
${OUTPUTNAME}: ${OBJ}
	${CXX} -o ${OUTPUTNAME} ${OBJ} ${LIB}
	mv ${OUTPUTNAME} ${OUTDIR}

# Intermediate object files
${OBJ}: ${ODIR}/%.o : ${SDIR}/%.cpp
	@${MKDIR} ${ODIR}
	${CXX} ${CFLAGS} ${INC} -Wall -D LINUX -c $< -o $@

# Clean up intermediate objects
clean_obj:
	rm -f ${OBJ}
	@echo "intermediate objects cleaned up!"

# Clean up everything.
clean: clean_obj
	rm -f ${OUTDIR}/${OUTPUTNAME}
	@echo "all cleaned up!"
