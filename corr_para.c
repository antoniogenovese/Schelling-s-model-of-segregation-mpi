#include <stdio.h>

#include <mpi.h>

#include <stdlib.h>

#include <stdbool.h>

void scatter(int * displ_rcv, int * num_rcv, int * local_rcv_num, int * local_snd_num, int * num_snd, int * displ_snd, int * from_snd_local, int * to_snd_local, int me, int m, int n, int nproc);

int check_async(int i, int * array, int n, int me, int * localA, int * localB,double perc) {

  int count = 0;

  for (int s = 0; s < n; s++) {
    array[s] = localA[i * n + s];
  }

  for (int j = 0; j < n; j++) // colonne
  {

    if (array[j] != 0) {
      int same = 0;
      int div = 0;
      int empty = 0;
      int c = 0;

      if (localA[(i - 1) * n + j] == array[j]) {
        same++;
      } else if (localA[(i - 1) * n + j] == 0) {
        empty++;
      } else {
        div++;
      }

      if (localA[(i + 1) * n + j] == array[j]) {
        same++;
      } else if (localA[(i + 1) * n + j] == 0) {
        empty++;
      } else {
        div++;
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
      if (j != 0) //controllo la cella in alto a sinistra
      {
        if (localA[(i - 1) * n + j - 1] == array[j]) {
          same++;
        } else if (localA[(i - 1) * n + j - 1] == 0) {
          empty++;
        } else {
          div++;
        }
      }
      if (j != n - 1) //controllo la cella in alto a destra
      {
        if (localA[(i - 1) * n + j + 1] == array[j]) {
          same++;
        } else if (localA[(i - 1) * n + j + 1] == 0) {
          empty++;
        } else {
          div++;
        }
      }
      if (j != 0) //controllo la cella in basso a sinistra
      {
        if (localA[(i + 1) * n + j - 1] == array[j]) {
          same++;
        } else if (localA[(i + 1) * n + j - 1] == 0) {
          empty++;
        } else {
          div++;
        }
      }
      if (j != n - 1) //controllo la cella in basso a destra
      {
        if (localA[(i + 1) * n + j + 1] == array[j]) {
          same++;
        } else if (localA[(i + 1) * n + j + 1] == 0) {
          empty++;
        } else {
          div++;
        }
      }
      if (div + same == 0) {
        div++;
        same++;
        localB[i * n + j] = array[j];
        count++;
      } else if ((double) same / (div + same) < perc) {
        int v = 0;
        while (true) {
          if (array[v] == 0) {
            localB[i * n + v] = array[j];
            array[v] = array[j];
            localB[i * n + j] = 0;
            array[j] = 0;
            break;
          }
          v++;
          if (v >= n) {
            localB[i * n + j] = array[j];
            break;
          }
        }
      } else {
        localB[i * n + j] = array[j];

        count++;
      }
    } else {
      localB[i * n + j] = array[j];
      count++;
    }

  }

}

int check(int to_snd_local, int to, int from, int * array, int local_rcv_num, int n, int me, int * localA, int * localB, int nproc, double perc) {
  int count = 0;
  for (int i = 0; i < local_rcv_num; i++) // righe
  {
    for (int s = 0; s < n; s++) {
      array[s] = localA[i * n + s];
    }
    for (int j = 0; j < n; j++) // colonne
    {
      if (i < from || i > to) {

        localB[i * n + j] = localA[i * n + j];
      } else {

        if (array[j] != 0) {
          int same = 0;
          int div = 0;
          int empty = 0;
          int c = 0;
          if (i != 0) //controllo la cella in alto
          {
            if (localA[(i - 1) * n + j] == array[j]) {
              same++;
            } else if (localA[(i - 1) * n + j] == 0) {
              empty++;
            } else {
              div++;
            }
          }
          if (!(me == nproc - 1 && i == to_snd_local)) //controllo la cella in basso
          {
            if (localA[(i + 1) * n + j] == array[j]) {
              same++;
            } else if (localA[(i + 1) * n + j] == 0) {
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
          if (i != 0 && j != 0) //controllo la cella in alto a sinistra
          {
            if (localA[(i - 1) * n + j - 1] == array[j]) {
              same++;
            } else if (localA[(i - 1) * n + j - 1] == 0) {
              empty++;
            } else {
              div++;
            }
          }
          if (i != 0 && j != n - 1) //controllo la cella in alto a destra
          {
            if (localA[(i - 1) * n + j + 1] == array[j]) {
              same++;
            } else if (localA[(i - 1) * n + j + 1] == 0) {
              empty++;
            } else {
              div++;
            }
          }
          if (!(me == nproc - 1 && i == to_snd_local) && j != 0) //controllo la cella in basso a sinistra
          {
            if (localA[(i + 1) * n + j - 1] == array[j]) {
              same++;
            } else if (localA[(i + 1) * n + j - 1] == 0) {
              empty++;
            } else {
              div++;
            }
          }
          if (!(me == nproc - 1 && i == to_snd_local) && j != n - 1) //controllo la cella in basso a destra
          {
            if (localA[(i + 1) * n + j + 1] == array[j]) {
              same++;
            } else if (localA[(i + 1) * n + j + 1] == 0) {
              empty++;
            } else {
              div++;
            }
          }
          if (div + same == 0) {
            div++;
            same++;
            localB[i * n + j] = array[j];
            count++;
          } else if ((double) same / (div + same) < perc) {
            int v = 0;
            while (true) {
              if (array[v] == 0) {
                localB[i * n + v] = array[j];
                array[v] = array[j];
                localB[i * n + j] = 0;
                array[j] = 0;
                break;
              }
              v++;
              if (v >= n) {
                localB[i * n + j] = array[j];
                break;
              }
            }
          } else {
            localB[i * n + j] = array[j];

            count++;
          }
        } else {
          localB[i * n + j] = array[j];
          count++;
        }
      }
    }
  }

  return count;
}
void scatter(int * displ_rcv, int * num_rcv, int * local_rcv_num, int * local_snd_num, int * num_snd, int * displ_snd, int * from_snd_local, int * to_snd_local, int me, int m, int n, int nproc) {
  // scatter gather per righe
  // inizializzazione vettori di ricezione e invio
  if (me == 0) {
    num_rcv[0] = (m / nproc * n) + n;
    if (0 < m % nproc) //se presente resto riceve una riga in piu
    {
      num_rcv[0] = num_rcv[0] + n;

    }
    for (int i = 1; i < nproc - 1; i++) {
      num_rcv[i] = ((m / nproc) + 2) * n; //ogni processore riceve m/nproc righe + altre 2 che condivide con il p superiore
      if (i < m % nproc) {
        num_rcv[i] = num_rcv[i] + n; //se presente resto riceve una riga in piu
      }
    }
    num_rcv[nproc - 1] = (m / nproc * n) + n; //l'ultimo p riceve una riga in piu a m/nproc
    displ_rcv[0] = 0; //variabile che definisce i displacement nella scatter(salti)

    for (int i = 1; i < nproc; i++) {
      displ_rcv[i] = displ_rcv[i - 1] + num_rcv[i - 1] - 2 * n;
    }

    * local_rcv_num = m / nproc + 1; //righe ricevute da p=0 nella scatter
    if (0 < m % nproc) {
      * local_rcv_num = * local_rcv_num + 1; //se presente resto riceve una riga in piu
    }

    * local_snd_num = * local_rcv_num - 1; //righe inviate da p=0 nella gather

    for (int i = 0; i < nproc; i++) {
      num_snd[i] = m / nproc * n; //righe inviate da i esimo p nella gather 
      if (i < m % nproc) {
        num_snd[i] = num_snd[i] + n; //se presente resto invia una riga in piu
      }
    }

    displ_snd[0] = 0; //variabile che definisce i displacement nella gather (salti)
    for (int i = 1; i < nproc; i++) {
      displ_snd[i] = displ_snd[i - 1] + num_snd[i - 1];
    }

    * from_snd_local = 0; //da che riga dove far partitre la gather
    * to_snd_local = * local_rcv_num - 2; //dove farla finire
  } else {
    if (me == nproc - 1) {
      * local_rcv_num = m / nproc + 1; //righe ricevute nella scatter
      * local_snd_num = * local_rcv_num - 1; // righr inviate nella gather
      * from_snd_local = 1; //da che riga far partire la gather
      * to_snd_local = * local_rcv_num - 1; //dove finirla
    } else {
      * local_rcv_num = m / nproc + 2;
      if (me < m % nproc) {
        * local_rcv_num = * local_rcv_num + 1;
      }
      * local_snd_num = * local_rcv_num - 2;
      * from_snd_local = 1;
      * to_snd_local = * local_rcv_num - 2;
    }
  }

}

void main(int argc, char ** argv) {
  int me, nproc;

  MPI_Init( & argc, & argv);
  MPI_Status status;
  MPI_Comm_size(MPI_COMM_WORLD, & nproc);
  MPI_Comm_rank(MPI_COMM_WORLD, & me);
  MPI_Request request[2];

int m = atoi(argv[1]); // righe
  int n = atoi(argv[2]); // colonne
  int passiTot = atoi(argv[3]); // passi
  int percent= atoi(argv[4]); //percentuale
  double perc= (double)percent/100;
  int local_rcv_num;
  int local_snd_num; // righe inviate nella gather
  int from_snd_local;
  int to_snd_local;

  int * A;
  int * localA; // sottomatrice in lettura
  int * localB; // sottomatrice in scrittura
  int * array;

  int * displ_rcv; // variabile per scatter
  int * num_rcv; // variabile per scatter
  int * displ_snd; // variabile per gather
  int * num_snd; // variabile per gather

  int finito = 0;
  int tot = 0;
  int passi;

  FILE * fptr;
  fptr = fopen("corr_para.txt", "w");
  // exiting program
  if (fptr == NULL) {
    printf("Error!");
    exit(1);
  }
  if (me == 0) {
    // allocazione matrici e vettori per scatter e gather
    A = (int * ) malloc(n * m * sizeof(int));
    displ_rcv = (int * ) malloc(nproc * sizeof(int));
    num_rcv = (int * ) malloc(nproc * sizeof(int));
    displ_snd = (int * ) malloc(nproc * sizeof(int));
    num_snd = (int * ) malloc(nproc * sizeof(int));

    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        srand(i * n + j);
        A[i * n + j] = rand() % 5;
      }
    }
    // stampa matrice su file
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (A[i * n + j] == 1 || A[i * n + j] == 2) { // rosso
          fprintf(fptr, "\U0001f7e5 ");
        }

        if (A[i * n + j] == 3 || A[i * n + j] == 4) { // blu
          fprintf(fptr, "\U0001f7e6 ");
        }

        if (A[i * n + j] == 0) {
          fprintf(fptr, "\U0001f532 ");
        }
      }
      fprintf(fptr, "\n");
    }
    fprintf(fptr, "\n");
  }
  scatter(displ_rcv, num_rcv, & local_rcv_num, & local_snd_num, num_snd, displ_snd, & from_snd_local, & to_snd_local, me, m, n, nproc);
  localA = (int * ) malloc(n * local_rcv_num * sizeof(int));
  localB = (int * ) malloc(n * local_rcv_num * sizeof(int));
  array = (int * ) malloc(n * sizeof(int));

  int to, from;
  if (me == 0) {
    from = 0;
    to = local_rcv_num - 3;
  } else if (me == nproc - 1) {
    from = 2;
    to = local_rcv_num - 1;
  } else {
    from = 2;
    to = local_rcv_num - 3;
  }

  MPI_Scatterv( & A[0], & num_rcv[0], & displ_rcv[0], MPI_INT, & localA[0], local_rcv_num * n, MPI_INT, 0, MPI_COMM_WORLD);

  finito = check(to_snd_local, to_snd_local, from_snd_local, array, local_rcv_num, n, me, localA, localB, nproc,perc);
  MPI_Gatherv( & localB[from_snd_local * n], local_snd_num * n, MPI_INT, & A[0], & num_snd[0], & displ_snd[0], MPI_INT, 0, MPI_COMM_WORLD);

  if (me == 0) {
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (A[i * n + j] == 1 || A[i * n + j] == 2) { // rosso
          fprintf(fptr, "\U0001f7e5 ");
        }

        if (A[i * n + j] == 3 || A[i * n + j] == 4) { // blu
          fprintf(fptr, "\U0001f7e6 ");
        }

        if (A[i * n + j] == 0) {
          fprintf(fptr, "\U0001f532 ");
        }
      }
      fprintf(fptr, "\n");
    }
    fprintf(fptr, "\n");

  }

  for (passi = 1; passi < passiTot; passi++) {
    if (me == 0) {
      if (nproc > 1) {

        MPI_Isend( & localB[(local_rcv_num - 2) * n], n, MPI_INT,
          me + 1, 0, MPI_COMM_WORLD, & request[0]);
        MPI_Irecv( & localB[(local_rcv_num - 1) * n], n, MPI_INT,
          me + 1, 1, MPI_COMM_WORLD, & request[1]);

      }
    } else if (me == nproc - 1) {
      MPI_Isend( & localB[n], n, MPI_INT,
        me - 1, 1, MPI_COMM_WORLD, & request[1]);
      MPI_Irecv( & localB[0], n, MPI_INT,
        me - 1, 0, MPI_COMM_WORLD, & request[0]);

    } else {

      MPI_Isend( & localB[n], n, MPI_INT,
        me - 1, 1, MPI_COMM_WORLD, & request[1]);
      MPI_Isend( & localB[(local_rcv_num - 2) * n], n, MPI_INT,
        me + 1, 0, MPI_COMM_WORLD, & request[0]);
      MPI_Irecv( & localB[0], n, MPI_INT,
        me - 1, 0, MPI_COMM_WORLD, & request[0]);
      MPI_Irecv( & localB[(local_rcv_num - 1) * n], n, MPI_INT,
        me + 1, 1, MPI_COMM_WORLD, & request[1]);
    }

    finito = finito + check(to_snd_local, to, from, array, local_rcv_num, n, me, localB, localA, nproc,perc);
    MPI_Waitall(2, request, MPI_STATUSES_IGNORE);
    if (me == 0) {
      finito = finito + check_async(local_rcv_num - 2, array, n, me, localB, localA,perc);
    } else if (me == nproc - 1) {
      finito = finito + check_async(1, array, n, me, localB, localA,perc);
    } else {
      finito = finito + check_async(local_rcv_num - 2, array, n, me, localB, localA,perc);
      finito = finito + check_async(1, array, n, me, localB, localA,perc);
    }

    MPI_Gatherv( & localA[from_snd_local * n], local_snd_num * n, MPI_INT, & A[0], & num_snd[0], & displ_snd[0], MPI_INT, 0, MPI_COMM_WORLD);

    if (me == 0) {
      for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
          if (A[i * n + j] == 1 || A[i * n + j] == 2) { // rosso
            fprintf(fptr, "\U0001f7e5 ");
          }

          if (A[i * n + j] == 3 || A[i * n + j] == 4) { // blu
            fprintf(fptr, "\U0001f7e6 ");
          }

          if (A[i * n + j] == 0) {
            fprintf(fptr, "\U0001f532 ");
          }
        }
        fprintf(fptr, "\n");
      }
      fprintf(fptr, "\n");

    }

    //controllo finito        
    MPI_Allreduce( & finito, & tot, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (tot == m * n) break;

    passi++;
    if(passi==passiTot) break;
    finito = 0;
    if (me == 0) {
      if (nproc > 1) {
        MPI_Isend( & localA[(local_rcv_num - 2) * n], n, MPI_INT,
          me + 1, 0, MPI_COMM_WORLD, & request[0]);
        MPI_Irecv( & localA[(local_rcv_num - 1) * n], n, MPI_INT,
          me + 1, 1, MPI_COMM_WORLD, & request[1]);
      }
    } else if (me == nproc - 1) {
      MPI_Isend( & localA[n], n, MPI_INT,
        me - 1, 1, MPI_COMM_WORLD, & request[1]);
      MPI_Irecv( & localA[0], n, MPI_INT,
        me - 1, 0, MPI_COMM_WORLD, & request[0]);

    } else {
      MPI_Isend( & localA[n], n, MPI_INT,
        me - 1, 1, MPI_COMM_WORLD, & request[1]);
      MPI_Isend( & localA[(local_rcv_num - 2) * n], n, MPI_INT,
        me + 1, 0, MPI_COMM_WORLD, & request[0]);
      MPI_Irecv( & localA[0], n, MPI_INT,
        me - 1, 0, MPI_COMM_WORLD, & request[0]);
      MPI_Irecv( & localA[(local_rcv_num - 1) * n], n, MPI_INT,
        me + 1, 1, MPI_COMM_WORLD, & request[1]);
    }

    finito = finito + check(to_snd_local, to, from, array, local_rcv_num, n, me, localA, localB, nproc,perc);
    MPI_Waitall(2, request, MPI_STATUSES_IGNORE);

    if (me == 0) {

      finito = finito + check_async(local_rcv_num - 2, array, n, me, localA, localB,perc);
    } else if (me == nproc - 1) {
      finito = finito + check_async(1, array, n, me, localA, localB,perc);
    } else {
      finito = finito + check_async(local_rcv_num - 2, array, n, me, localA, localB,perc);
      finito = finito + check_async(1, array, n, me, localA, localB,perc);
    }

    finito = 0;
    MPI_Gatherv( & localB[from_snd_local * n], local_snd_num * n, MPI_INT, & A[0], & num_snd[0], & displ_snd[0], MPI_INT, 0, MPI_COMM_WORLD);

    if (me == 0) {
      for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
          if (A[i * n + j] == 1 || A[i * n + j] == 2) { // rosso
            fprintf(fptr, "\U0001f7e5 ");
          }

          if (A[i * n + j] == 3 || A[i * n + j] == 4) { // blu
            fprintf(fptr, "\U0001f7e6 ");
          }

          if (A[i * n + j] == 0) {
            fprintf(fptr, "\U0001f532 ");
          }
        }
        fprintf(fptr, "\n");
      }
      fprintf(fptr, "\n");

    }
    //controllo finito
    MPI_Allreduce( & finito, & tot, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (tot == m * n) break;

  }

  if (me == 0) {
    free(A);
    free(displ_rcv);
    free(num_rcv);
    free(displ_snd);
    free(num_snd);
  }
  free(localA);
  free(localB);
  free(array);
  fclose(fptr);
  MPI_Finalize();
}
