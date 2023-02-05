#include <stdio.h>

#include <time.h>

#include <stdlib.h>

#include <stdbool.h>

#include "mpi.h"

int check(int * array, int m, int n, int * A, int * B, int passi) {
  int count = 0;
  for (int i = 0; i < m; i++) // righe
  {
    for (int s = 0; s < n; s++) {
      array[s] = A[i * n + s];
    }

    for (int j = 0; j < n; j++) // colonne
    {
      if (array[j] != 0) {
        int same = 0;
        int div = 0;
        int empty = 0;
        int c = 0;
        if (i != 0) //controllo la cella in alto
        {
          if (A[(i - 1) * n + j] == array[j]) {

            same++;
          } else if (A[(i - 1) * n + j] == 0) {
            empty++;
          } else {

            div++;
          }
        }
        if (i != m - 1) //controllo la cella in basso
        {
          if (A[(i + 1) * n + j] == array[j]) {

            same++;
          } else if (A[(i + 1) * n + j] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (j != n - 1) //controllo la cella a destra
        {
          if (array[j + 1] == array[j]) {

            same++;
          } else if (array[j + 1] == 0) {

            empty++;
          } else {
            div++;
          }
        }
        if (j != 0) //controllo la cella a sinistra
        {
          if (array[j - 1] == array[j]) {

            same++;
          } else if (array[j - 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != 0 && j != 0) //controllo la cella sopra a sinistra
        {
          if (A[(i - 1) * n + j - 1] == array[j]) {
            same++;
          } else if (A[(i - 1) * n + j - 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != 0 && j != n - 1) //controllo la cella sopra a destra
        {
          if (A[(i - 1) * n + j + 1] == array[j]) {
            same++;
          } else if (A[(i - 1) * n + j + 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != m - 1 && j != 0) //controllo la cella in basso a sinistra
        {
          if (A[(i + 1) * n + j - 1] == array[j]) {
            same++;
          } else if (A[(i + 1) * n + j - 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != m - 1 && j != n - 1) //controllo la cella in basso a destra
        {

          if (A[(i + 1) * n + j + 1] == array[j]) {

            same++;
          } else if (A[(i + 1) * n + j + 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (div + same == 0) {
          div++;
          same++;
          B[i * n + j] = array[j];
          count++;
        } else if ((double) same / (div + same) < perc) {

          int v = 0;

          while (true) {
            if (array[v] == 0) {
              B[i * n + v] = array[j];
              array[v] = array[j];
              B[i * n + j] = 0;
              array[j] = 0;
              break;
            }
            v++;
            if (v >= n) {
              B[i * n + j] = array[j];
              break;
            }
          }
        } else {
          count++;
          B[i * n + j] = array[j];
        }
      } else {
        count++;
        B[i * n + j] = array[j];
      }
    }
  }

  return count;
}

int main(int argc, char ** argv) {

  
  int m = atoi(argv[1]); // righe
  int n = atoi(argv[2]); // colonne
  int passiTot = atoi(argv[3]); // passi
  int perc = atoi(argv[4])/100; // percentuale
  int * A, * B, * array;
  int tot = 0; // variabile per controllare completamento
  int passi;

  FILE * fptr;
  fptr = fopen("corr_sequenz.txt", "w");
  // exiting program
  if (fptr == NULL) {
    printf("Error!");
    exit(1);
  }
  // allocazione matrici e vettori per scatter e gather

  A = (int * ) malloc(m * n * sizeof(int));
  B = (int * ) malloc(m * n * sizeof(int));
  array = (int * ) malloc(n * sizeof(int));

  // costruzione matrice
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      srand(i * n + j);
      A[i * n + j] = rand() % 3;
    }
  }

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      if (A[i * n + j] == 1) { // rosso
        fprintf(fptr, "\U0001f7e5 ");
      }

      if (A[i * n + j] == 2) { // blu
        fprintf(fptr, "\U0001f7e6 ");
      }

      if (A[i * n + j] == 0) {
        fprintf(fptr, "\U0001f532 ");
      }
    }
    fprintf(fptr, "\n");
  }
  fprintf(fptr, "\n");
  
  for (passi = 1; passi <= passiTot; passi++) {
    tot = check(array, m, n, A, B, passi);

    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (B[i * n + j] == 1) {
          fprintf(fptr, "\U0001f7e5 ");
        }

        if (B[i * n + j] == 2) {
          fprintf(fptr, "\U0001f7e6 ");
        }

        if (B[i * n + j] == 0) {
          fprintf(fptr, "\U0001f532 ");
        }
      }
      fprintf(fptr, "\n");
    }
    fprintf(fptr, "\n");

    if (tot == m * n) {
      break;
    }

    passi++;
    if (passi > passiTot) break;
    tot = check(array, m, n, B, A, passi);

    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (A[i * n + j] == 1) { // rosso
          fprintf(fptr, "\U0001f7e5 ");
        }

        if (A[i * n + j] == 2) { // blu
          fprintf(fptr, "\U0001f7e6 ");
        }

        if (A[i * n + j] == 0) {
          fprintf(fptr, "\U0001f532 ");
        }
      }
      fprintf(fptr, "\n");
    }
    fprintf(fptr, "\n");

    if (tot == m * n) {
      break;
    }

  }

  free(A);
  free(B);
  free(array);
  fclose(fptr);

}
