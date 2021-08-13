################################################################################
#Makefile to Generate Fractal GPU 	Edg@r J 2021 :)
################################################################################
#Compilers
GCC				= gcc
CXX 			= g++ 

CUDA 			= /usr/local/cuda-11.2
CUDA_SDK	= $(CUDA)/samples
NVCC     	= $(CUDA)/bin/nvcc

#Include Paths
CUDAINC   = -I. 
INCQT			= -I/usr/include/x86_64-linux-gnu/qt5/

INC = -I$(HOME)/graphics/NumCpp/NumCpp/include -Iglew/include
#Library Paths
CUDALIB		= -L/usr/lib/x86_64-linux-gnu -L$(CUDA)/lib64 \
						-lcuda -lcudart -lcudadevrt
GLLIB  		=  -lEGL -lGLEW  -lOpenGL 
QTLIB			= -lQt5Core
OTHERLIB	= -lfreeimage -lstdc++fs 
LIB 			= -Lglew/lib  $(GLLIB) -lm -lstb

################ Choosing architecture code for GPU ############################
NVCC_ARCH			=
HOSTNAME		 	= $(shell uname -n)

ifeq ("$(HOSTNAME)","Edgar-PC")
	NVCC_ARCH		= -gencode arch=compute_61,code=sm_61
endif

###############	Debug, 0 -> False,  1-> True
DEBUGON						:= 0

ifeq (1,$(DEBUGON))
	CXXDEBUG 				:= -g -Wall
	CXXOPT					:= -O0 -std=c++17
#	NVCCDEBUG				:= -g -pg -G -fPIC -std=c++17
	NVCCDEBUG				:= 
	NVCCOPT					:= -O0
	NVCCFLAGSXCOMP 	:= -Xcompiler -g,-Wall,-O0 	
else
	CXXDEBUG 				:= 
	CXXOPT					:= -O3 -ffast-math -funroll-loops -std=c++17
	NVCCDEBUG				:= 
	NVCCOPT					:= -O3 --cudart=shared -use_fast_math
	NVCCFLAGSXCOMP 	:= -Xcompiler -O3,-ffast-math,-funroll-loops
endif
###############################################################################
CXXFLAGS				= $(CXXDEBUG) $(CXXOPT) -fopenmp -Wno-unused-function
NVCCFLAGS 			= $(NVCCDEBUG) $(NVCC_DP) --compile $(NVCCOPT) $(NVCC_ARCH)
NVCCFLAGSLINK		= $(NVCCDEBUG) $(NVCC_DP) $(NVCCOPT) $(NVCC_ARCH)
###############################################################################

#g++ -DGLEW_EGL -DGLEW_STATIC -Iglew/include -I. egltest.cpp glew/src/glew.c shader.o -lGL -lEGL -lstb

TARGET = egl

all: $(TARGET)

OBJLIST = shader.o glew.o

egl : main.o $(OBJLIST)
	$(CXX) -DGLEW_EGL -DGLEW_STATIC $(CXXFLAGS) $(CUDAINC) $< -o $@ $(OBJLIST) $(LIB) 

main.o: main.cpp 
	$(CXX)  $(CXXFLAGS) $(INC) $(CUDAINC) -c $< -o $@ 

shader.o: shader.cpp
	$(CXX)  $(CXXFLAGS) $(INC) $(CUDAINC) -c $< -o $@ 

glew.o: glew.c
	$(CXX) -DGLEW_EGL -DGLEW_STATIC $(CXXFLAGS) $(INC) -c $< -o $@	


clean:
	-rm -f *.o 
	-rm -f $(TARGET)
