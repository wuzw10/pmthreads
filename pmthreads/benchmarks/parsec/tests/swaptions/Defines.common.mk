# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================
PROG := swaptions

SRCS += CumNormalInv.cpp  HJM.cpp  HJM_Securities.cpp  HJM_SimPath_Forward_Blocking.cpp  HJM_Swaption_Blocking.cpp  icdf.cpp  MaxFunction.cpp  RanUnif.cpp 

CSRC += nr_routines.c
#
OBJS := ${SRCS:.cpp=.o}
OBJS += ${CSRC:.c=.o}

CC = g++

LIBS += 

CFLAGS += -DENABLE_THREADS -pthread

# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
