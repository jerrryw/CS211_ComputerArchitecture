#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "second.h"

int main(int argc, char* argv[argc + 1])
{
    if (argc != 9) {
        fprintf(stderr, "Usage:\n\t%s <test.txt>\n", argv[0]);
        //"\t%s <cache size> <associativity> <cache policy> <block size> <trace file>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    //$./second 32 assoc:2 fifo 4 64 assoc lru trace2.txt
    //./second <L1 cache size><L1 associativity><L1 cache policy><block size>
    //         <L2 cache size><L2 associativity><L2 cache policy><trace le>
    char   str[16];

    UINT32 l1CacheSize      = 0;
    UINT32 l1Associative    = 0;
    UINT32 l1CachePolicy    = 0;
    UINT32 l1CacheMapping   = 0;

    UINT32 blockSize        = 0;
    const char* traceFile   = argv[8];
    UINT32 ret              = 0;

    UINT32 l2CacheSize      = 0;
    UINT32 l2Associative    = 0;
    UINT32 l2CachePolicy    = 0;
    UINT32 l2CacheMapping   = 0;

    ret = sscanf(argv[1], "%ld", &l1CacheSize);
    assert(ret == 1);
    if (isPowerOfTwo(l1CacheSize) == FALSE) {
        printf("error");
    }

    // Cache Mapping Associativity
    if (strcmp("direct", argv[2]) == 0) {
        l1CacheMapping = CacheMappingDirect;
        l1Associative = 1;
    } else if (strcmp("assoc", argv[2]) == 0) {
        l1CacheMapping = CacheMappingAssoc;
    } else {
        l1CacheMapping = CacheMappingAssoc;
        ret = sscanf(argv[2], "assoc:%lu", &l1Associative);
        assert(ret == 1);
    }

    // Cache Policy
    ret = sscanf(argv[3], "%s", str);
    assert(ret == 1);
    if (strcmp("fifo", str) == 0) {
        l1CachePolicy = CachePolicyFIFO;
    } else if (strcmp("lru", str) == 0) {
        l1CachePolicy = CachePolicyLRU;
    } else {
        ERROR();
    }

    // Block Size
    ret = sscanf(argv[4], "%lu", &blockSize);
    assert(ret == 1);
    if (l1CacheMapping == CacheMappingAssoc && l1Associative == 0) {
        assert(l1CacheSize % blockSize == 0);
        l1Associative = l1CacheSize / blockSize;
        if (!isPowerOfTwo(l1Associative)) {
            ERROR();
        }
    }

    ret = sscanf(argv[5], "%ld", &l2CacheSize);
    assert(ret == 1);
    if (isPowerOfTwo(l2CacheSize) == FALSE) {
        printf("error");
    }

    // Cache Mapping Associativity
    if (strcmp("direct", argv[6]) == 0) {
        l2CacheMapping = CacheMappingDirect;
        l2Associative = 1;
    } else if (strcmp("assoc", argv[6]) == 0) {
        l2CacheMapping = CacheMappingAssoc;
    } else {
        l2CacheMapping = CacheMappingAssoc;
        ret = sscanf(argv[6], "assoc:%lu", &l2Associative);
        assert(ret == 1);
    }

    // Cache Policy
    ret = sscanf(argv[7], "%s", str);
    assert(ret == 1);
    if (strcmp("fifo", str) == 0) {
        l2CachePolicy = CachePolicyFIFO;
    } else if (strcmp("lru", str) == 0) {
        l2CachePolicy = CachePolicyLRU;
    } else {
        ERROR();
    }

    // Block Size
    ret = sscanf(argv[4], "%lu", &blockSize);
    assert(ret == 1);
    if (l2CacheMapping == CacheMappingAssoc && l2Associative == 0) {
        assert(l2CacheSize % blockSize == 0);
        l2Associative = l2CacheSize / blockSize;
        if (!isPowerOfTwo(l2Associative)) {
            ERROR();
        }
    }

    CacheConfig l1Config;
    l1Config.m_CacheSize   = l1CacheSize;
    l1Config.m_Associative = l1Associative;
    l1Config.m_Policy      = l1CachePolicy;
    l1Config.m_BlockSize   = blockSize;
    l1Config.m_TraceFile   = traceFile;

    
    CacheConfig l2Config;
    l2Config.m_CacheSize   = l2CacheSize;
    l2Config.m_Associative = l2Associative;
    l2Config.m_Policy      = l2CachePolicy;
    l2Config.m_BlockSize   = blockSize;
    l2Config.m_TraceFile   = traceFile;

    //Create cache
    CacheLine* pL1Cache = createCache(&l1Config);
    assert(pL1Cache != NULL);
    CacheLine* pL2Cache = createCache(&l2Config);
    assert(pL2Cache != NULL);

    simCache(pL1Cache, pL2Cache, &l1Config, &l2Config);
    
    destroyCache(pL1Cache);
    pL1Cache = NULL;
    destroyCache(pL2Cache);
    pL2Cache = NULL;
    return EXIT_SUCCESS;
}

/*************************************
 *   Cache Simulation
 ************************************/
void simCache(CacheLine* pL1Cache, CacheLine* pL2Cache, CacheConfig* pL1Config, CacheConfig* pL2Config)
{

    // Start simulation
    FILE* fin = fopen(pL1Config->m_TraceFile, "rt");
    assert(fin != NULL);

    UINT32 setCount  = 0;
    UINT32 blockID   = 0;
    //UINT32 setID     = 0;
    UINT32 memTag    = 0;
    UINT32 ret       = 0;
    UINT32 l1CacheHits = 0;
    UINT32 l1CacheMiss = 0;
    UINT32 l2CacheHits = 0;
    UINT32 l2CacheMiss = 0;
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

        blockID  = addr / pL1Config->m_BlockSize;
        setCount = pL1Config->m_CacheSize / pL1Config->m_BlockSize / pL1Config->m_Associative;
        // setID    = blockID % setCount;
        memTag   = blockID / setCount;

        CacheLine* victim_pCacheLine = NULL;
        bool isL1CacheHit = FALSE;
        isL1CacheHit = findVictimLine(pL1Cache, pL1Config, addr, &victim_pCacheLine);

        if (isL1CacheHit) {         // L1 hit
            l1CacheHits++;
            if (pL1Config->m_Policy == CachePolicyLRU) {
                victim_pCacheLine->timestamp = timestamp;
            }
        } else {                    // L1 miss
            l1CacheMiss++;
            ///////////////////////////////////////////////////////////////
            CacheLine* victimL2_pCacheLine = NULL;
            bool isL2CacheHit = FALSE;
            isL2CacheHit = findVictimLine(pL2Cache, pL2Config, addr, &victimL2_pCacheLine);
            if (isL2CacheHit) {     // L2 hit
                l2CacheHits++;
                // remove from L2
                victimL2_pCacheLine->valid     = FALSE;
                victimL2_pCacheLine->addr      = 0;
                victimL2_pCacheLine->tag       = 0;
                victimL2_pCacheLine->timestamp = 0;
            } else {                // L2 miss
                l2CacheMiss++;
                counterR++;
            }

            if (victim_pCacheLine->valid == TRUE) {
                // @TODO move from L1 to L2
                bool isL2CacheHit2 = FALSE;
                isL2CacheHit2 = findVictimLine(pL2Cache, pL2Config, victim_pCacheLine->addr, &victimL2_pCacheLine);
                assert(isL2CacheHit2 == FALSE);

                UINT32 blockID2  = victim_pCacheLine->addr / pL2Config->m_BlockSize;
                UINT32 setCount2 = pL2Config->m_CacheSize / pL2Config->m_BlockSize / pL2Config->m_Associative;
                UINT32 memTag2   = blockID2 / setCount2;
                
                victimL2_pCacheLine->valid     = TRUE;
                victimL2_pCacheLine->addr      = victim_pCacheLine->addr;
                victimL2_pCacheLine->tag       = memTag2;
                victimL2_pCacheLine->timestamp = timestamp;
            }
            ///////////////////////////////////////////////////////////////
            // memory tag -> L1
            victim_pCacheLine->valid     = TRUE;
            victim_pCacheLine->addr      = addr;
            victim_pCacheLine->tag       = memTag;
            victim_pCacheLine->timestamp = timestamp;
        }

        if (modeReadWrite == CacheWrite) {
            counterW++;
        }
        
        timestamp++;
    }
    
    printf("memread:%lu\n", counterR);
    printf("memwrite:%lu\n", counterW);
    printf("l1cachehit:%lu\n", l1CacheHits);
    printf("l1cachemiss:%lu\n", l1CacheMiss);
    printf("l2cachehit:%lu\n", l2CacheHits);
    printf("l2cachemiss:%lu\n", l2CacheMiss);
}

bool findVictimLine(CacheLine* pCache, CacheConfig* pConfig, UINT64 addr, CacheLine** victim_pCacheLine)
{    
    UINT32 setCount  = 0;
    UINT32 blockID   = 0;
    UINT32 setID     = 0;
    UINT32 memTag    = 0;

    blockID  = addr / pConfig->m_BlockSize;
    setCount = pConfig->m_CacheSize / pConfig->m_BlockSize / pConfig->m_Associative;
    setID    = blockID % setCount;
    memTag   = blockID / setCount;

    (*victim_pCacheLine) = &pCache[setID * pConfig->m_Associative + 0];
    UINT32 ts = (*victim_pCacheLine)->timestamp;
    bool isCacheHit = FALSE;
    for (UINT32 i = 0; i < pConfig->m_Associative; ++i) {
        CacheLine* cur_pCacheLine = &pCache[setID * pConfig->m_Associative + i];
        if (cur_pCacheLine->valid) {
            if (cur_pCacheLine->tag == memTag) {
                isCacheHit = TRUE;
                (*victim_pCacheLine) = cur_pCacheLine;
                ts = cur_pCacheLine->timestamp;
                break;
            }
        }
        // find the one with min ts
        if (!cur_pCacheLine->valid || cur_pCacheLine->timestamp < ts) {
            (*victim_pCacheLine) = cur_pCacheLine;
            ts = cur_pCacheLine->timestamp;
        }
    }
    return isCacheHit;
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

