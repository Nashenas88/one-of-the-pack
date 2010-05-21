NAME = one_of_the_pack

SRC_DIR = src

ifeq ($(OS),Windows_NT)
	PROG = $(NAME).exe
	CLFAGS = -Wall -mno-cygwin
else
	PROG = $(NAME)
	CFLAGS = -Wall
endif

all: $(PROG) $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h

$(PROG): $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h
	cd $(SRC_DIR) && make

ifeq ($(shell uname),Darwin)
app: $(PROG)
	cd $(SRC_DIR) && make app
endif

clean:
	rm -f $(PROG)
	cd $(SRC_DIR) && make clean
