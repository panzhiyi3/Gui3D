#ifndef GUI3DCOMMON_H
#define GUI3DCOMMON_H

#if defined(_WIN32) || defined(WIN32)
    #ifdef _GUI3D_EXPORTS
        #define GUI3D_EXPORTS __declspec(dllexport)
    #else
        #define GUI3D_EXPORTS __declspec(dllimport)
    #endif
#else
    #ifdef _GUI3D_EXPORTS
        #define GUI3D_EXPORTS 
    #else
        #define GUI3D_EXPORTS 
    #endif
#endif

#endif
