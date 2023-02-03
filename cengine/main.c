#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include<windows.h>

int min_value(int a, int b)
{
  return (a > b) ? b : a;
}

char* toLowerCase(char *str)
{
  size_t len = strlen(str);
  char *lower = calloc(len+1, sizeof(char));
  for (size_t i = 0; i < len; ++i) {
      lower[i] = tolower((unsigned char)str[i]);
  }
  return lower;
}

int editDistance(char *s1, char *s2)
{
  s1 = toLowerCase(s1);
  s2 = toLowerCase(s2);


  float costs[strlen(s2) + 1];

  for (size_t i = 0; i <= strlen(s1); i++) {

    int lastValue = i;

    for (size_t j = 0; j <= strlen(s2); j++) {

      if (i == 0) {
        costs[j] = j;
      } else {
        if (j > 0) {
          int newValue = costs[j - 1];
          if (s1[i - 1] != s2[j - 1]) {
            newValue = min_value(min_value(newValue, lastValue), costs[j]) + 1;
          }
          costs[j - 1] = lastValue;
          lastValue = newValue;
        }
      }
    }
    if (i > 0) {
      costs[strlen(s2)] = lastValue;
    }
  }
  return costs[strlen(s2)];
}

float similarity(char *s1, char *s2) {
  char *longer = s1;
  char *shorter = s2;

  if (strlen(s1) < strlen(s2)) {
    longer = s2;
    shorter = s1;
  }

  int longerLength = strlen(longer);  

  if (longerLength == 0)
  {
    return 1.0;
  }

  return (float)(longerLength - editDistance(longer, shorter)) / longerLength;
}

int main(){

  clock_t t;
  t = clock();
  float metric = similarity("Stackverflow", "ssss");
  t = clock() - t;
  double time_taken = ((double)t)/CLOCKS_PER_SEC;

  printf("Time: %.7f\n", time_taken);
  printf("%f\n", metric);;

  return 0;
}