CC=g++
FLAG=-g
target:
	$(CC) $(FLAG) capacityScaling.cpp -o test
clean:
	rm test