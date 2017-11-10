CC = g++

all: libclient.so
OBJ=myfunc.o change.o http.o queue.o client.o SysLog.o

CFLAGS = -m32 -Wall -g

libclient.so:$(OBJ)
	ar -r $@ $+

install:libclient.so
	cp libclient.so ../lib

SysLog.o:SysLog.cpp
	$(CC) $(CFLAGS) -o $@ -c $+

myfunc.o:myfunc.cpp
	$(CC) $(CFLAGS) -o $@ -c $+

change.o:change.cpp
	$(CC) $(CFLAGS) -o $@ -c $+

http.o:http.cpp
	$(CC) $(CFLAGS) -o $@ -c $+
	
queue.o:queue.cpp
	$(CC) $(CFLAGS) -o $@ -c $+

client.o:client.cpp
	$(CC) $(CFLAGS) -o $@ -c $+

clean:
	rm -f *.o client *~ *.log *.so
