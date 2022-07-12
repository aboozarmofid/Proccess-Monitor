#include "process.h"

#include <linux_parser.h>
#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

int Process::Pid() { return pid_; }

void Process::CpuUtilization() {
  float cpu_usage{0.0};
  float totalTime_delta{0.0};
  float seconds_delta{0.0};
  this->current_total_time_ = LinuxParser::ActiveJiffies(this->Pid());
  this->current_seconds_ =
      LinuxParser::UpTime() - LinuxParser::UpTime(this->Pid());
  totalTime_delta = this->current_total_time_ - this->prev_total_time_;
  seconds_delta = this->current_seconds_ - this->prev_seconds_;
  try {
    cpu_usage =
        (totalTime_delta / float(sysconf(_SC_CLK_TCK))) / float(seconds_delta);

  } catch (...) {
    cpu_usage = 0.0;
  }
  this->cpu_usage_ = cpu_usage;

  this->prev_total_time_ = this->current_total_time_;
  this->prev_seconds_ = this->current_seconds_;
}

std::string Process::Command() { return LinuxParser::Command(this->Pid()); }

std::string Process::Ram() { return LinuxParser::Ram(this->Pid()); }

std::string Process::User() { return LinuxParser::User(this->Pid()); }

long int Process::UpTime() {
  return LinuxParser::UpTime() - LinuxParser::UpTime(this->Pid());
}

float Process::CpuUsage() const { return this->cpu_usage_; }

bool Process::operator<(Process const& a) const {
  return a.cpu_usage_ < cpu_usage_;
}