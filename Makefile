CC= gcc -g

LIBO= list.o roomslist.o
LIBH= list.h roomslist.h

all: server client 
server: server.o ${LIBO}
	${CC} -lpthread server.o ${LIBO} -o server 
server.o: server.c ${LIBH}
	${CC} -lpthread -c server.c
client: client.o ${LIBO}
	${CC} client.o ${LIBO} -o client
client.o: client.c ${LIBH} 
	${CC} -c client.c
list.o: list.c list.h 
	${CC} -c list.c
roomslist.o: roomslist.c roomslist.h list.h
	${CC} -c roomslist.c
clean:
	rm -f *.o server client
