CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

TARGET = app
SRC = *.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
