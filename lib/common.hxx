#pragma once

#define INLINE inline

#if defined(__GNUC__)
# define PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
# define PRETTY_FUNCTION __FUNCSIG__
#else
# define PRETTY_FUNCTION __func__
#endif
