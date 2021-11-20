.DEFAULT_GOAL := all

all: aux sequential process thread

aux: src/aux.cpp
	g++ -o aux src/aux.cpp -g -fsanitize=address

sequential: src/sequential.cpp
	g++ -o sequential src/sequential.cpp -g -fsanitize=address

process: src/process.cpp
	g++ -o process src/process.cpp -g -fsanitize=address -Wall

thread: src/threads.cpp
	g++ -o thread src/threads.cpp -g -fsanitize=address -Wall -pthread