#include "processor.h"
#include "linux_parser.h"

std::vector<float> Processor::Utilization() { 
    return LinuxParser::CpuUtilization(); 
}