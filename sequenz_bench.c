#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <time.h>

#include "mpi.h"

int check(int * array, int m, int n, int * A, int * B, double perc) {
  int count = 0;
  for (int i = 0; i < m; i++) { // righe
    for (int s = 0; s < n; s++) {
      array[s] = A[i * n + s];
    }

    for (int j = 0; j < n; j++) { // colonne
      if (array[j] != 0) {
        int same = 0;
        int div = 0;
        int empty = 0;
        int c = 0;
        if (i != 0) { //controllo la cella in alto
          if (A[(i - 1) * n + j] == array[j]) {
            same++;
          } else if (A[(i - 1) * n + j] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != m - 1) { //controllo la cella in basso
          if (A[(i + 1) * n + j] == array[j]) {
            same++;
          } else if (A[(i + 1) * n + j] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (j != n - 1) { //controllo la cella a destra
          if (array[j + 1] == array[j]) {
            same++;
          } else if (array[j + 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (j != 0) { //controllo la cella a sinistra
          if (array[j - 1] == array[j]) {
            same++;
          } else if (array[j - 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != 0 && j != 0) { //controllo la cella sopra a sinistra
          if (A[(i - 1) * n + j - 1] == array[j]) {
            same++;
          } else if (A[(i - 1) * n + j - 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != 0 && j != n - 1) { //controllo la cella sopra a destra
          if (A[(i - 1) * n + j + 1] == array[j]) {
            same++;
          } else if (A[(i - 1) * n + j + 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != m - 1 && j != 0) { //controllo la cella in basso a sinistra
          if (A[(i + 1) * n + j - 1] == array[j]) {
            same++;
          } else if (A[(i + 1) * n + j - 1] == 0) {
            empty++;
          } else {
            div++;
          }
        }
        if (i != m - 1 && j != n - 1) { //controllo la cella in basso a destra
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
  int percent = atoi(argv[4]); //percentuale
  double perc = (double) percent / 100;
  int * A, * B, * array;
  int tot = 0; // variabile per controllare completamento
  int passi;

  // allocazione matrici e vettori per scatter e gather

  A = (int * ) malloc(m * n * sizeof(int));
  B = (int * ) malloc(m * n * sizeof(int));
  array = (int * ) malloc(n * sizeof(int));

  double start, end;

  // costruzione matrice
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      srand(i * n + j);
      A[i * n + j] = rand() % 5;
      if (A[i * n + j] == 1 || A[i * n + j] == 2) { // rosso
        A[i * n + j] = 1;
      }
      if (A[i * n + j] ==3 || A[i * n + j] == 4) { // blu
        A[i * n + j] = 2;
      }
    }
  }

  start = MPI_Wtime();
  for (passi = 1; passi <= passiTot; passi++) {
    tot = check(array, m, n, A, B, perc);
    passi++;
    if (passi > passiTot) break;
    tot = check(array, m, n, B, A, perc);

  }
  end = MPI_Wtime();
  printf("\nEsperimento completato in %.5fs.\n", end - start);
  free(A);
  free(B);
  free(array);
}
