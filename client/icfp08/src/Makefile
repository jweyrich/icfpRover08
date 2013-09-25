.PHONY: clean test

test: icfpRover
	./icfpRover 127.0.0.1 1234

icfpRover: socket.o vector2.o main.o
	$(CXX) -lpthread -o $@ $^ -Wall -Wextra

main.o: main.cpp
socket.o: socket.cpp socket.h common.h
vector2.o: vector2.h

clean:
	rm -rf *.o icfpRover
