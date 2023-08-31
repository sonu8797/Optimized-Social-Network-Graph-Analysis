all: main.cpp consumer.cpp producer.cpp main consumer producer

main:
	g++ main.cpp -o main

con:
	g++ consumer.cpp -o consumer

pro:
	g++ producer.cpp -o producer

clean:
	rm -f main consumer producer producer_out.txt 

