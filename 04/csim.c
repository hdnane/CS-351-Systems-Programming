#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h> 
#include <unistd.h>
#include <stdbool.h>
#include <tgmath.h>
#include <string.h>
#include <stdint.h>

typedef struct cache cache_t;
typedef struct trace_line  trace_line_t;
typedef struct result result_t;

struct cache 
{ 
    int assoc;
    int block_offset, block_size;
    int set_idx, set_size;
    int ** tags; 
};

struct trace_line
{
    char operation;
    unsigned int address;
    int size;
};

struct result
{
    int hit;
    int miss;
    int eviction;
};

#define HELP_TAG "-h"
#define VERBOSE_TAG "-v" 
#define SET_TAG "-s"
#define BLOCK_TAG "-b"
#define ASSOC_TAG "-E"
#define TRACE_TAG "-t"

#define BUF 80

void usage_info();
result_t caching(cache_t cache, char * fileName, bool verbose);
result_t simulation(trace_line_t * lines, int num_lines, cache_t cache, bool verbose);

int main(int argc, char * argv[])
{
    bool verbose = false;
    bool set = false;
    bool assoc = false;
    bool block = false;
    bool trace = false;
    
    char * file_name = malloc(BUF * sizeof(char));
    cache_t cache;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], HELP_TAG))
            usage_info();

        else if (!strcmp(argv[i], VERBOSE_TAG)){  verbose = true; }
  
        else if (!strcmp(argv[i], SET_TAG)) 
        {
          char * value = argv[i+1];
          cache.set_idx = atoi(value);
          cache.set_size = pow(2, atoi(value));
          set = true; 
        }
  
        else if (!strcmp(argv[i], ASSOC_TAG))
        {
          char * value = argv[i+1];
          cache.assoc = atoi(value);
          assoc = true;
        }

        else if (!strcmp(argv[i], BLOCK_TAG))
        {
          char * value = argv[i+1];
          cache.block_offset = atoi(value);
          cache.block_size = pow(2, atoi(value));
          block = true;
        }

        else if (!strcmp(argv[i], TRACE_TAG)) 
        {
          char * value = argv[i+1];
          strcpy(file_name, value);
          trace = true;
        }
    }

    if (!set || !trace || !assoc || !block) 
    {
        printf(" csim: Missing required command line argument \n"
        " Usage: ./csim -s <number> -E <number> -b <number> -t <filename>.trace\n");
        return 1;
    }

    result_t result = caching(cache, file_name, verbose);
    printSummary(result.hit, result.miss, result.eviction);

    return 0;
}

uint64_t bit_man(uint64_t src, int32_t low, int32_t high)
{
    if (low > 63 || high > 63)
        return 0;

    if (low > high)
        return 0;

    src = src << (63 - high);
    src = src >> (63 - high);
    src = src >> low;
    return src;
}

result_t caching(cache_t cache, char * file_name, bool verbose) 
{

    int  file_lines = 0; 
    char line_text[BUF];

    FILE * trace_file ; 
    trace_file = fopen(file_name, "r");

    
    while (fgets(line_text, BUF, trace_file)) 
    {
        file_lines++;
    }

    fclose(trace_file);

    int line_num = 0;
    trace_file = fopen(file_name, "r");
    trace_line_t lines[file_lines];

    while (fgets(line_text, BUF, trace_file))
     {
         sscanf(line_text, " %c %x,%d", &lines[line_num].operation,
                 &lines[line_num].address, &lines[line_num].size);
         line_num++;
     }

    return simulation(lines, file_lines, cache, verbose);
}

result_t simulation(trace_line_t * lines, int file_lines, cache_t cache, bool verbose)  
{

    cache.tags = malloc(sizeof (int*) * cache.set_size); 

    result_t result; 
    result.hit = 0; 
    result.miss = 0; 
    result.eviction = 0;

    for(int i = 0; i < cache.set_size; i++) 
    {
        cache.tags[i] = malloc(sizeof(int) * cache.assoc); 

        for(int j = 0; j < cache.assoc; j++) 
        {
            cache.tags[i][j] = -1;
        }

    }

    for(int n = 0; n < file_lines; n++) 
    {

        if(verbose)
        printf("%c %x,%d ",lines[n].operation,lines[n].address,lines[n].size);
        
        if(lines[n].operation == 'I')
            continue; 

        uint64_t set_idx = bit_man(lines[n].address, cache.block_offset, cache.block_offset + cache.set_idx - 1);
        uint64_t tag_bits = bit_man(lines[n].address, cache.block_offset + cache.set_idx, 63);

        int * ptr = cache.tags[set_idx]; 
        bool found = false;

        for(int i = 0; i < cache.assoc; i++) 
        {
            if(ptr[i] == tag_bits)
            {
                if(lines[n].operation == 'M') 
                    result.hit += 2;

                else   
                    result.hit++;
                    
                if(verbose)
                printf("hit \n");

                found = true;

                for(int j = i; j > 0; j--)
                  ptr[j] = ptr[j-1];
        
                ptr[0] = tag_bits;
            }
        }

        if(!found) 
        {
            if(lines[n].operation == 'M') 
            {
                result.miss++;
                result.hit++;

                if(verbose)
                printf("miss hit \n");

            } 

            else 
            {
                result.miss++;
                if(verbose)
                printf("miss\n");
            }
                
            if(ptr[cache.assoc - 1] > -1)
            {
                result.eviction++;
                if(verbose)
                printf("eviction\n");
            } 
                
        
            for(int i = cache.assoc - 1; i > 0; i--) 
                ptr[i] = ptr[i-1];

            ptr[0] = tag_bits;
        }
    }
    return result;
}

void  usage_info()
{    
    printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n"
    "-h: Optional help flag that prints usage info \n"
    "-v: Optional verbose flag that displays trace info \n"
    "-s <s>: Number of set index bits (S = 2s is the number of sets) \n"
    "-E <E>: Associativity (number of lines per set) \n"
    "-b <b>: Number of block bits (B = 2b is the block size) \n"
    "-t <tracefile>: Name of the valgrind trace to replay \n"
    "\n"
    "Examples: \n"
    " linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace \n"
    " linux>  ./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.trace \n");
}