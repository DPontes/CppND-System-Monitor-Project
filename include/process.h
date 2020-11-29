#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process (int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

 private:
  int pid_;
  std::string cmd_;
  std::string ram_;
};

#endif
