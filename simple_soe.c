/**
   This program is a simple implementation of the sieve of
   Eratosthenese (SOE).  It consists of two stages: 
   
   Stage 1: sieving out the "small" primes, i.e. from 0 to some
   smaller bound 'nbits'.
   
   Stage 2: using the primes/sieve table from stage 1, sieve chunks
   starting from a given base.
   
   Here of course, nbits*nbits > base should be true.
 */

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


/**
   Print/debug functions.
*/
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


/**
   Stage 1: soe_init() implements the sieving of "small" primes.  
   
   @param: |nbits| is the *effective* nbits, the number of candidates
   stored on |nbits| is |I2P(nbits)|.
   
   @param: |st| a pointer to the sieve table, with |nbits| number of
   bits allocated i.e. |nbits/8| bytes.
 */
void soe_init(size_t nbits, word_t* st){
  prime_t p = 3;
  prime_t q = P2I(p);
  
  while(p * p < I2P(nbits) ) {
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

/**
   negmodp2I() calculates the index of the candidate $-x \bmod p$,
   taking special care when $-x \bmod p$ is even.
*/
inline prime_t negmodp2I(prime_t x, prime_t p)
{
  prime_t q = x % p;
  q = q ? p-q : 0;
  return q % 2 ? P2I(q + p) : P2I( q );
}

/**
   TODO documentation.
 */
void soe_chunk(size_t nbits, word_t* st, 
	       size_t chunk_bits, word_t* chunk, 
	       prime_t base)
{
  for(size_t i = 1; i < nbits; ++i){ // start from 1 if 1 is in primes
    if( ! GET(st,i) ){
      prime_t p = I2P(i);
      prime_t q = I2P( base );
      assert(p<q);
      q = negmodp2I( q, p );
      while(q < chunk_bits){
	SET(chunk, q);
	q += p;
      }
    }
  }
}

int main(){
  printf("Simple Sieve of Eratosthenese\n");
  
  size_t log_upper_bound = 13; 
  size_t n = P2I(1<<(log_upper_bound-LOG_WORD_SIZE));
  size_t nbits = n * sizeof(word_t) * CHAR_BIT;
  word_t *st = (word_t*) calloc(sizeof(*st),n);
  printf("Last candidate: %lu\n\n", I2P(nbits-1));
  
  soe_init(nbits, st);
  // print_table(n, st);
  // print_primes(nbits, st);

  /* Initialization of chunks */
  prime_t base = nbits;
  size_t log_chunk_upper_bound = 10;
  size_t chunk_size = 1<<(log_chunk_upper_bound - LOG_WORD_SIZE); 
  size_t chunk_bits = chunk_size * sizeof(word_t) * CHAR_BIT;

  size_t num_chunks = 16;
  word_t** chunks = (word_t**) malloc(sizeof(word_t*)*num_chunks);
  word_t* chunk;
  for( size_t i = 0; i < num_chunks; ++i ) {
    chunks[i] = (word_t*) calloc( sizeof(word_t), chunk_size);
  }
  prime_t chunk_base = base;
  for( size_t i = 0; i < num_chunks; ++i ) {
    soe_chunk( nbits, st, chunk_bits, chunks[i], chunk_base);
    print_primes_chunk( chunk_bits, chunks[i], chunk_base);
    chunk_base += chunk_bits;
  }

  for(size_t i=0; i<num_chunks; i++) free(chunks[i]);
  free(chunks);
  free(st);

  printf("--- the end ---\n");

  return 0;
}
