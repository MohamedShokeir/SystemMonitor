#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

#include <cmath>

using std::vector;

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 public:
  float idle_p{0.f};
  float total_p{0.f};
};

#endif