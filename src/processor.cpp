#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 

  float totalTime = LinuxParser::Jiffies();
  float activeTime = LinuxParser::ActiveJiffies() ;
  float result = 1.0 * (activeTime/totalTime);
  return result;
}