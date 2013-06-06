CXX	:= g++
#CXXFLAGS := -O3 -g -std=c++0x  -march=corei7-avx -mcmodel=large
#CXXFLAGS := -O2 -g -std=c++0x  
CXXFLAGS  := -Wall -O0 -std=c++0x  -g
#CXXFLAGS := -mcmodel=large
#CXXFLAGS := -funroll-loops
#CXXFLAGS := -pg
#CXXLFLAGS := -L. -lpthread  -L./ -lgzstream -lz
CXXLFLAGS := -L. -lpthread   -lgzstream -lz  
INCLUDES := -I .

OBJECTS	:= main.o trace.o inst.o func.o ed.o util.o

ped: $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXLFLAGS) -o ped

main.o: main.cpp ./trace.h 
	$(CXX)  $(INCLUDES) $(CXXFLAGS) -c  main.cpp    -o  main.o

trace.o: ./trace.cpp ./trace.h
	$(CXX)	$(INCLUDES)	$(CXXFLAGS)	-c	./trace.cpp	-o	trace.o

inst.o: ./inst.cpp ./inst.h
	$(CXX)	$(INCLUDES)	$(CXXFLAGS)	-c	./inst.cpp	-o	inst.o

func.o: ./func.cpp ./func.h
	$(CXX)	$(INCLUDES)	$(CXXFLAGS)	-c	./func.cpp	-o	func.o


ed.o: ./ed.cpp ./ed.h 
	$(CXX)	$(INCLUDES)	$(CXXFLAGS)	-c	./ed.cpp	-o	ed.o

#util.o: ./util.cpp ./util.h 
#	$(CXX)	$(INCLUDES)	$(CXXFLAGS)	-c	./util.cpp	-o	util.o


clean:
	rm -rf ./*.o    
	rm -f insted* ped
