build: ../src/*.cpp ../src/*.h
	g++ -std=c++20 -g -Wall -Wextra -Wpedantic ../src/main.cpp -o zad3.out

run: build
	./zad3.out file ../graphs/tsplib/ftv47.atsp

plot: run costs.csv
	python ../src/plot.py costs.csv
