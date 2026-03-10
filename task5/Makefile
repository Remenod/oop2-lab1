CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 -lnotcurses -lnotcurses-core

TARGET = app
SRC = *.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
