#include "format.h"

#include <chrono>
#include <iomanip>
#include <string>

std::string Format::ElapsedTime(long seconds) {
  auto time_elapsed = std::chrono::seconds(seconds);
  auto h = std::chrono::duration_cast<std::chrono::hours>(time_elapsed);
  time_elapsed -= h;
  auto m = std::chrono::duration_cast<std::chrono::minutes>(time_elapsed);
  time_elapsed -= m;
  auto s = std::chrono::duration_cast<std::chrono::seconds>(time_elapsed);

  std::ostringstream result;
  result << std::setw(2) << std::setfill('0') << std::to_string(h.count())
         << ":" << std::setw(2) << std::setfill('0')
         << std::to_string(m.count()) << ":" << std::setw(2)
         << std::setfill('0') << std::to_string(s.count());
  return result.str();
}