#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <cmath>

#include "linux_parser.h"

using std::vector;

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float idle_p{0.f};
  float active_p{0.f};
};

#endif