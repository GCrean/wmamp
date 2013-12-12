/* Knuth shuffle ak.a. Fisher-Yates shuffle.
   This implementation is due to Ben Pfaff:
   http://benpfaff.org/writings/clc/shuffle.html.

   Adapted for Linux/WMA11b by GJ06.
*/

#include <stdio.h>
#include <stdlib.h>


/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator.
*/
void shuffle(int *array, size_t n)
{
  int i;
#if 0
  int *array_i = array;
  for (i = 0; i < n - 1; i++, array_i++) {
    /* Select element to swap from array[i..n-1] */
    size_t j = i + rand() / (RAND_MAX / (n - i));
    int t = array[j];
    array[j] = *array_i;
    *array_i = t;
  }
#else
  int *array_i = array + n;
  for (i = n-1; i >= 0; i--) {
    /* Select element to swap from array[0..i] */
    size_t j = rand() / (RAND_MAX / (i + 1));
    int t = array[j];
    array_i--;
    array[j] = *array_i;
    *array_i = t;
  }
#endif
}

int
main(int argc, char *argv[])
{
  int A[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
  int i, k, N;

  printf("RAND_MAX: %ld\n", RAND_MAX);

  for (k = 0; k < 100; k++) {
  shuffle(A, 16);
  for (i = 0; i < 16; i++)
    printf(" %d", A[i]);
  putchar('\n');
  }

#if 0
  N = 10;
  for (i = 0; i < 100; i++) {
    size_t k = rand() / (RAND_MAX / (N + 1));
    printf("%2d\n", k);
  }
#endif

  return 0;
}
