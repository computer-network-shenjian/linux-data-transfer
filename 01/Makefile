CXXFLAGS := -std=c++11 -Wall -Wextra

SRC_LIB = $(wildcard ../common/*.cpp)
HEADER_LIB = $(wildcard ../common/*.hpp)
OBJ_LIB := $(SRC_LIB:.cpp=.o)

RM := rm -f

all: sender receiver

sender: sender.cpp $(SRC_LIB) $(HEADER_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

receiver: receiver.cpp $(SRC_LIB) $(HEADER_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADER_LIB)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) sender receiver
	$(RM) ../common/*.o

.PHONY : all test clean
