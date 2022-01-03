zad2.out: src/sa.cpp
	g++ -std=c++20 -g -O -Wall -Wextra -Wpedantic src/main.cpp -o zad2.out

run: zad2.out
	./zad2.out file graphs/tsp_10.txt

plot: run
	python src/plot.py costs.csv