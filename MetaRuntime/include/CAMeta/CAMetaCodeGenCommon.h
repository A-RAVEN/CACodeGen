#pragma once
#if _WIN32
#ifdef _CAMETA_DLLEXPORT
#define CAINTERFACE __declspec(dllexport)
#else
#define CAINTERFACE __declspec(dllimport)
#endif
#else
#define CAINTERFACE
#endif