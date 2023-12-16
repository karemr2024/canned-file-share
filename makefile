.SUFFIXES:
.SUFFIXES: .o .cpp
#============================================================
TARGET	=  NFileTree

C_SOURCES =  NFileTree.cpp
C_OBJS     = NFileTree.o
MY_INCLUDES =

CCX = g++
CXXFLAGS = -g -std=c++14  -Wall

#============================================================
all: $(TARGET)

.o:.cpp	$(MY_INCLUDES)
	$(CCX)  -c  $(CXXFLAGS) $<  

$(TARGET) :   $(C_OBJS)
	$(CCX) $(CXXFLAGS)  $^ $(LIBDIRS)  -o $@

# Implicit rules: $@ = target name, $< = first prerequisite name, $^ = name of all prerequisites
#============================================================

ALL_SOURCES = makefile $(C_SOURCES) $(MY_INCLUDES)

INPUT_FILES  =  input.txt

clean:
	rm -f $(TARGET) $(C_OBJS) core *~

tar: $(ALL_SOURCES) $(INPUT_FILES)
	tar cvf $(TARGET).tar $(ALL_SOURCES)  $(INPUT_FILES)

# $(TARGET).ps: $(ALL SOURCES) 
# 	enscript -pcode.ps $(ALL_SOURCES)