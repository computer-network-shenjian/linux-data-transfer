CXXFLAGS := -std=c++11 -Wall -Wextra

SRC_SENDER := 01/sender1.cpp 02/sender2.cpp #03/sender3.cpp #04/sender4.cpp #05/sender5.cpp #06/sender6.cpp
SRC_RECEIVER := 01/receiver1.cpp 02/receiver2.cpp #03/receiver3.cpp #04/receiver4.cpp #05/receiver5.cpp #06/receiver6.cpp

TARGET_SENDER := $(SRC_SENDER:.cpp=)
TARGET_RECEIVER:= $(SRC_RECEIVER:.cpp=)

SRC_LIB := common/shared_library.cpp common/Log.cpp
HEADER_LIB := common/shared_conf.hpp common/status.hpp common/shared_library.hpp
OBJ_LIB := $(SRC_LIB:.cpp=.o)

RM := rm -f

all: $(OBJ_LIB) $(HEADER_LIB) $(TARGET_SENDER) $(TARGET_RECEIVER)

%: %.o $(OBJ_LIB) $(HEADER_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET_SENDER) $(TARGET_RECEIVER)
	$(RM) common/*.o

.PHONY : all test clean
