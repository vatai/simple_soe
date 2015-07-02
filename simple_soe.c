#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef unsigned long prime_t;
typedef unsigned long word_t;
#define LOG_WORD_SIZE 6

#define INDEX(i) ((i)>>(LOG_WORD_SIZE))
#define MASK(i) ((word_t)(1) << ((i)&(((word_t)(1)<<LOG_WORD_SIZE)-1)))
#define GET(p,i) (p[INDEX(i)]&MASK(i))
#define SET(p,i) (p[INDEX(i)]|=MASK(i))
#define RESET(p,i) (p[INDEX(i)]&=~MASK(i))
#define P2I(p) ((p)>>1) // (((p-2)>>1)) 
#define I2P(i) (((i)<<1)+1) // ((i)*2+3)

void print_table(size_t n, word_t *p){
  for(size_t i=n; i>0; --i)
    printf("%08lx ",p[i-1]);
  printf("\n");
}

void print_primes(size_t nbits, word_t *st){
  for(size_t i=0; i<nbits; i++) 
    if(!GET(st,i)) printf("%lu, ", I2P(i));
  printf("\n");
}

void print_primes_chunk(size_t nbits, word_t *st, size_t base){
  for(size_t i=0; i<nbits; i++) 
    if( ! GET( st, i ) ) printf( "%lu, ", I2P( i + base ) );
  printf("\n");
}


void soe_init(size_t nbits, word_t* st){
  prime_t p = 2;
  prime_t q = P2I(p);
  
  while(p*p<nbits) {
    while(GET(st,q)) q++;
    p = I2P(q);
    prime_t i = P2I(p*p);
    while(i<nbits){
      SET(st,i);
      i+=p;
    }
    q++;
  }
}

typedef struct chunk_param chunk_param;
struct chunk_param{
  size_t nbits;
  word_t* st;
  size_t chunk_bits;
  word_t** chunk_table;
  size_t chunk_index;
  size_t base;
};

/*** Old soe_chunk ***
void soe_chunk(size_t nbits,
	       word_t* st,
	       size_t chunk_bits, 
	       word_t** chunk_table, 
	       size_t chunk_index,
	       size_t base)
{
  word_t* chunk = *(chunk_table + chunk_index);
}
*/

inline size_t negmodp(size_t x, size_t p)
{
  size_t q = x % p;
  return q ? p-q : 0;
}

void soe_chunk(size_t nbits, word_t* st, 
	       size_t chunk_bits, word_t* chunk, 
	       size_t base)
{
  for(size_t i = 1; i < nbits; ++i){ // start from 1 if 1 is in primes
    if(GET(st,i)){
      size_t p = I2P(i);
      size_t q = negmodp(I2P(base),p);
      q = q % 2 ? P2I(q) : P2I(q + p);
      while(q < chunk_bits){
	SET(chunk, q);
	q += p;
      }
    }
  }
}

int main(){
  printf("Simple Sieve of Eratosthenese\n");
  
  size_t n = 4;
  size_t nbits = n * sizeof(word_t) * CHAR_BIT;
  word_t *st = (word_t*) calloc(sizeof(*st),n);
  printf("Last candidate: %lu\n\n", I2P(nbits-1));
  
  soe_init(nbits, st);
  print_table(n, st);
  print_primes(nbits, st);

  /* Initialization of chunks */
  size_t base = 2048;    
  size_t chunk_size = 4; 
  size_t chunk_bits = chunk_size * sizeof(word_t) * CHAR_BIT;
  word_t* chunk = calloc(chunk_size, sizeof(word_t)); // del this
  soe_chunk(nbits, st, chunk_bits, chunk, base);
  print_primes_chunk(chunk_bits, chunk, base);

  /*************************
  // Temporarily ignoresd //
  size_t num_chunks = 2;
  word_t** chunks = (word_t**) malloc(sizeof(*chunks)*num_chunks);
  word_t* chunk;
  for(size_t i=0; i<num_chunks; i++) 
  chunks[i] = (word_t*) calloc(sizeof(*chunks[i]),chunk_size);
  for(size_t i=0; i<num_chunks; i++) 
  soe_chunk(nbits, st, chunk_bits, chunks, i, base);
  /************************/

  // for(size_t i=0; i<num_chunks; i++) Free(chunks[i]);
  // free(chunks);
  free(st);

  printf("--- the end ---\n");
  return 0;
}
