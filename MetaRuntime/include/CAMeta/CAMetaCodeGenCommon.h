#ifndef CAMETA_CODEGEN_COMMON_H
#define CAMETA_CODEGEN_COMMON_H
//This File Is Used By C Interface Code Generator, Make Sure It Is C Compatible

#if _WIN32
#ifdef _CAMETA_DLLEXPORT
#define CAINTERFACE __declspec(dllexport)
#else
#define CAINTERFACE __declspec(dllimport)
#endif
#else
#define CAINTERFACE
#endif

#endif//CAMETA_CODEGEN_COMMON_H