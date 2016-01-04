#ifdef _WIN32
    #if defined(__MINGW64__)
        #include "bzip2/windows/x64/bzlib.h"
    #else
        #include "bzip2/windows/x86/bzlib.h"
    #endif
#elif defined(__linux) || defined(__unix) || defined(__posix)
    #if defined(__x86_64__)
        #include "bzip2/linux/x86_64/bzlib.h"
    #else
        #include "bzip2/linux/i686/bzlib.h"
    #endif
#else
    #error unknown platform
#endif
#include <stdlib.h>

#define COMPRESS    0
#define DECOMPRESS  1

int bz2_init(int mode, int compressionLevel, int compressionWorkFactor, void** handle)
{
    int ret = BZ_OK;
    if (!handle)
    {
        ret = BZ_MEM_ERROR;
    }
    else
    {
        bz_stream* stream = (bz_stream*)malloc(sizeof(bz_stream));
        switch (mode)
        {
            case COMPRESS:
            {
                stream->bzalloc = NULL;
                stream->bzfree = NULL;
                stream->opaque = NULL;
                ret = BZ2_bzCompressInit(stream, compressionLevel, 0, compressionWorkFactor);
                break;
            }
            case DECOMPRESS:
            {
                stream->bzalloc = NULL;
                stream->bzfree = NULL;
                stream->opaque = NULL;
                ret = BZ2_bzDecompressInit(stream, 0, 0);
                break;
            }
        }
        if (ret != BZ_OK)
        {
            free(stream);
            *handle = NULL;
        }
        else
        {
            *handle = stream;
        }
    }
    return ret;
}

void bz2_done(int mode, void* handle)
{
    bz_stream* strm = (bz_stream*)handle;
    switch (mode)
    {
        case COMPRESS:
        {
            BZ2_bzCompressEnd(strm);
            break;
        }
        case DECOMPRESS:
        {
            BZ2_bzDecompressEnd(strm);
            break;
        }
    }    
    free(strm);
}

void bz2_set_input(void* inChunk, int inAvail, void* handle)
{
    bz_stream* strm = (bz_stream*)handle;
    strm->next_in = (char*)inChunk;
    strm->avail_in = inAvail;
}

int bz2_compress(void* outChunk, int outChunkSize, int* have, int* outAvail, void* handle, int action)
{
    bz_stream* strm = (bz_stream*)handle;
    strm->next_out = outChunk;
    strm->avail_out = outChunkSize;
    int ret = BZ2_bzCompress(strm, action);
    *have = outChunkSize - strm->avail_out;
    *outAvail = strm->avail_out;
    return ret;
}

int bz2_decompress(void* outChunk, int outChunkSize, int* have, int* outAvail, int* inAvail, void* handle)
{
    bz_stream* strm = (bz_stream*)handle;
    strm->next_out = outChunk;
    strm->avail_out = outChunkSize;
    int ret = BZ2_bzDecompress(strm);
    *have = outChunkSize - strm->avail_out;
    *outAvail = strm->avail_out;
    *inAvail = strm->avail_in;
    return ret;
}

const char* bz2_retval_str(int retVal)
{
    switch (retVal)
    {
        case BZ_OK: return "BZ_OK";
        case BZ_RUN_OK: return "BZ_RUN_OK";
        case BZ_FLUSH_OK: return "BZ_FLUSH_OK";
        case BZ_FINISH_OK: return "BZ_FINISH_OK";
        case BZ_STREAM_END: return "BZ_STREAM_END";
        case BZ_SEQUENCE_ERROR: return "BZ_SEQUENCE_ERROR";
        case BZ_PARAM_ERROR: return "BZ_PARAM_ERROR";
        case BZ_MEM_ERROR: return "BZ_MEM_ERROR";
        case BZ_DATA_ERROR: return "BZ_DATA_ERROR";
        case BZ_DATA_ERROR_MAGIC: return "BZ_DATA_ERROR_MAGIC";
        case BZ_IO_ERROR: return "BZ_IO_ERROR";
        case BZ_UNEXPECTED_EOF: return "BZ_UNEXPECTED_EOF";
        case BZ_OUTBUFF_FULL: return "BZ_OUTBUFF_FULL";
        case BZ_CONFIG_ERROR: return "BZ_CONFIG_ERROR";
    }
    return "";
}
