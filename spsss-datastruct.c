
#if !defined(SPSSS_DATASTRUCT_H)
#define SPSSS_DATASTRUCT_H

#define LINE_MAX 1024
#define SUCCESSFUL_COMPILATION 1
#define FAILED_COMPILATION 0
#define FILE_FLAGS (O_WRONLY | O_CREAT | O_TRUNC)
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

typedef struct Spsss_commands{
    char **compile;
    char **test;
    int compile_size;
    int test_size;
} Spsss_commands;

#endif
