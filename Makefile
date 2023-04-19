CC=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -pedantic

SRCDIR=src
INCDIR=include
OBJDIR=obj

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET=program

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o
