#ifdef WIN32
    #include <ptbuildopts_win32.h>
#else
    #ifdef _WIN32_
        #include <ptbuildopts_win32.h>
    #else
       #ifdef __REDHAT__
       		#ifdef _FEDORA_
       				#include <ptbuildopts_linux_x86_f4.h>
       		#else	
          		#include <ptbuildopts_linux_x86_r9.h>
          #endif
       #else
       		#include <ptbuildopts_linux_ppc.h>
       #endif
    #endif
#endif
