#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "linux_parser.h"

class Processor {
 public:
  float Utilization();

 private:
  long prev_idle_giffies_{0};
  long prev_active_giffies_{0};
  long prev_total_giffies_{0};

  long current_idle_giffies_{0};
  long current_active_giffies_{0};
  long current_total_giffies_{0};
};

#endif