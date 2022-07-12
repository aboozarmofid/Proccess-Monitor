#include "processor.h"

float Processor::Utilization() {
  long total_delta{0};
  long idle_delta{0};
  float cpu_percentage{0.0};

  this->current_total_giffies_ = LinuxParser::Jiffies();
  this->current_active_giffies_ = LinuxParser::ActiveJiffies();
  this->current_idle_giffies_ = LinuxParser::IdleJiffies();

  total_delta = this->current_total_giffies_ - this->prev_total_giffies_;
  idle_delta = this->current_idle_giffies_ - this->prev_idle_giffies_;

  cpu_percentage = float(total_delta - idle_delta) / float(total_delta);

  this->prev_total_giffies_ = this->current_total_giffies_;
  this->prev_active_giffies_ = this->current_active_giffies_;
  this->prev_idle_giffies_ = this->current_idle_giffies_;

  return cpu_percentage;
}