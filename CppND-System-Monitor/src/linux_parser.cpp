#include "linux_parser.h"

#include <dirent.h>
#include <math.h>
#include <unistd.h>

#include <filesystem>
#include <numeric>
#include <sstream>
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
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;

  std::string path = kProcDirectory;
  for (auto& dir : std::filesystem::directory_iterator(path)) {
    auto directory_name = dir.path().stem().string();
    if (std::all_of(directory_name.begin(), directory_name.end(), isdigit)) {
      int pid = std::stoi(directory_name);
      pids.emplace_back(pid);
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float memoryUtilization{0.0};
  string memInfoFileAddress = kProcDirectory + kMeminfoFilename;
  try {
    long memTotal = std::stol(
        LinuxParser::GetValueFromFile(memInfoFileAddress, "MemTotal"));
    long memFree =
        std::stol(LinuxParser::GetValueFromFile(memInfoFileAddress, "MemFree"));
    long buffers =
        std::stol(LinuxParser::GetValueFromFile(memInfoFileAddress, "Buffers"));
    long cached =
        std::stol(LinuxParser::GetValueFromFile(memInfoFileAddress, "Cached"));
    memoryUtilization =
        (float)(memTotal - memFree - (buffers + cached)) / (float)memTotal;
  } catch (...) {
    memoryUtilization = 0.0;
  }

  return memoryUtilization;
}

long LinuxParser::UpTime() {
  try {
    return std::stol(
        LinuxParser::GetValueFromFile(kProcDirectory + kUptimeFilename));
  } catch (...) {
    return 0;
  }
}

long LinuxParser::Jiffies() {
  auto cpu_jiffies = CpuUtilization();
  return std::accumulate(cpu_jiffies.begin(), cpu_jiffies.end(), 0);
}

long LinuxParser::ActiveJiffies(int pid) {
  vector<long> proc_cpu_times{};
  string value{""};
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 22; i++) {
      linestream >> value;
      if (i == 14 || i == 15 || i == 16 || i == 17) {
        try {
          proc_cpu_times.emplace_back(std::stol(value));
        } catch (...) {
        }
      }
    }
  }
  return std::accumulate(proc_cpu_times.begin(), proc_cpu_times.end(), 0);
}

long LinuxParser::ActiveJiffies() {
  auto cpu_jiffies = CpuUtilization();
  return cpu_jiffies[CPUStates::kUser_] + cpu_jiffies[CPUStates::kNice_] +
         cpu_jiffies[CPUStates::kSystem_] + cpu_jiffies[CPUStates::kIRQ_] +
         cpu_jiffies[CPUStates::kSoftIRQ_] + cpu_jiffies[CPUStates::kSteal_];
}

long LinuxParser::IdleJiffies() {
  auto cpu_jiffies = CpuUtilization();
  return cpu_jiffies[CPUStates::kIdle_] + cpu_jiffies[CPUStates::kIOwait_];
}

vector<long> LinuxParser::CpuUtilization() {
  vector<long> cpu_times;
  string key;
  long value{0};
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key == "cpu") {
        while (linestream >> value) {
          cpu_times.emplace_back(value);
        }
      }
    }
  }
  return cpu_times;
}

int LinuxParser::TotalProcesses() {
  try {
    return std::stol(LinuxParser::GetValueFromFile(
        kProcDirectory + kStatFilename, "processes"));
  } catch (...) {
    return 0;
  }
}

int LinuxParser::RunningProcesses() {
  try {
    return std::stol(LinuxParser::GetValueFromFile(
        kProcDirectory + kStatFilename, "procs_running"));
  } catch (...) {
    return 0;
  }
}

string LinuxParser::Command(int pid) {
  string fileAddress = kProcDirectory + std::to_string(pid) + kCmdlineFilename;
  return LinuxParser::GetValueFromFile(fileAddress);
}

string LinuxParser::Ram(int pid) {
  string file_address = kProcDirectory + std::to_string(pid) + kStatusFilename;
  string memory_kb = LinuxParser::GetValueFromFile(file_address, "vmSize");
  try {
    long memory_mb = std::stol(memory_kb) / 1024;
    return std::to_string(memory_mb);
  } catch (...) {
    return "0";
  }
}

string LinuxParser::Uid(int pid) {
  string file_address = kProcDirectory + std::to_string(pid) + kStatusFilename;
  return LinuxParser::GetValueFromFile(file_address, "Uid");
}

string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string key;
  string value{""};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value;
}

long LinuxParser::UpTime(int pid) {
  string value{""};
  long uptime_tick{0};
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 22; i++) {
      linestream >> value;
      if (i == 22) {
        uptime_tick = std::stol(value);
      }
    }
  }
  return uptime_tick / sysconf(_SC_CLK_TCK);
}

string LinuxParser::GetValueFromFile(string fileAddress, string wantedKey) {
  string value{""};
  string key;
  string line;
  std::ifstream filestream(fileAddress);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == wantedKey) {
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::GetValueFromFile(string fileAddress) {
  string value{""};
  string line;
  std::ifstream filestream(fileAddress);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}
