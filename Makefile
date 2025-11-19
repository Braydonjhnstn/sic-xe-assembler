##
## Name: Braydon Johnston REDid: 131049942
## Name: Reinaldo Roldan  REDid: 825760540
## Cs530-03-Fall2025
## Assignment 2 SIC/XE Assembler
## Makefile
##

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -Iinclude
SRCDIR = src
OBJDIR = obj
TARGET = asx

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

test: $(TARGET)
	@echo "Running tests..."
	@for file in tests/*.sic; do \
		echo "Testing $$file..."; \
		./$(TARGET) $$file; \
	done

