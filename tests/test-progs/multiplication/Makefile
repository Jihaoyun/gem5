ARM_CC = arm-linux-gnueabi-g++

C_FILE = $(wildcard *.cpp)
ASSEMBLE_FILE = $(wildcard *.s)

OBJ_FILE = $(C_FILE:.cpp=.o) $(ASSEMBLE_FILE:.s=.assemble)

ASSEMBLE_OBJ_FILE = $(C_FILE:.cpp=.s)

all: $(OBJ_FILE)

#########################
#####ARM COMPILATION#####
#########################

#COMPILE C SOURCES
%.o: %.cpp
	@echo
	@echo "### $< ARM C_FILE COMPILATION ###"
	$(ARM_CC) $< -o $@ -static -g -lm 
	@echo

#COMPILE ASSEMBLE SOURCES
%.assemble: %.s
	@echo
	@echo "### $< ARM ASSEMBLE_FILE COMPILATION ###"
	$(ARM_CC) $< -o $@ -static -g -lm
	@echo

#########################
#####ARM ASSEMBLING######
#########################

build: $(ASSEMBLE_OBJ_FILE)

%.s: %.cpp
	@echo
	@echo "### $< ARM BUILD ASSEMBLE FILES ###"
	$(ARM_CC) -S $< -o $@
	@echo

clean:
	rm -rf $(OBJ_FILE)

