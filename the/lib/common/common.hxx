#pragma once

#define INLINE inline

#if defined(__GNUC__)
# define PP_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
# define PP_FUNCTION __FUNCSIG__
#else
# define PP_FUNCTION __func__
#endif

#define PP_QUOTE(x)     #x
#define PP_STRINGIZE(x) PP_QUOTE(x)
#define PP_WHERE        __FILE__ ":" PP_STRINGIZE(__LINE__)
