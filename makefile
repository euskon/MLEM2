LEM2Prog: main.o
	g++ -std=c++14 -g -Wall main.o -o LEM2Prog

main.o: main.cpp Dataset.h Dataset.cpp Column.h Column.cpp
	g++ -std=c++14 -g -Wall -c main.cpp

##Column.o: Column.h Column.cpp
	##g++ -std=c++14 -g -Wall -c Column.cpp

clean:
	rm *.o LEM2Prog
