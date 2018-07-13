CXXFLAGS=-I. -g -DDEBUG -Wall -std=c++0x
OBJS= common.o AutoTable.o File.o main.o
CXX=g++

EXE=auto_table

all:$(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS)

clean:
	rm -f $(OBJS) $(EXE)
