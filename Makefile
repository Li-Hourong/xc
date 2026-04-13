CPP := g++
CPPFLAGS := -std=c++17 -Wall -Wextra -O2
TARGET := xc_lexer
SRCS := main.cpp lexer.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	-del /q $(TARGET).exe 2>nul
	-del /q $(TARGET) 2>nul
	-del /q *.o 2>nul
