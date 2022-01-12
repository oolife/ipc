CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall -I../inc -I. -std=c++14

SRCS=client_main.cpp ../SharedMem.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: client 

client: $(OBJS)
	$(CXX) -o client $(OBJS)

client_main.o: client_main.cpp ../inc/ipc_data.h ../inc/Request.h ../inc/SharedMem.h
SharedMem.o: ../SharedMem.cpp ../inc/ipc_data.h ../inc/Request.h ../inc/SharedMem.h

clean:
	$(RM) $(OBJS)


