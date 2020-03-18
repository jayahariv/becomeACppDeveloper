#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { return std::stof(LinuxParser::CpuUtilization().front()); }