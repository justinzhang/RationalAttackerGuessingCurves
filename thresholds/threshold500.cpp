#include <iostream>

#include "distribution.hpp"
#include "pwdio.hpp"
#include "bounds.hpp"
#include "lp_bounds.hpp"
#include "wrappers.hpp"
#include "plotting.hpp"
#include <fstream>
#include "gurobi_c++.h"

int main() {
  std::ofstream myFile("iterativeB500.txt",std::ofstream::app);
  
  double v = 500.0;
  double k = 1.0;
  int N_star = 10000; // v/k * 100 + 0 
  //for dataset size N_star, percentage threshold P, guessing number B
  for (int B = 1; B <= N_star; B += 1) {
    std::map<int,std::pair<double,double>> lamBounds;

    //build lambdaI dataset by reading file
    std::ifstream lamBoundsFile("optimal-bounds.txt");
    std::string currLine; 
    while (std::getline(lamBoundsFile, currLine)) {
      size_t firstCommaIndex = currLine.find(",");
      std::string currB = currLine.substr(0,firstCommaIndex);

      std::string currLine2 = currLine.substr(firstCommaIndex + 1); 
      size_t secondCommaIndex = currLine2.find(",");
      std::string currMin = currLine2.substr(0,secondCommaIndex);

      std::string currMax = currLine2.substr(secondCommaIndex + 1); 
      
      //based on values i do not expect exceptions
      lamBounds[std::stoi(currB)] = {std::stod(currMin), std::stod(currMax)};
      // std::cout << currB << "," << currMin << "," << currMax << std::endl; 
    }

    std::pair<int, double> currMin = threshold_LP(v, k, B, N_star, lamBounds, GRB_MINIMIZE);
    std::pair<double,double> currSol;
    if (currMin.first != 0) {
      std::cout << "unable to minimize B = " << std::to_string(B); 
      currSol.first = 0.0;
    } else {
      std::cout << "minimization works for B = " << std::to_string(B) << ", sol = " << std::to_string(currMin.second); 
      currSol.first = currMin.second;
    }

    std::pair<int, double> currMax = threshold_LP(v, k, B, N_star, lamBounds, GRB_MAXIMIZE);
    if (currMax.first != 0) {
      std::cout << "unable to maximize B = " << std::to_string(B); 
      currSol.second = 1.0;
    } else {
      std::cout << "maximize works for B = " << std::to_string(B) << ", sol = " << std::to_string(currMax.second); 
      currSol.second = currMax.second;
    }

    //TODO: store these currSol values to then do a min/max

    //write currMin and currMax incrementally to a file
    if (myFile.is_open()) {
      myFile << std::to_string(B) << "," << std::to_string(currMin.second) << "," << 
      std::to_string(currMax.second)  << std::endl;
      std::ofstream myFileCurr("presentation/" + std::to_string(B) + ".txt",std::ofstream::app);

    } else {
      std::cerr << "Unable to open file." << std::endl;
      return 1;
    }

  }

}

