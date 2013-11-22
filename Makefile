CC= gcc -g

GENFUNCO= myfunctions.o
GENFUNCH= myfunctions.h
LIBO= list.o ${GENFUNCO}
LIBH= list.h ${GENFUNCH}

all: server client 
server: server.o ${LIBO}
	${CC} server.o ${LIBO} -o server 
server.o: server.c ${LIBH}
	${CC} -c server.c
client: client.o ${LIBO}
	${CC} client.o ${LIBO} -o client
client.o: client.c ${LIBH} 
	${CC} -c client.c
list.o: list.c list.h ${GENFUNCH}
	${CC} -c list.c
myfunctions.o: myfunctions.c myfunctions.h
	${CC} -c myfunctions.c
clean:
	rm -f *.o server client
