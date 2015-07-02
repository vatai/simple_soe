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

void print_table(word_t *p, size_t n){
  for(size_t i=n; i>0; --i)
    printf("%08lx ",p[i-1]);
  printf("\n");
}

void print_primes(word_t *st, size_t nbits){
  for(size_t i=0; i<nbits; i++) 
    if(!GET(st,i)) printf("%lu, ", I2P(i));
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

void soe_chunk(size_t nbits,
	       word_t* st,
	       size_t chunk_bits, 
	       word_t** chunk_table, 
	       size_t chunk_index,
	       size_t base)
{
  word_t* chunk = *(chunk_table + chunk_index);
}

prime_t next_pime(word_t *st, int reset){
  static prime_t cur;
  if(reset) cur = 0;
  while(!GET(st,cur)) cur;
  return cur;
}

int main(){
  printf("Simple Sieve of Eratosthenese\n");
  
  size_t n = 4;
  size_t nbits = n * sizeof(word_t) * CHAR_BIT;
  word_t *st = (word_t*) calloc(sizeof(*st),n);

  soe_init(nbits, st);
  print_table(st,n);
  print_primes(st,nbits);

  /* Initialization of chunks */
  size_t base = 2048;
  size_t chunk_size = 4;
  size_t chunk_bits = chunk_size * sizeof(word_t) * CHAR_BIT;
  size_t num_chunks = 2;
  word_t** chunks = (word_t**) malloc(sizeof(*chunks)*num_chunks);
  word_t* chunk;
  for(size_t i=0; i<num_chunks; i++) 
    chunks[i] = (word_t*) calloc(sizeof(*chunks[i]),chunk_size);

  for(size_t i=0; i<num_chunks; i++) 
    soe_chunk(nbits, st, chunk_bits, chunks, i, base);

  for(size_t i=0; i<num_chunks; i++) free(chunks[i]);
  free(chunks);
  free(st);

  printf("--- the end ---\n");
  return 0;
}
