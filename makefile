all: client server db

db: createDB.c
	gcc -o db createDB.c `pkg-config --libs mysqlclient`

client: client.c
	gcc -o client client.c clientFunction.c protocol.c `pkg-config --libs mysqlclient`
server: server.c
	gcc -pthread -o server server.c serverFunction.c protocol.c `pkg-config --libs mysqlclient`

clean:
	rm -f db client server