#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/*
    Generic Functions to use when searching for information in files
*/

template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename, int pid) {
  string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + filename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if(key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
}

template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if(key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
}

template <typename T>
T getValueOfFile(std::string const &filename) {
  string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

/*
  -------------End-------------------
*/

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);
  float Free  = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}

long LinuxParser::UpTime() {
  string line;
  string timeSystemString = "";
  string timeIdleProcString;
  long timeSystem;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> timeSystemString >> timeIdleProcString;
    }
  }
  timeSystem = std::stol(timeSystemString);
  return timeSystem;
}

long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  static vector<string> cpuTimes;
  string line, value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value) { cpuTimes.push_back(value); }
  }

  long uTime = std::stol(cpuTimes[13]);
  long sTime = std::stol(cpuTimes[14]);
  long cuTime = std::stol(cpuTimes[15]);
  long csTime = std::stol(cpuTimes[16]);

  return uTime + sTime + cuTime + csTime;
}

long LinuxParser::ActiveJiffies() {
  static vector<string> CpuUtil = LinuxParser::CpuUtilization();

  return (stof(CpuUtil[kUser_]) + stof(CpuUtil[kNice_]) +
          stof(CpuUtil[kSystem_]) + stof(CpuUtil[kIRQ_]) +
          stof(CpuUtil[kSoftIRQ_]) + stof(CpuUtil[kSteal_]));
}

long LinuxParser::IdleJiffies() {
  static vector<string> CpuUtil = LinuxParser::CpuUtilization();

  return (stof(CpuUtil[kIdle_]) + stof(CpuUtil[kIOwait_]));
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuData;
  string line, key, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "cpu") {
      while (linestream >> value) cpuData.push_back(value);
    }
  }
  return cpuData;
}

int LinuxParser::TotalProcesses() {
  string procsTotal = "processes";

  return findValueByKey<int>(procsTotal, kStatFilename);
}

int LinuxParser::RunningProcesses() {
  string procsRunning = "procs_running";

  return findValueByKey<int>(procsRunning, kStatFilename);
}

string LinuxParser::Command(int pid) {
  return string(getValueOfFile<string>(to_string(pid) + kCmdlineFilename));
}

/*
  NOTE: using VmData instead of VmSize as indicated in the Guidelines on purpose
  Reason: VmSize sum of all virtual memory
          VmData exact physical memory being used as part of physical RAM
*/
string LinuxParser::Ram(int pid) {
  string ramData = "VmData";

  long ram = findValueByKey<long>(ramData, kStatusFilename, pid);
  ram = ram / 1024;
  return to_string(ram);
}

string LinuxParser::Uid(int pid) {
  string uidValue = "Uid:";

  return findValueByKey<string>(uidValue, kStatusFilename, pid);
}

string LinuxParser::User(int pid) {
  string line, user, _, uid;

  string uid_value = Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      while (linestream >> user >> _ >> uid) {
        if (uid == uid_value) {
          return user;
        }
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  string line, up_time;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int steps = 0; steps < 22; steps++) { linestream >> up_time; }
    return LinuxParser::UpTime() - std::stol(up_time) / sysconf(_SC_CLK_TCK);
  }

  return std::stol(up_time);
}
