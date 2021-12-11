
#undef TEST
#define LOG

// uncomment this to enable unit tests instead of normal code execution
#define TEST

// logging is deactivated when running unit tests
#ifdef TEST
#undef LOG
#endif

// uncomment this to activate logging depite unit tests
// #define LOG
