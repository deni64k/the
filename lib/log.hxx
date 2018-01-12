#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

namespace astro {

namespace streams {
extern std::ofstream Null;
}

void Log(char const *format, ...);

}

#define PANIC() if (true) std::cerr << "PANIC: " << __func__ << ':' << __LINE__ << ": "
#define ERROR() if (true) std::cerr << "ERROR: " << __func__ << ':' << __LINE__ << ": "
#define WARN()  if (true) std::cerr << "WARN : " << __func__ << ':' << __LINE__ << ": "
#define INFO()  if (true) std::cerr << "INFO : " << __func__ << ':' << __LINE__ << ": "
#define DEBUG() if (true) std::cerr << "DEBUG: " << __func__ << ':' << __LINE__ << ": "
