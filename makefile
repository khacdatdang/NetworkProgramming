all: client server db

db: createDB.c
	gcc -o db createDB.c `pkg-config --libs mysqlclient`
client: client.c
	gcc -o client client.c clientFunction.c `pkg-config --libs mysqlclient` -Wall 
server: server.c
	gcc -pthread -o server server.c serverFunction.c `pkg-config --libs mysqlclient`

clean:
	rm -f db client server
