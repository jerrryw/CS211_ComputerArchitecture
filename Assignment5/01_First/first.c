#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "first.h"

int main(int argc, char* argv[argc + 1])
{
    if (argc != 6) {
        fprintf(stderr, "Usage:\n\t%s <test.txt>\n", argv[0]);
        //"\t%s <cache size> <associativity> <cache policy> <block size> <trace file>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    //$./first32 assoc:2 fifo 4 trace2.txt
    //<cache size> <associativity> <cache policy> <block size> <trace file>
    char   str[16];
   
    UINT32 cacheSize      = 0;
    UINT32 associative    = 0;
    UINT32 cachePolicy    = 0;
    UINT32 blockSize      = 0;
    const char* traceFile = argv[5];
    UINT32 ret            = 0;
    UINT32 cacheMapping   = 0;

    ret = sscanf(argv[1], "%ld", &cacheSize);
    assert(ret == 1);
    if (isPowerOfTwo(cacheSize) == FALSE) {
        printf("error");
    }

    // Cache Mapping Associativity
    if (strcmp("direct", argv[2]) == 0) {
        cacheMapping = CacheMappingDirect;
        associative = 1;
    } else if (strcmp("assoc", argv[2]) == 0) {
        cacheMapping = CacheMappingAssoc;
    } else {
        cacheMapping = CacheMappingAssoc;
        ret = sscanf(argv[2], "assoc:%lu", &associative);
        assert(ret == 1);
    }

    // Cache Policy
    ret = sscanf(argv[3], "%s", str);
    assert(ret == 1);
    if (strcmp("fifo", str) == 0) {
        cachePolicy = CachePolicyFIFO;
    } else if (strcmp("lru", str) == 0) {
        cachePolicy = CachePolicyLRU;
    } else {
        ERROR();
    }

    // Block Size
    ret = sscanf(argv[4], "%lu", &blockSize);
    assert(ret == 1);
    if (cacheMapping == CacheMappingAssoc && associative == 0) {
        assert(cacheSize % blockSize == 0);
        associative = cacheSize / blockSize;
        if (!isPowerOfTwo(associative)) {
            ERROR();
        }
    }

    CacheConfig config;
    config.m_CacheSize   = cacheSize;
    config.m_Associative = associative;
    config.m_Policy      = cachePolicy;
    config.m_BlockSize   = blockSize;
    config.m_TraceFile   = traceFile;

    simCache(&config);
    
    return EXIT_SUCCESS;
}

/*************************************
 *   Cache Simulation
 ************************************/
void simCache(const CacheConfig* pConfig)
{
    //Create cache
    CacheLine* pCache = createCache(pConfig);
    assert(pCache != NULL);

    // Start simulation
    FILE* fin = fopen(pConfig->m_TraceFile, "rt");
    assert(fin != NULL);

    UINT32 setCount  = 0;
    UINT32 blockID   = 0;
    UINT32 setID     = 0;
    UINT32 memTag    = 0;
    UINT32 ret       = 0;
    UINT32 cacheHits = 0;
    UINT32 cacheMiss = 0;
    UINT32 counterW  = 0;
    UINT32 counterR  = 0;
    char   str[16];
    UINT64 addr;
    UINT32 modeReadWrite = CacheRead;
    UINT32 timestamp = 0;

    while (TRUE) {
        ret = fscanf(fin, "%s %lx", str, &addr);
        if (ret != 2) break;
        assert(ret == 2);
        // printf("%s 0x%012lx\n", str, addr);

        //include counter
        if (str[0] == 'W') {
            modeReadWrite = CacheWrite;
        } else if (str[0] == 'R') {
            modeReadWrite = CacheRead;
        }


        blockID  = addr / pConfig->m_BlockSize;
        setCount = pConfig->m_CacheSize / pConfig->m_BlockSize / pConfig->m_Associative;
        setID    = blockID % setCount;
        memTag   = blockID / setCount;
        /////////////////////////////////////////////
        
        // find victim cache line
        CacheLine* victim_pCacheLine = &pCache[setID * pConfig->m_Associative + 0];
        UINT32 ts = victim_pCacheLine->timestamp;
        bool isCacheHit = FALSE;
        for (UINT32 i = 0; i < pConfig->m_Associative; ++i) {
            CacheLine* cur_pCacheLine = &pCache[setID * pConfig->m_Associative + i];
            if (cur_pCacheLine->valid) {
                if (cur_pCacheLine->tag == memTag) {
                    isCacheHit = TRUE;
                    victim_pCacheLine = cur_pCacheLine;
                    ts = cur_pCacheLine->timestamp;
                    break;
                }
            }
            // find the one with min ts
            if (!cur_pCacheLine->valid || cur_pCacheLine->timestamp < ts) {
                victim_pCacheLine = cur_pCacheLine;
                ts = cur_pCacheLine->timestamp;
            }
        }
        /////////////////////////////////////////////

        if (isCacheHit) {
            cacheHits++;
            if (pConfig->m_Policy == CachePolicyLRU) {
                victim_pCacheLine->timestamp = timestamp;
            }
        } else {
            cacheMiss++;
            counterR++;
            victim_pCacheLine->valid = 1;
            victim_pCacheLine->tag = memTag;
            victim_pCacheLine->timestamp = timestamp;
        }

        if (modeReadWrite == CacheWrite) {
            counterW++;
        }
        
        timestamp++;
    }
    
    printf("memread:%lu\n", counterR);
    printf("memwrite:%lu\n", counterW);
    printf("cachehit:%lu\n", cacheHits);
    printf("cachemiss:%lu\n", cacheMiss);


    destroyCache(pCache);
    pCache = NULL;
}

bool isPowerOfTwo(UINT32 num)
{
    if (num == 0) {
        return FALSE;
    }
    while (num != 1)
    {
        if (num%2 != 0)
            return FALSE;
        num /= 2;
    }
    return TRUE;
}

void ERROR()
{
    printf("error\n");
}

CacheLine* createCache(const CacheConfig* pConfig)
{
    const UINT32 N = pConfig->m_CacheSize / pConfig->m_BlockSize;
    CacheLine* pCache = calloc(N, sizeof(CacheLine));
    return pCache;
}

void destroyCache(CacheLine* pCache)
{
    free(pCache);
}

