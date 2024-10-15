#pragma once

#if defined(__CASCADE_REFLECTION_PARSER__)
#define CAMETA(...) __attribute__((annotate("CAMETA_PROPS:" #__VA_ARGS__)))
#else
// //combine macro strings
// #define CA_COMBINE(PREFIX, ID) PREFIX##ID
// //expand macro once
// #define CA_COMBINE_EXPAND1(PREFIX, ID) CA_COMBINE(PREFIX, ID)
// #define CAMETA(...) static void CA_COMBINE_EXPAND1(_CAMetaInfo_, __COUNTER__)(){__VA_ARGS__;}
#define CAMETA(...)
#endif
