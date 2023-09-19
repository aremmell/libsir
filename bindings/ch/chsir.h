#ifndef _CHSIR_H_INCLUDED
# define _CHSIR_H_INCLUDED

# if defined(_CH_) || defined(__CH__)

#  include <dlfcn.h>

extern void* _Chsir_handle = dlopen("libsir.dl", RTLD_LAZY);

#  pragma exec _Chsir_handle ? 0 : \
    fprintf(_stderr, "Error: dlopen(): %s\n", dlerror()), \
    fprintf(_stderr, "       cannot get _Chsir_handle in %s:%d\n", \
            __FILE__, __LINE__);

void
_dlclose_sir(void) {
    dlclose(_Chsir_handle);
}

atexit(_dlclose_sir);

# endif

# include "../../include/sir.h"

#endif /* _CHSIR_H_INCLUDED */
