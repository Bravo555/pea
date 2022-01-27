#!/usr/bin/fish

for i in (seq 1 10)
    ./zad2.out file ../graphs/tsplib/rbg403.atsp
    mv costs.csv results/403/{$i}.csv
end