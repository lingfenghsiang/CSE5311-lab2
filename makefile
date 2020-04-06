CC=g++
FLAG=-g -w
target:
	$(CC) $(FLAG) capacityScaling.cpp -o test
clean:
	rm test