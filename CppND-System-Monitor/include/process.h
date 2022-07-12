#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process(int pid) : pid_(pid) {}
  int Pid();
  std::string User();
  std::string Command();
  void CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;
  float CpuUsage() const;

 private:
  int pid_;
  float cpu_usage_{0.0};

  float prev_total_time_{0.0};
  float prev_seconds_{0.0};
  float current_total_time_{0.0};
  float current_seconds_{0.0};
};

#endif