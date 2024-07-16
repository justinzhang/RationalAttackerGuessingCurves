#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <algorithm>
#include <vector>

#include "pwdio.hpp"
#include "distribution.hpp"
#include "helpers.hpp"
#include "bounds.hpp"
#include "lp_bounds.hpp"
#include "plotting.hpp"

std::string trim(std::string);
std::string prompt(std::string);
int create_sample();
int choose_sample();
void confirm_exit();
std::string sample_menu();
std::string main_menu();
std::string plot_bound_menu();

std::vector<std::string> main_menu_choices = {
  "Add a password sample",
  "Choose from existing samples",
  "Exit"
};

std::vector<std::string> sample_menu_choices = {
  "Bound guessing curve",
  "Plot guessing curve",
  "Write sample to file (freqcount format)",
  "Back"
};

std::vector<std::string> plot_bound_choices = {
  "Best bound",
  "All bounds"
}

std::vector<std::string> bounds

std::vector<dist_t> samples;
int cur_id = -1;

std::string trim(std::string s) {
  if (s.size() == 0) {
    return s;
  }
  auto beg = s.find_first_not_of(" \t\n\r");
  auto end = s.find_last_not_of(" \t\n\r");
  return s.substr(beg, end-beg+1);
}

std::string prompt(std::string choices) {
  if (choices.size() == 0) {
    std::cout << "> ";
  }
  else {
    std::cout << choices << ": ";
  }
  std::cout << std::flush;
  std::string res;
  getline(std::cin, res);
  return trim(res);
}

void horizontal() {
  std::cout << "\n--------------------------------" << std::endl;
}

int create_sample() {
  std::string filename = "";
  std::string filetype = "";

  std::cout << "Enter filename containing password sample." << std::endl;
  while (filename.size() == 0) {
    filename = prompt("");
  }

  std::cout << "Specify the format of the password sample file.\n1. plain\n2. pwdfreq\n3. freqcount" << std::endl;
  while (filetype != "1" && filetype != "2" && filetype != "3") {
    filetype = prompt("[1/2/3]");
  }

  if (filetype == "1") {
    filetype = "plain";
  }
  else if (filetype == "2") {
    filetype = "pwdfreq";
  }
  else {
    filetype = "freqcount";
  }

  dist_t new_samp;
  set_verbose(new_samp, false);
  if (read_file(new_samp, filename, filetype)) {
    samples.push_back(new_samp);
    cur_id = (int) samples.size() - 1;
    return cur_id;
  }
  return -1;
}

int choose_sample() {
  if (samples.size() == 0) {
    return -1;
  }
  std::string choices = "[";
  for (int i=0; i<samples.size(); ++i) {
    std::cout << i+1 << ". " << samples[i].filename << " (" << samples[i].filetype << ")" << std::endl;
    choices += std::to_string(i+1) + ((i+1 != samples.size()) ? "/" : "]");
  }

  std::string choice = "";
  while (true) {
    choice = prompt(choices);
    bool valid = false;
    for (int i=1; i<=samples.size(); ++i) {
      if (choice == std::to_string(i)) {
        valid = true;
        break;
      }
    }
    if (valid) {
      break;
    }
  }

  cur_id = std::stoi(choice) - 1;
  return cur_id;
}

void confirm_exit() {
  std::cout << "Confirm exit? (unsaved data would be lost)" << std::endl;
  std::string choice = "";
  while (choice != "y" && choice != "n") {
    choice = prompt("[y/n]");
  }
  if (choice == "y") {
    exit(0);
  }
}

std::string sample_menu() {
  horizontal();
  std::cout << "Current Sample: " << samples[cur_id].filename << " (" << samples[cur_id].filetype << ")" << std::endl;
  for (int i=0; i<sample_menu_choices.size(); ++i) {
    std::cout << i+1 << ". " << sample_menu_choices[i] << std::endl;
  }
  std::string choice = "";
  while (choice != "1" && choice != "2" && choice != "3" && choice != "4") {
    choice = prompt("[1/2/3/4]");
  }
  return choice;
}

std::string plot_bound_menu() {
  std::cout << "Choose bounds to plot." << std::endl;
  for (int i=0; i<plot_bound_choices.size(); ++i) {
    std::cout << i+1 << ". " << plot_bound_choices[i] << std::endl;
  }
  std::string choice = "";
  while (choice != "1" && choice != "2") {
    choice = prompt("[1/2]");
  }
  return choice;
}

std::string main_menu() {
  horizontal();
  for (int i=0; i<main_menu_choices.size(); ++i) {
    std::cout << i+1 << ". " << main_menu_choices[i] << std::endl;
  }
  std::string choice = "";
  while (choice != "1" && choice != "2" && choice != "3") {
    choice = prompt("[1/2/3]");
  }
  return choice;
}

int main() {
  std::string choice;
  while (true) {
    choice = main_menu();
    if (choice == "3") {
      confirm_exit();
    }
    else {
      if (choice == "1") {
        if (create_sample() == -1) {
          std::cout << "Error: can't read from the file." << std::endl;
          continue;
        }
      }
      else {
        if (samples.size() == 0) {
          std::cout << "Error: No samples present. Please add sample first." << std::endl;
          continue;
        }
        else {
          choose_sample();
        }
      }
      while (true) {
        choice = sample_menu();
        if (choice == "4") {
          break;
        }
        else {
          if (choice == "1") {
          }
          else if (choice == "2") {
            std::string filename = "";
            std::cout << "Enter filename to write to." << std::endl;
            while (filename.size() == 0) {
              filename = prompt("");
            }
            choice = plot_bound_menu();
            if (choice == "1") {
            }
            else {
            }
          }
          else if (choice == "3") {
            std::string filename = "";
            std::cout << "Enter filename to write to." << std::endl;
            while (filename.size() == 0) {
              filename = prompt("");
            }
            if (!write_freqcount(samples[cur_id], filename)) {
              std::cout << "Error: can't open the file." << std::endl;
            }
          }
        }
      }
    }
  }
}

