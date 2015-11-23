# SocketPractice
ipv6, pthread
# compile client and server #
gcc -o client client.c
gcc -o server server.c -lpthread

# start server with port #
./server 2222

# start client with server's address(localhost for example), port, and username #
./client localhost 2222 username
