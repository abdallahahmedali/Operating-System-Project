#include <math.h>

typedef struct {
    char *data;    // pointer to the bitmap data
    int size;      // size of the bitmap in bits
} Bitmap;

int roundToPowerOf2(int num) {
    return pow(2, ceil(log2(num)));
}

void initBitmap(Bitmap *bmp, int size) {
    bmp->data = malloc(size / 8 + 1);
    memset(bmp->data, 0, size / 8 + 1);
    bmp->size = size;
}

void setBit(Bitmap *bmp, int index) {
    int byte_index = index / 8;
    int bit_index = index % 8;
    bmp->data[byte_index] |= (1 << bit_index);
}

void clearBit(Bitmap *bmp, int index) {
    int byte_index = index / 8;
    int bit_index = index % 8;
    bmp->data[byte_index] &= ~(1 << bit_index);
}

int isBitSet(Bitmap *bmp, int index) {
    int byte_index = index / 8;
    int bit_index = index % 8;
    return bmp->data[byte_index] & (1 << bit_index);
}


void setBlock(Bitmap *bmp, int index, int block_size) {
    for (int i = index; i < index + block_size; i++)
        setBit(bmp, i);
}


void clearBlock(Bitmap *bmp, int index, int block_size) {
    for (int i = index; i < index + block_size; i++)
        clearBit(bmp, i);
}


void clearBuddyBlock(Bitmap *bmp, int index, int block_size) {
    for (int i = index; i < index + roundToPowerOf2(block_size); i++)
        clearBit(bmp, i);
}

int firstFitIndex(Bitmap* bmp,int block_size){
    int count = 0;
    for(int i = 0; i < bmp->size; i++){
        if(isBitSet(bmp,i))
            count = 0;
        else
            count++;
        if(count == block_size)
            return i - block_size + 1;
    }
    return -1;
}

// find best fit index
int bestFitIndex(Bitmap* bmp,int block_size){
    int powered_block_size= roundToPowerOf2(block_size);
    int count = 0;
    int best_size=2000;
    int best_idx=-1;
    int last_beg=-1;
    for(int i=0;i<bmp->size;i++)
    {
        if(isBitSet(bmp,i))
        {
            if(count >= powered_block_size && count < best_size)
            {
                best_size = count;
                best_idx = i-count;
            }
            i=i-count+powered_block_size-1,count = 0;
        }
        else
        {
            if(!count)
                last_beg = i;
            count++;
        }
        // if(count==128)
        //     printBitmap(bmp);
    }
    if(count >= powered_block_size && count < best_size)
        best_idx = last_beg;
    // printf("best_idx = %d\n",best_idx);
    return best_idx;
}

int allocateProcessMemory(Bitmap* bmp,int block_size){
    int idx = firstFitIndex(bmp,block_size);
    if(idx != -1)
        setBlock(bmp,idx,block_size);    
    return idx;
}

int allocateBuddyProcessMemory(Bitmap* bmp,int block_size){
    int idx = bestFitIndex(bmp,block_size);
    if(idx != -1)
        setBlock(bmp,idx,roundToPowerOf2(block_size));    
    return idx;
}

int totalAllocatedBits(Bitmap* bmp){
    int count = 0;
    for (int i = 0; i < bmp->size; i++)
        if(isBitSet(bmp,i)) count++;

    return count;
}

void freeBitmap(Bitmap *bmp) {
    if (bmp != NULL && bmp->data != NULL)
        free(bmp->data);
}

void printBitmap(Bitmap *bmp) {
    for (int i = 0; i < bmp->size; i++) {
        printf("%d", isBitSet(bmp, i));
        if(i % 128 == 0) printf("\n");
    }
    printf("\n");
}