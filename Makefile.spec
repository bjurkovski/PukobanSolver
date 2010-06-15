# Default Makefile.spec

#profiling
#PROF = -pg

#Additional include paths
INCLUDE +=

#Additional C flags
CFLAGS += -Wall $(PROF)

#Additional C++ flags
CPPFLAGS +=

#List of libraries to link with
LINKWITH +=

#Additional Linker flags
LDFLAGS += $(PROF)

#Name of the executable to create
EXECUTABLE = solver

#Name of the static library to create
ARCHIVE =

#Name of the dynamic library to create
DYNLIB =

#Other directories to update before compiling
SUBDIR =

#all: $(EXECUTABLE)

#profiling
#prof:
#	gprof -c solver > profiling

#clean:
#	rm gmon.out -f
