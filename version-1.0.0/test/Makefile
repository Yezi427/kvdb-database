#
# Makefile
#
COMPILE = make
CC  = gcc
CXX = g++

FLAGS = -g -Wall
LIBS  = -Wl,-rpath,. -Wl,-rpath,..
INC = -I . 

TEST_IO_OBJS = test-io.o

TEST_IO = test-io

all:
	$(COMPILE) $(TEST_IO)

$(TEST_IO):$(TEST_IO_OBJS)
	$(CC) -o $@ $^ $(FLAGS) -pthread -L.. -lkvdb $(LIBS)
clean:
	rm -f $(TEST_IO) $(TEST_IO_OBJS)


###############################################################
test-io.o:test-io.c
	$(CXX) -o $@ $(FLAGS) $(INC) -c $<

