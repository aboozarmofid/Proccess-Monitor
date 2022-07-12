#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() { this->processes_ = vector<Process>{}; }

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  auto pids = LinuxParser::Pids();
  // remove stopped processes
  for (auto& p : this->processes_) {
    if (std::find(pids.begin(), pids.end(), p.Pid()) == pids.end()) {
      int pid = p.Pid();
      auto it =
          std::find_if(this->processes_.begin(), this->processes_.end(),
                       [&pid](Process& proc) { return proc.Pid() == pid; });
      auto index = std::distance(this->processes_.begin(), it);
      this->processes_.erase(this->processes_.begin() + index);
    }
  }
  ///////////////////////////
  for (int i : pids) {
    auto it = std::find_if(this->processes_.begin(), this->processes_.end(),
                           [&i](Process& proc) { return proc.Pid() == i; });
    if (it != this->processes_.end()) {
      // procces is in the list. just update cpu utilization
      auto index = std::distance(this->processes_.begin(), it);
      this->processes_[index].CpuUtilization();
    } else {
      // process is not in the list. insert process to vector
      Process proc(i);
      proc.CpuUtilization();
      this->processes_.emplace_back(proc);
    }
  }

  std::sort(this->processes_.begin(), this->processes_.end());
  return this->processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
