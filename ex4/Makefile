#################################################################
#
#				Warning!!!!
#				============
#	
# this make file compiles all the sources with implicit rules
# therefore in order to make sure all the changes were made 
# run it with "make clean all"
#
##################################################################

CG_HOME = /cs/course/2013/cg

# OpenMesh defs
OM_DIR = $(CG_HOME)/OpenMesh
OM_LIBS = -L$(OM_DIR)/lib -lOpenMeshCore
OM_INCLUDE=  -I$(OM_DIR)/include

# Bimage defs
BI_INCLUDE = -I$(CG_HOME)/bimage
BI_LIBS = -L$(CG_HOME)/bimage -lbimage

EXEC = 

# list of all the sources files
SRCS	=  	ex4.cpp		\
		Model.cpp 	\
		ShaderIO.cpp	\
			
# compiler 
CC	= g++

INCLUDE = -I/usr/include -I/usr/X11R6/include -I$(CG_HOME)/include $(BI_INCLUDE) $(OM_INCLUDE)

OBJS = $(subst .cpp,.o,$(SRCS)) 		#substituting .cc -> .o

# in order to run in debug mode use the command "make DBG=1"
ifdef DBG

	CFLAGS = 	\
		-Wall 	\
		-Werror \
		-DDEBUG	\
		-O0		\
		-g3		\
		-Wno-deprecated
					
						
else

	CFLAGS = 		\
		-Wall		\
		-DNDEBUG    \
		-O2
				 
endif


# linker
LD	= $(CC)
LDFLAGS	=


# the name of the lib to create 
APP = ex4

# each lib should add with -l flag
LIBS =  -L/usr/lib -lm -lGLEW -lglut -lGL -lGLU -ldl -L/usr/X11R6/lib $(BI_LIBS) $(OM_LIBS)


# rule to change the suffix of the sources files
OBJS	= ${SRCS:.cpp=.o}

# clear out all suffixes
.SUFFIXES:
# list only those we use
.SUFFIXES: .o .cpp

# define a suffix rule for .c -> .o
.cpp.o :
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

##################################
# 	default target 		##
##################################
all : $(APP)

$(APP) : $(OBJS)
	$(LD)  $(OBJS) $(LIBS) -o $(APP)

$(OBJS) : $(SRCS)


tar:
	tar cvf $(EXEC).tar Makefile $(SRCS) $(HDRS) README
	

.PHONY:
clean :
	rm -f $(OBJS) $(EXEC) *~ $(APP)
