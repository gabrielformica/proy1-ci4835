CC= gcc -g -lpthread

LIBO= list.o roomslist.o
LIBH= list.h roomslist.h

all: schat cchat 
schat: server.o ${LIBO}
	${CC}  server.o ${LIBO} -o schat 
server.o: server.c ${LIBH}
	${CC}  -c server.c
cchat: client.o ${LIBO}
	${CC} client.o ${LIBO} -o cchat
client.o: client.c ${LIBH} 
	${CC} -c client.c
list.o: list.c list.h 
	${CC} -c list.c
roomslist.o: roomslist.c roomslist.h list.h
	${CC} -c roomslist.c
clean:
	rm -f *.o schat cchat
