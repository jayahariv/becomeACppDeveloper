#include "processor.h"
#include "linux_parser.h"

std::vector<float> Processor::Utilization() { 
    std::vector<std::string> list = LinuxParser::CpuUtilization();
    std::vector<float> res;
    for (std::string val: list) {
        res.push_back(std::stof(val));
    }
    return res; 
}