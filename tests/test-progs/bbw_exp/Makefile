ARM_CC = arm-linux-gnueabi-gcc

C_FILE = $(wildcard *.c)
ASSEMBLE_FILE = $(wildcard *.s)

OBJ_FILE = $(C_FILE:.c=.o) $(ASSEMBLE_FILE:.s=.assemble)

ASSEMBLE_OBJ_FILE = $(C_FILE:.c=.s)

all: $(OBJ_FILE)

#########################
#####ARM COMPILATION#####
#########################

#COMPILE C SOURCES
%.o: %.c
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

%.s: %.c
	@echo
	@echo "### $< ARM BUILD ASSEMBLE FILES ###"
	$(ARM_CC) -S $< -o $@
	@echo

clean:
	rm -rf $(OBJ_FILE)

