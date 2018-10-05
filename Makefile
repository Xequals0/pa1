FLAGS = -W -Wall -pthread -lboost_serialization -lrt
CC = g++

all: mapred

mapred: mapred.o
	$(CC) $(FLAGS) -o mapred mapred.o

mapred.o: mapred.cpp mapred.h
	$(CC) $(FLAGS) -c mapred.cpp

clean:
	rm -rf *.o mapred
