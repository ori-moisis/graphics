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

EXEC = 

# list of all the sources files
SRCS_1   = ex1a.cpp \
          ModelA.cpp \
          ShaderIO.cpp
SRCS_2	= ex1b.cpp 	\
		  ModelB.cpp 	\
		  Ball.cpp    \
          ShaderIO.cpp \

			
# compiler 
CC	= g++

INCLUDE = -I/usr/include -I/usr/X11R6/include -I/cs/course/current/cg/include

OBJS_1 = $(subst .cpp,.o,$(SRCS_1)) 		#substituting .cc -> .o
OBJS_2 = $(subst .cpp,.o,$(SRCS_2)) 		#substituting .cc -> .o

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
APP_1 = ex1a
APP_2 = ex1b

# each lib should add with -l flag
LIBS =  -L/usr/lib -lm -lGLEW -lglut -lGL -lGLU -ldl -L/usr/X11R6/lib


# rule to change the suffix of the sources files
# OBJS	= ${SRCS:.cpp=.o}

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
all : $(APP_1) $(APP_2)

$(APP_1) : $(OBJS_1)
	$(LD)  $(OBJS_1) $(LIBS) -o $(APP_1)

$(OBJS_1) : $(SRCS_1)


$(APP_2) : $(OBJS_2)
	$(LD)  $(OBJS_2) $(LIBS) -o $(APP_2)

.PHONY:
clean :
	rm -f $(OBJS_1) $(OBJS_2) $(EXEC) *~ $(APP_1) $(APP_2)
