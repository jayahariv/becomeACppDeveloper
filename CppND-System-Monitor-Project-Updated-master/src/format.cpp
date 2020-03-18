#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int hour = seconds/3600;
    seconds = seconds%3600;
    int mints = seconds/60;
    seconds = seconds%60;

    return std::stoi(hour) + ":" + std::stoi(mints) + ":" + std::stoi(seconds); 
}