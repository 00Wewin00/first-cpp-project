CXX = g++
# Оставили только стандарт, без лишних предупреждений
CXXFLAGS = -std=c++20 -g

all: server client

server: server.cpp db.cpp db.h
	$(CXX) $(CXXFLAGS) server.cpp db.cpp -o server -lsqlite3

client: client.cpp
	$(CXX) $(CXXFLAGS) client.cpp -o client

clean:
	rm -f server client