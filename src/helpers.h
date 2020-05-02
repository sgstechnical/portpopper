#ifndef _HELPERS_H_
#define _HELPERS_H_

#define GLOBAL_VARIABLE static
#define INTERNAL static
#define LOCAL_VARIABLE static
#define INLINE inline

#define KB(x) ((x)*1024)
#define MB(x) ((x)*1024*1024)

typedef uint32_t b32;
typedef float f32;

#define SETBUF() setvbuf(stdout, NULL, _IONBF, BUFSIZ)

#if DEBUG_ENABLED
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif


#define TEMP_BUFF_SIZE(x,s) char x[s]; x[0] = 0x00
#define TEMP_BUFF(b) TEMP_BUFF_SIZE(b,1024)
#define TEMP_PATH_BUFF(b) TEMP_BUFF_SIZE(b,MAX_PATH)
#define EXPAND_TEMP_BUFF(b) b,(sizeof(b))

#define create_string(b,...) snprintf(b,sizeof(b),__VA_ARGS__)

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define clearStruct(s) memory_clear((void*)&s,sizeof(s))


//##############################################################################
//@ String helpers
//##############################################################################
void string_clear(char* str);
size_t string_copy(char* d, const char* s, size_t destMax);
size_t string_length(const char* string);


//##############################################################################
//@ Memory helpers
//##############################################################################
void memory_clear(void*p,size_t s);
void memory_copy(void* dest, void* src, int64_t length);


#endif
