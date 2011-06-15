

#ifndef HAS_BOOLEAN
#ifndef BOOLEAN_ALREADY_DEFINED
enum bool {false = 0, true = 1};
#define INT_TO_BOOL(x) ((x) == 1 ? true : false)
#define BOOLEAN_ALREADY_DEFINED
#endif
#else
#ifndef BOOLEAN_ALREADY_DEFINED
#define INT_TO_BOOL(x) x
#define BOOLEAN_ALREADY_DEFINED
#endif
#endif
