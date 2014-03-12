CC=g++
CFALGS=-Wall -g
SRC=	main.cpp \
	tcpserver.cpp \
	tcpclient.cpp \
	localtcpserver.cpp \
	localtcpclient.cpp \
	udpserver.cpp \
	udpclient.cpp 
MAIN=main
all:
	echo "make test"
	$(CC) $(CFLAGS) ${SRC} -o $(MAIN)
clean:
	rm $(MAIN) -rf
	
