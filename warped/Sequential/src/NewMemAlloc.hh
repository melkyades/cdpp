#ifdef NEW_MEM_ALLOC

# include <cstdio>
# include <stdlib.h>

# define MAX_ENTRIES        32   // How many distinct size that need to be
                                 // maintained in the system.

# define MULTIPLES_FACTOR   32   // Each entry is a multiple of this factor
                                 // .ie. each  entry has blocks of size
                                 // MULTIPLIES_FACTOR * n where 'n' the
                                 // number of the std::list entry.
                                 // Note : But for the last entry which has
                                 // ---- : all the larger blocks

# define MULTIPLES_POW_2     5   // Which power of 2 is the factor specified.
                                 // This is helpful in avoiding division

                                 // Note : The last entry in the std::list has
                                 // ----   all nodes > the previous entry.
                                 //        .ie. all big blocks go in there.

# define INITIALLY_ALLOCATE 8192 // The following constants define the size
# define NEED_MORE_ALLOCATE 8192 // of the blocks that need to be allocated
                                 // intially and then the size of the blocks
                                 // that are to be allocated when more memory
                                 // memory is needed. This is done in terms of
                                 // the MULTIPLES_FACTOR. The value must be  
                                 // >= MAX_ENTRIES.
                                 // Note : The actual size of the blocks
                                 // ----   allocated is (X) * MULTIPLES_FACTOR

typedef struct BlockInfo BlockHeader;

typedef
struct BlockInfo {               // Structure for info. on memory block.
  unsigned sizeAndFlag;          // This variable is used to maintain the
                                 // size and usage information on a block
  BlockHeader *next;             // Points to the next block in the std::list
  BlockHeader *prev;             // Points to the prev block in the free std::list
  
                                 // The size of this block is needed while
                                 // garbage collecting.
} BlockHeader;

                                 // Note : Size is maintained as multiples of
                                 // ----   32. The first bit of sizeAndFlag
                                 //        variable is used to indicate if the
                                 //        block is free or not .ie. if set
                                 //        the block is in-use else it is free


#define INUSE 0x80000000         // These constants are used to set the inuse
#define FREE  0x7FFFFFFF         // flag on and off. To set inuse on, bit-wise
                                 // OR sizeAndFlag variable. To reset it bit-
                                 // wise AND it with sizeAndFlag variable.
                                 // Similary operations are used to extract /
                                 // set size of the block etc. Some Macros to
                                 // the standard stuff are std::listed below.

#define getSize(x)     ((x & FREE))
#define markInUse(x)   ((x |= INUSE))
#define markFree(x)    ((x &= FREE))
#define setSize(x, y)  (x = y)
#define inUse(x)       ((x & INUSE))

typedef			
struct GlobalBlockList_Info {         // Structure used to maintain the std::list of
  struct GlobalBlockList_Info *link;  // chunks of memory allocated by the 
  char                        *memory;// system.
} GlobalBlockList;                  

class NewMemAlloc {
public:
  NewMemAlloc();
  ~NewMemAlloc();

  void *allocateBlock(size_t);
  void deleteBlock(void *);
  void printList(void);

protected:

#ifdef NEW_MEM_STATS
  long totalNews,   totalDeletes;
  long totalHits, newBlock;
  long coaleseCount;
  long totalLastEntrySearches;
#endif

  void initialise(void);           // Initalize the memory management system.
                                   // Some functions that are used internally

  void splitBlock(BlockHeader *, int, int);
  void coaleseMemory(char *, int);
  void deleteAllMemoryBlocks(void);
  void insertBlock(BlockHeader *);
  
  void removeFromList(BlockHeader *);
  void *checkLastEntry(int, char);
  
  BlockHeader *allocateChunkOfMemory(int);
  
private:
  BlockHeader     *freeBlocksList[MAX_ENTRIES];
  GlobalBlockList *listOfBlocks;
};

extern NewMemAlloc GlobalMemoryManager;

void * operator new(size_t);          // Allocate block of memory of given size
void operator delete(void *, size_t); // Free a block of memory.

#endif

// End of source code
