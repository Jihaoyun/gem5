ALPHA_CC = alphaev67-unknown-linux-gnu-gcc
ALPHA_CXX = alphaev67-unknown-linux-gnu-g++

ARM_CC = arm-linux-gnueabi-gcc
ARM_CXX = arm-linux-gnueabi-g++

X86_CC = gcc
X86_CXX = g++

CPP_FILES := $(wildcard src/*.cpp)
C_FILES := $(wildcard src/*.c)
OBJ_FILES :=	$(addprefix bin/alpha/,$(notdir $(C_FILES:.c=.o))) $(addprefix bin/alpha/,$(notdir $(CPP_FILES:.cpp=.o))) \
							$(addprefix bin/x86/,$(notdir $(C_FILES:.c=.o))) $(addprefix bin/x86/,$(notdir $(CPP_FILES:.cpp=.o))) \
							$(addprefix bin/arm/,$(notdir $(C_FILES:.c=.o))) $(addprefix bin/arm/,$(notdir $(CPP_FILES:.cpp=.o)))
#COMPILE WITHOUT LINKING
all: $(OBJ_FILES)

#########################
####ALPHA COMPILATION####
#########################

#COMPILE C SOURCES
bin/alpha/%.o: src/%.c
	@mkdir -p bin/alpha
	@echo
	@echo "### $< ALPHA COMPILATION ###"
	$(ALPHA_CC) $< -o $@ -static -g -lm

#COMPILE C++ SOURCES
bin/alpha/%.o: src/%.cpp
	@mkdir -p bin/alpha
	@echo
	@echo "### $< ALPHA COMPILATION ###"
	$(ALPHA_CXX) $< -o $@ -static -g


#########################
#####X86 COMPILATION#####
#########################

#COMPILE C SOURCES
bin/x86/%.o: src/%.c
	@mkdir -p bin/x86
	@echo
	@echo "### $< X86 COMPILATION ###"
	$(X86_CC) $< -o $@ -static -g -lm

#COMPILE C++ SOURCES
bin/x86/%.o: src/%.cpp
	@mkdir -p bin/x86
	@echo
	@echo "### $< X86 COMPILATION ###"
	$(X86_CXX) $< -o $@ -static -g

#########################
#####ARM COMPILATION#####
#########################

#COMPILE C SOURCES
bin/arm/%.o: src/%.c
	@mkdir -p bin/arm
	@echo
	@echo "### $< ARM COMPILATION ###"
	$(ARM_CC) $< -o $@ -static -g -lm

#COMPILE C++ SOURCES
bin/arm/%.o: src/%.cpp
	@mkdir -p bin/arm
	@echo
	@echo "### $< ARM COMPILATION ###"
	$(ARM_CXX) $< -o $@ -static -g

clean:
	rm -rf bin

clean-arm:
	rm -rf bin/arm
