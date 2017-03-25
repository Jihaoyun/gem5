ALPHA_CC = alphaev67-unknown-linux-gnu-gcc
ALPHA_CXX = alphaev67-unknown-linux-gnu-g++

CPP_FILES := $(wildcard src/*.cpp)
C_FILES := $(wildcard src/*.c)
OBJ_FILES :=	$(addprefix bin/,$(notdir $(C_FILES:.c=.o))) $(addprefix bin/,$(notdir $(CPP_FILES:.cpp=.o))) \

#COMPILE WITHOUT LINKING
all: $(OBJ_FILES)

#COMPILE C SOURCES
bin/%.o: src/%.c
	@mkdir -p bin
	@echo
	@echo "### $< ALPHA COMPILATION ###"
	$(ALPHA_CC) $< -o $@ -I../../../ ../../../util/m5/m5op_alpha.S --static

#COMPILE C++ SOURCES
bin/%.o: src/%.cpp
	@mkdir -p bin
	@echo
	@echo "### $< ALPHA COMPILATION ###"
	$(ALPHA_CXX) $< -o $@ -I../../../ ../../../util/m5/m5op_alpha.S --static

clean:
	rm -rf bin
