server : server.cpp rio.o client
	g++ server.cpp rio.o -std=c++11 -o server -g
client : client.cpp rio.o
	g++ client.cpp rio.o  -std=c++11 -o client -g
rio.o : rio.cpp
	g++ rio.cpp -c -std=c++11 -g


clean :
	rm *.o
	rm client
	rm server