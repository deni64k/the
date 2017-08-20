#pragma once

namespace astro {

void Log(char const *format, ...);

}

#define DEBUG(format, args...) astro::Log("DEBUG: %s:%d: " format, __func__, __LINE__, ##args)
#define ERROR(format, args...) astro::Log("ERROR: " format, ##args)
#define INFO(format, args...)  astro::Log("INFO: " format, ##args)
