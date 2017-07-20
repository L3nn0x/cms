PREFIX=.

BIN=$(PREFIX)/bin

BOOST_DIR=$(PREFIX)/boost_1_64_0

EXE=$(BIN)/cms

CC=g++

CFLAGS=-g -Wall -Werror -Wextra

SRCDIR=$(PREFIX)/src
SRC=$(wildcard $(SRCDIR)/*.cpp)

OBJDIR=$(PREFIX)/obj
OBJ=$(subst $(SRCDIR), $(OBJDIR), $(SRC:.cpp=.o))

INC=-I$(PREFIX)/include -I$(BOOST_DIR)

LIB=$(BOOST_DIR)/stage/lib/libboost_system.a

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(INC) $(LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

clean:
	rm -rf $(OBJ)
	rm -rf $(EXE)

re: clean all
