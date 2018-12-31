CXXFLAGS := -std=c++11 -Wall -Wextra

SRC_LIB = $(wildcard ../common/*.cpp)
HEADER_LIB = $(wildcard ../common/*.hpp)
OBJ_LIB := $(SRC_LIB:.cpp=.o)

RM := rm -f

all: $(OBJ_LIB) sender receiver $(HEADER_LIB)

%.o: %.cpp $(HEADER_LIB)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

sender: sender.cpp $(OBJ_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

receiver: receiver.cpp $(OBJ_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) sender receiver
	$(RM) ../common/*.o

.PHONY : all test clean
