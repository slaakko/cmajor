#define MAX_FILE_MAPPINGS 1024

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct { int in_use; HANDLE file_handle; HANDLE mapped_handle; void* data; } mapped_file_data;

mapped_file_data file_mappings[MAX_FILE_MAPPINGS];

static int get_file_mapping_handle()
{
    int file_mapping_handle = 0;
    while (file_mapping_handle < MAX_FILE_MAPPINGS)
    {
        if (!file_mappings[file_mapping_handle].in_use)
        {
            return file_mapping_handle;
        }
        ++file_mapping_handle;
    }
    return -1;
}

int create_file_mapping(const char* filePath, const char** begin, const char** end)
{
    int file_mapping_handle = get_file_mapping_handle();
    if (file_mapping_handle == -1)
    {
        return -1;
    }
    HANDLE handle = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return -2;
    }
    DWORD fileSize = GetFileSize(handle, NULL);
    if (fileSize == INVALID_FILE_SIZE)
    {
        CloseHandle(handle);
        return -3;
    }
    HANDLE mapped_handle = CreateFileMapping(handle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapped_handle == NULL)
    {
        CloseHandle(handle);
        return -4;
    }
    void* data = MapViewOfFileEx(mapped_handle, FILE_MAP_READ, 0, 0, 0, NULL);
    if (data == NULL)
    {
        CloseHandle(mapped_handle);
        CloseHandle(handle);
        return -5;
    }
    *begin = (const char*)data;
    *end = (const char*)data + fileSize;
    file_mappings[file_mapping_handle].in_use = 1;
    file_mappings[file_mapping_handle].file_handle = handle;
    file_mappings[file_mapping_handle].mapped_handle = mapped_handle;
    file_mappings[file_mapping_handle].data = data;
    return file_mapping_handle;
}

int close_file_mapping(int file_mapping_handle)
{
    if (file_mapping_handle < 0 || file_mapping_handle >= MAX_FILE_MAPPINGS) return -1;
    UnmapViewOfFile(file_mappings[file_mapping_handle].data);
    file_mappings[file_mapping_handle].data = NULL;
    CloseHandle(file_mappings[file_mapping_handle].mapped_handle);
    file_mappings[file_mapping_handle].mapped_handle = INVALID_HANDLE_VALUE;
    CloseHandle(file_mappings[file_mapping_handle].file_handle);
    file_mappings[file_mapping_handle].file_handle = INVALID_HANDLE_VALUE;
    file_mappings[file_mapping_handle].in_use = 0;
    return 0;
}

#elif defined(__linux) || defined(__unix) || defined(__posix)

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>      // mmap, munmap.
#include <sys/stat.h>
#include <sys/types.h>     // struct stat.
#include <unistd.h>        // sysconf.

typedef struct { int in_use; int file_handle; void* data; size_t length; } mapped_file_data;

mapped_file_data file_mappings[MAX_FILE_MAPPINGS];

static int get_file_mapping_handle()
{
    int file_mapping_handle = 0;
    while (file_mapping_handle < MAX_FILE_MAPPINGS)
    {
        if (!file_mappings[file_mapping_handle].in_use)
        {
            return file_mapping_handle;
        }
        ++file_mapping_handle;
    }
    return -1;
}

int create_file_mapping(const char* filePath, const char** begin, const char** end)
{
    int file_mapping_handle = get_file_mapping_handle();
    if (file_mapping_handle == -1)
    {
        return -1;
    }
    int handle = open(filePath, O_RDONLY, S_IRWXU);
    if (handle == -1)
    {
        return -2;
    }
    struct stat statBuf;
    int result = fstat(handle, &statBuf);
    if (result == -1)
    {
        close(handle);
        return -3;
    }
    size_t size = statBuf.st_size;
    void* data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, handle, 0);
    if (data == MAP_FAILED)
    {
        close(handle);
        return -5;
    }
    *begin = (const char*)data;
    *end = (const char*)data + size;
    file_mappings[file_mapping_handle].in_use = 1;
    file_mappings[file_mapping_handle].file_handle = handle;
    file_mappings[file_mapping_handle].data = data;
    file_mappings[file_mapping_handle].length = size;
    return file_mapping_handle;
}

int close_file_mapping(int file_mapping_handle)
{
    if (file_mapping_handle < 0 || file_mapping_handle >= MAX_FILE_MAPPINGS) return -1;
    munmap(file_mappings[file_mapping_handle].data, file_mappings[file_mapping_handle].length);
    file_mappings[file_mapping_handle].data = NULL;
    file_mappings[file_mapping_handle].length = 0;
    close(file_mappings[file_mapping_handle].file_handle);
    file_mappings[file_mapping_handle].file_handle = -1;
    file_mappings[file_mapping_handle].in_use = 0;
    return 0;
}

#else

    #error unknown platform

#endif
