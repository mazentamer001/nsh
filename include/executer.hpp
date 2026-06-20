#pragma once

#include <string>
#include <vector>

class Executer {
 private:
  static int jobID;
 public:
  static void execute(const std::vector<std::string>& tokens);
};
