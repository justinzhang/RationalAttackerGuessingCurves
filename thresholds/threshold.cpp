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
  std::vector<std::pair<double,double>> smallVkRatios = {
    // std::make_pair(100.0,1.0), 
    // std::make_pair(250.0,1.0), 
    // std::make_pair(500.0,1.0), 
    // std::make_pair(750.0,1.0),
    // std::make_pair(1000.0,1.0),
    std::make_pair(1500.0,1.0)
    // std::make_pair(2000.0,1.0),
    // std::make_pair(5000.0,1.0)
  };
  std::vector<std::pair<double,double>> largeVkRatios = {
    // std::make_pair(5000.0,1.0), //13-120
    // std::make_pair(10000.0,1.0), // 56-320, by 2
    // std::make_pair(50000.0 /,1.0) // 800-1000, by 4
  };
  for (std::pair<double,double> vk : largeVkRatios) {
    break; //for when testing smaller
    double v = vk.first;
    double k = vk.second;
    int N_star = v / (10*k) ;
    std::ofstream myFile("pwd_cracked/large" + std::to_string((int64_t)(round(v))) + ".txt",std::ofstream::app);
    for (int B = 10; B < N_star; B += 2) {
      if (B >= N_star) break;
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
        // std::cout << currB << std::endl;
        lamBounds[std::stoi(currB)] = {std::stod(currMin), std::stod(currMax)};
        // std::cout << currB << "," << currMin << "," << currMax << std::endl; 
      }
      lamBoundsFile.close();

      std::pair<int, double> currMin = threshold_LP(v, k, B, N_star, lamBounds, GRB_MINIMIZE);
      std::pair<double,double> currSol;
      if (currMin.first != 0) {
        std::cout << "unable to minimize B = " << std::to_string(B) << std::endl; 
        currSol.first = 0.0;
      } else {
        std::cout << "minimization works for B = " << std::to_string(B) << ", sol = " << std::to_string(currMin.second) << std::endl; 
        currSol.first = currMin.second;
      }

      std::pair<int, double> currMax = threshold_LP(v, k, B, N_star, lamBounds, GRB_MAXIMIZE);
      if (currMax.first != 0) {
        std::cout << "unable to maximize B = " << std::to_string(B) << std::endl; 
        currSol.second = 1.0;
      } else {
        std::cout << "maximize works for B = " << std::to_string(B) << ", sol = " << std::to_string(currMax.second) << std::endl; 
        currSol.second = currMax.second;
      }

      //write currMin and currMax incrementally to a file
      if (myFile.is_open()) {
        myFile << std::to_string(B) << "," << std::to_string(currMin.second) << "," << 
          std::to_string(currMax.second)  << std::endl;
      } else {
        std::cerr << "Unable to open file." << std::endl;
        return 1;
      }
      
    }
    myFile.close();
  }

  for (std::pair<double,double> vk : smallVkRatios) {
    double v = vk.first;
    double k = vk.second;
    int N_star = 1 *  v / k + 100;
    std::ofstream myFile("pwd_cracked/small" + std::to_string((int64_t)(round(v))) + ".txt",std::ofstream::app);
    for (int B = 10; B < N_star; B += 2) {
      if (B >= N_star) break;
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
        // std::cout << currB << std::endl;
        lamBounds[std::stoi(currB)] = {std::stod(currMin), std::stod(currMax)};
        // std::cout << currB << "," << currMin << "," << currMax << std::endl; 
      }
      lamBoundsFile.close();

      std::pair<int, double> currMin = threshold_LP(v, k, B, N_star, lamBounds, GRB_MINIMIZE);
      std::pair<double,double> currSol;
      if (currMin.first != 0) {
        std::cout << "unable to minimize B = " << std::to_string(B) << std::endl; 
        currSol.first = 0.0;
      } else {
        std::cout << "minimization works for B = " << std::to_string(B) << ", sol = " << std::to_string(currMin.second) << std::endl; 
        currSol.first = currMin.second;
      }

      std::pair<int, double> currMax = threshold_LP(v, k, B, N_star, lamBounds, GRB_MAXIMIZE);
      if (currMax.first != 0) {
        std::cout << "unable to maximize B = " << std::to_string(B) << std::endl; 
        currSol.second = 1.0;
      } else {
        std::cout << "maximize works for B = " << std::to_string(B) << ", sol = " << std::to_string(currMax.second) << std::endl; 
        currSol.second = currMax.second;
      }

      //write currMin and currMax incrementally to a file
      if (myFile.is_open()) {
        myFile << std::to_string(B) << "," << std::to_string(currMin.second) << "," << 
          std::to_string(currMax.second)  << std::endl;
      } else {
        std::cerr << "Unable to open file." << std::endl;
        return 1;
      }

      if (currMax.first != 0 || currMin.first != 0) break; 
      
    }
    myFile.close();
  }

}

