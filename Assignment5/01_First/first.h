// first.h
#ifndef bool
    #define bool unsigned char
    #define TRUE (1)
    #define FALSE (0)
#endif

typedef unsigned long int UINT64;
typedef unsigned long int UINT32;

// const UINT32 CacheMappingDirect = 0;
// const UINT32 CacheMappingAssoc  = 1;
enum CacheMappingTypes { CacheMappingDirect, CacheMappingAssoc };
enum CacheReplacementPolicy { CachePolicyFIFO, CachePolicyLRU };
enum CacheEvent { CacheRead, CacheWrite };

typedef struct s_CacheConfig {
    UINT32 m_CacheSize;
    UINT32 m_Associative;
    UINT32 m_Policy;
    UINT32 m_BlockSize;
    const char* m_TraceFile;
} CacheConfig;

typedef struct s_CacheLine {
    // UINT32* data;
    UINT32     tag;
    UINT32	   valid;
    UINT32     timestamp;
} CacheLine;

bool isPowerOfTwo();
void ERROR();
void simCache(const CacheConfig* pConfig);
CacheLine* createCache(const CacheConfig* pConfig);
void destroyCache(CacheLine* pCache);

