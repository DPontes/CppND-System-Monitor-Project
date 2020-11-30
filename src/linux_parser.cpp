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
  string line;
  string key;
  double value;
  double total_mem{0};
  double free_mem{0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total_mem = value;
        } else if (key == "MemFree:") {
          free_mem = value;
        }
      }
    }
  }
  return (total_mem - free_mem) / total_mem;
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
  string line, key;
  int value{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

int LinuxParser::RunningProcesses() {
  string line, key;
  int value{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Command(int pid) {
  string line, cmd;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  string line, key;
  int ram_value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> ram_value) {
        if (key == "VmSize:") {
          return to_string(ram_value / 1000);
        }
      }
    }
  }
  return to_string(ram_value);
}

string LinuxParser::Uid(int pid) {
  string line, key, uid_value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> uid_value) {
        if (key == "Uid:") {
          return uid_value;
        }
      }
    }
  }
  return uid_value;
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
