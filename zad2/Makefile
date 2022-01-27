build: ../src/*.cpp ../src/*.h
	g++ -std=c++20 -g -O -Wall -Wextra -Wpedantic ../src/main.cpp -o zad2.out

run: build
	./zad2.out file graphs/tsplib/ftv47.atsp

plot: run costs.csv
	python ../src/plot.py costs.csv