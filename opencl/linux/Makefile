EXECUTABLES = main

all: $(EXECUTABLES)

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

main: main.cpp
	g++ $(CL_CFLAGS) $(CL_LDFLAGS) -o$@ $^ -lrt -lOpenCL -g

clean:
	rm -f $(EXECUTABLES) *.o
