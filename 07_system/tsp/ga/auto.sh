g++ -std=c++2a -O3 -Wall -Wextra -Wconversion -pedantic server_for_ga.cpp tsp.cpp -pthread -o server.out
g++ -std=c++2a -O3 -Wall -Wextra -Wconversion -pedantic main.cpp tsp.cpp -pthread -o main.out
