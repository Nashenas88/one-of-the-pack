NAME = one_of_the_pack

SRC_DIR = src

ifeq ($(OS),Windows_NT)
	PROG = $(NAME).exe
	CLFAGS = -Wall -mno-cygwin
else
	PROG = $(NAME)
	CFLAGS = -Wall
endif

all: $(PROG)

$(PROG):
	cd $(SRC_DIR); make

clean:
	rm -f $(PROG); cd $(SRC_DIR); make clean
