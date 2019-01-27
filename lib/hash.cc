/*
=========================================================================================
=========================================================================================
*/
#include <string.h>

int 
GetHashValue(char* key,
	     int depth
	     )
{
  int sum = 0;
  int len = strlen(key);
  if (len % 2 == 1) len++; // make len even
  // for an odd length, use the trailing '\0' as part of key
  for (int j = 0; j < len; j+=2){
    // mod by the largest prime in the integer range to ensure the sum stays in range
    // 2147483647 is not only the largest prime, but also the largest signed int value 
    sum = (sum + 100 * key[j] + key[j+1]) % 2147483647;
  }
  return ( sum % (2^depth) );
}             

int 
Hash(char* key
     )
{
  int sum = 0;
  int len = strlen(key);
  if (len % 2 == 1) len++; // make len even
  // for an odd length, use the trailing '\0' as part of key
  for (int j = 0; j < len; j+=2){
    sum = (sum + 100 * key[j] + key[j+1]) % 2147483647;
  }
  return ( sum );  
}
