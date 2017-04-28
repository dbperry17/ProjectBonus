
# Automatically generated on Mon, Feb 27 2017 15:43:16

DEP = .deps
CC  = gcc
CXX = g++
CFLAGS   = -Wall 
CXXFLAGS = -Wall -std=c++11

# Target
SOURCES  = parser.cc pgir.cpp ir_debug.cc compiler.cc lexer.cc inputbuf.cc
OBJECTS  = parser.o pgir.o ir_debug.o compiler.o lexer.o inputbuf.o
DEPFILES = $(patsubst %.o,$(DEP)/%.d,$(OBJECTS))
TARGET   = a.out
ZIP_FILE = source.zip

.PHONY: all clean zip

all: $(DEP) $(TARGET)

-include $(DEPFILES)

$(TARGET): $(OBJECTS)
	@echo "Linking $@"
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

%.o: %.c
	@echo "Compiling $*.c"
	$(CC) -c $(CFLAGS) $*.c $(INCLUDE) -o $@
	@$(CC) -MM -MP -MT $@ $(CFLAGS) $*.c $(INCLUDE) > $(DEP)/$*.d

%.o: %.cpp
	@echo "Compiling $*.cpp"
	$(CXX) -c $(CXXFLAGS) $*.cpp $(INCLUDE) -o $@
	@$(CXX) -MM -MP -MT $@ $(CXXFLAGS) $*.cpp $(INCLUDE) > $(DEP)/$*.d

%.o: %.cc
	@echo "Compiling $*.cc"
	$(CXX) -c $(CXXFLAGS) $*.cc $(INCLUDE) -o $@
	@$(CXX) -MM -MP -MT $@ $(CXXFLAGS) $*.cc $(INCLUDE) > $(DEP)/$*.d

$(DEP):
	@mkdir -p $@

zip: $(SOURCES)
	@echo "Zipping source files to $(ZIP_FILE)"
	@zip $(ZIP_FILE) Makefile $(SOURCES) ir_debug.h compiler.h lexer.h inputbuf.h parser.h

clean:
	@rm -f $(TARGET) $(DEPFILES) $(OBJECTS) $(ZIP_FILE)
	@rmdir $(DEP)



