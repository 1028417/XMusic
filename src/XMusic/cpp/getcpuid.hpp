
#if !__android
#if defined(__GNUC__)    // GCC
#include <cpuid.h>
#elif defined(_MSC_VER)    // MSVC
    #if _MSC_VER >=1400    // VC2005
#include <intrin.h>
    #endif
#else
#error Only supports MSVC or GCC.
#endif

string getcpuidex(unsigned int InfoType, unsigned int ECXValue)
{
    unsigned int CPUInfo[4]{0,0,0,0};
#if defined(__GNUC__)    // GCC
    __cpuid_count(InfoType, ECXValue, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
    #if defined(_WIN64) || _MSC_VER>=1600    // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
        __cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
    #else
        _asm{
            // load. 读取参数到寄存器.
            mov edi, CPUInfo;    // 准备用edi寻址CPUInfo
            mov eax, InfoType;
            mov ecx, ECXValue;
            // CPUID
            cpuid;
            // save. 将寄存器保存到CPUInfo
            mov    [edi], eax;
            mov    [edi+4], ebx;
            mov    [edi+8], ecx;
            mov    [edi+12], edx;
        }
    #endif
#endif

    char lpBuf[32] = "";
    sprintf(lpBuf, "%.8X%.8X%.8X%.8X", CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
    return lpBuf;
}

string getcpuid(unsigned int InfoType = 1)
{
    unsigned int CPUInfo[4]{0,0,0,0};
#if defined(__GNUC__)    // GCC
    __cpuid(InfoType, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)    // MSVC
    #if _MSC_VER>=1400    // VC2005才支持__cpuid
        __cpuid((int*)(void*)CPUInfo, (int)InfoType);
    #else
        return getcpuidex(InfoType, 0);
    #endif
#endif

    char lpBuf[32] = "";
    sprintf(lpBuf, "%.8X%.8X%.8X%.8X", CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
    return lpBuf;
}
#endif


//---------------以下安卓返回空

#if !__windows
#include <sys/types.h>
#include <sys/stat.h>

#define USE_MMAP
#ifdef USE_MMAP
#include <sys/mman.h>
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
//#include "types.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;

#ifdef BIGENDIAN
typedef struct {
    u32 h;
    u32 l;
} u64;
#else
typedef struct {
    u32 l;
    u32 h;
} u64;
#endif

#ifdef ALIGNMENT_WORKAROUND
static inline u64 U64(u32 low, u32 high)
{
    u64 self;

    self.l = low;
    self.h = high;

    return self;
}
#endif

#ifdef ALIGNMENT_WORKAROUND
#    ifdef BIGENDIAN
#    define WORD(x) (u16)((x)[1] + ((x)[0] << 8))
#    define DWORD(x) (u32)((x)[3] + ((x)[2] << 8) + ((x)[1] << 16) + ((x)[0] << 24))
#    define QWORD(x) (U64(DWORD(x + 4), DWORD(x)))
#    else /* BIGENDIAN */
#    define WORD(x) (u16)((x)[0] + ((x)[1] << 8))
#    define DWORD(x) (u32)((x)[0] + ((x)[1] << 8) + ((x)[2] << 16) + ((x)[3] << 24))
#    define QWORD(x) (U64(DWORD(x), DWORD(x + 4)))
#    endif /* BIGENDIAN */
#else /* ALIGNMENT_WORKAROUND */
#define WORD(x) (u16)(*(const u16 *)(x))
#define DWORD(x) (u32)(*(const u32 *)(x))
#define QWORD(x) (*(const u64 *)(x))
#endif /* ALIGNMENT_WORKAROUND */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>

struct dmi_header
{
        u8 type;
        u8 length;
        u16 handle;
        u8 *data;
};

static int myread(int fd, char *buf, size_t count, const char *prefix)
{
    ssize_t r = 1;
    size_t r2 = 0;

    while (r2 != count && r != 0)
    {
        r = read(fd, buf + r2, count - r2);
        if (r == -1)
        {
            if (errno != EINTR)
            {
                close(fd);
                perror(prefix);
                return -1;
            }
        }
        else
            r2 += r;
    }

    if (r2 != count)
    {
        close(fd);
        fprintf(stderr, "%s: Unexpected end of file\n", prefix);
        return -1;
    }

    return 0;
}

int checksum(const u8 *buf, size_t len)
{
    u8 sum = 0;
    size_t a;

    for (a = 0; a < len; a++)
        sum += buf[a];
    return (sum == 0);
}

/*
 * Copy a physical memory chunk into a memory buffer.
 * This function allocates memory.
 */
void *mem_chunk(size_t base, size_t len, const char *devmem)
{
    void *p;
    int fd;
#ifdef USE_MMAP
    size_t mmoffset;
    void *mmp;
#endif

    if ((fd = open(devmem, O_RDONLY)) == -1)
    {
        perror(devmem);
        return NULL;
    }

    if ((p = malloc(len)) == NULL)
    {
        perror("malloc");
        return NULL;
    }

#ifdef USE_MMAP
#ifdef _SC_PAGESIZE
    mmoffset = base % sysconf(_SC_PAGESIZE);
#else
    mmoffset = base % getpagesize();
#endif /* _SC_PAGESIZE */
    /*
     * Please note that we don't use mmap() for performance reasons here,
     * but to workaround problems many people encountered when trying
     * to read from /dev/mem using regular read() calls.
     */
    mmp = mmap(0, mmoffset + len, PROT_READ, MAP_SHARED, fd, base - mmoffset);
    if (mmp == MAP_FAILED)
        goto try_read;

    memcpy(p, (u8 *)mmp + mmoffset, len);

    if (munmap(mmp, mmoffset + len) == -1)
    {
        fprintf(stderr, "%s: ", devmem);
        perror("munmap");
    }

    goto out;

#endif /* USE_MMAP */

try_read:
    if (lseek(fd, base, SEEK_SET) == -1)
    {
        fprintf(stderr, "%s: ", devmem);
        perror("lseek");
        free(p);
        return NULL;
    }

    if (myread(fd, (char*)p, len, devmem) == -1)
    {
        free(p);
        return NULL;
    }

out:
    if (close(fd) == -1)
        perror(devmem);

    return p;
}

void to_dmi_header(struct dmi_header *h, u8 *data)
{
    h->type = data[0];
    h->length = data[1];
    h->handle = WORD(data + 2);
    h->data = data;
}

int get_cpuid(char *cpuid)
{
    char devmem[10] = "/dev/mem";
    u16 len;
    u16 num;
    size_t fp;
    u8 *buf=NULL, *nbuf=NULL, *data, *p;
    int i = 0;
    if ((buf = (u8*)mem_chunk(0xF0000, 0x10000, devmem)) == NULL)
    {
        free(buf);
        return 0;
    }

    for (fp = 0; fp <= 0xFFF0; fp += 16)
    {
        if(memcmp(buf + fp, "_SM_", 4) == 0 && fp <= 0xFFE0)
        {
            len = WORD(buf + fp + 0x16);
            num = WORD(buf + fp + 0x1C);

            if (!checksum(buf + fp, (buf + fp)[0x05]) || memcmp(buf + fp + 0x10, "_DMI_", 5) != 0 || !checksum(buf + fp + 0x10, 0x0F))
            {
                free(buf);
                return 0;
            }
            if ((nbuf = (u8*)mem_chunk(DWORD(buf + fp + 0x18), len, devmem)) == NULL)
            {
                fprintf(stderr, "Table is unreachable, sorry.\n");
                free(buf);
                free(nbuf);
                return 0;
            }
            data = nbuf;
            while (i < num && data+4 <= nbuf + len)
            {
                u8 *next;
                struct dmi_header h;

                to_dmi_header(&h, data);

                if (h.length < 4)
                {
                    printf("Invalid entry length (%u). DMI table is "
                           "broken! Stop.\n\n", (unsigned int)h.length);
                    free(buf);
                    free(nbuf);
                    return 0;
                }

                next = data + h.length;
                while (next - nbuf + 1 < len && (next[0] != 0 || next[1] != 0))
                    next++;
                next += 2;
                if (h.type ==4)
                {
                    p = h.data + 0x08;
                    //printf("CPUID:%02X%02X%02X%02X%02X%02X%02X%02X\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
                    sprintf(cpuid, "%02X%02X%02X%02X%02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
                    free(buf);
                    free(nbuf);
                    return 1;
                }
                data = next;
                i++;
            }
            fp += 16;
        }
    }
    free(buf);
    free(nbuf);
    return 0;
}
#endif
