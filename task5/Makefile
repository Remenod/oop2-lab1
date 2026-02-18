CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(shell pkg-config gtkmm-4.0 --cflags)
LDFLAGS  = $(shell pkg-config gtkmm-4.0 --libs)
SRC = *.cpp
TARGET = app

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
