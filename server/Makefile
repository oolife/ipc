CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall -I../inc -I. -std=c++14

SRCS=server_main.cpp ../SharedMem.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: server 

server: $(OBJS)
	$(CXX) -o server $(OBJS)

server_main.o: server_main.cpp ../inc/ipc_data.h ../inc/Request.h ../inc/SharedMem.h
SharedMem.o: ../SharedMem.cpp ../inc/ipc_data.h ../inc/Request.h ../inc/SharedMem.h

clean:
	$(RM) $(OBJS)


