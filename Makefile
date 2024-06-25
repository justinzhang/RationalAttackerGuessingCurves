# gurobi lp solver start
PLATFORM = macos_universal2
GUROBIPATH = /Library/gurobi1102/macos_universal2
INC = $(GUROBIPATH)/include/
CPPLIB = -L$(GUROBIPATH)/lib -lgurobi_c++ -lgurobi90
# gurobi lp solver end

CPP = g++
FLAGS = -std=c++20 -g

distribution.o: distribution.cpp distribution.hpp
	$(CPP) $(FLAGS) -c $<

pwdio.o: pwdio.cpp pwdio.hpp
	$(CPP) $(FLAGS) -c $<

helpers.o: helpers.cpp helpers.hpp
	$(CPP) $(FLAGS) -c $<

test: test.cpp distribution.o pwdio.o helpers.o
	$(CPP) $(FLAGS) -o test $^

main: distribution.o pwdio.o helpers.o
	$(CPP) $(FLAGS) -o main main.cpp $^

# lp:
# 	$(CPP) $(FLAGS) -o $@ $^ -I$(INC) $(CPPLIB) -lm

.PHONY: clean
clean:
	rm -f *.o main test
