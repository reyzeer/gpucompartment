PROG=cpu_big_int

all: $(PROG)

$(PROG): $(PROG).o
	g++ $(PROG).o -o $(PROG) -pthread -std=c++11

$(PROG).o: $(PROG).cpp
	g++ -c $(PROG).cpp  -pthread -std=c++11

run:
	./$(PROG)

clean:
	rm *o $(PROG)

