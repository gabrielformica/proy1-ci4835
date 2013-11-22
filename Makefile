CC= gcc -g
LIBO= list.o
LIBH= list.h

all: server client 
server: server.o ${LIBO}
	${CC} server.o ${LIBO} -o server 
server.o: server.c ${LIBH}
	${CC} -c server.c
client: client.o ${LIBO}
	${CC} client.o ${LIBO} -o client
client.o: client.c ${LIBH} 
	${CC} -c client.c
clean:
	rm -f *.o server client
