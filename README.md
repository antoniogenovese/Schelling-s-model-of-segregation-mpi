A parallel implementation of the Shelling's model using OpenMPI.

Realizzato per il corso Programmazione Concorrente e Parallela su Cloud

1. Introduzione al problema

Il modello di compone di una griglia.Gli agenti si dividono in due gruppi e ogni agente uno occupare uno spazio alla volta. Ogni agente desidera avere un vicinato la cui componente (ignorando spazi bisanchi) sia per almeno il 30% appartenente al suo gruppo.
In ogni round l'agente controlla che il suo vicinato soddisfi questo creiterio, in caso contrario l'agente si trasferisce in uno spazio libero. La simulazione si interrompe dopo un numero massimo di S step oppure quando tutti gli agenti sono soddisfatti.


2. Prima Soluzione proposta
La prima soluzione prevede che tutti i processi lavorino equamente su sottomatrici (quasi) uguali. Possiamo distinguere tre parti:
suddivisione della matrice, esecuzione parallela sulle sottomatrici e raccolta delle stesse.

  2.1. Suddivisione della matrice
Data la matrice iniziale M*N e P processori, i processori 0 e P-1 riceveranno (M/P)+1 righe, mentre per i restanti vengono assegnate (M/P)+2 righe. Inoltre, nel caso in cui N non sia multiplo di P assegnando ai primi R processi una riga in più. 


  2.2. Esecuzione parallela
La soluzione prevede che in ogni passo i processori lavorino dalla seconda alla penultima riga della sottomatrice assegnatagli. La prima e ultima riga sono utilizzate esclusivamente per il controllo dei vicini delle righe adiacenti. Essendo però queste due righe oggetto di lavoro di altri processori, che quindi possono cambiarle, i processori devono aspettare che gli altri P gli inviino le righe aggiornate. Per ottenere un'esecuzione più efficiente le righe non interessate a questo scambio iniziano il lavoro controllando se per ogni cella il vicinato rispetta i requisiti desiderati. Si ripete l'esecuzione finchè tutte le celle non sono soddisfatte per un numero massimo di S passi.

  2.3. Raccolta dei dati
Al termien degli S passi si riuniscono tutte le sottomatrici e si stampa il tempo totale di esecuzione. Quest'ultimo viene calcolato a partire dalla suddivisione della matrice fino alla raccolta delle sottomatrici da parte del processo master.

3. Dettagli implementativi

La struttura dati scelta è una semplice matrice di interi per rappresentare la griglia del modello.

  3.1. Distribuzione dei dati
Ogni processo lavora su due sottomatrici (lettura e scrittura) di dimensioni uguali e utilizzerà un array di dimensione N per lo scambio degli agenti.

Ogni processo calcola dunque la dimensione della sua sottomatrice e il numero di righe che riceve dalla ScatterVall'inizio e che deve inviare alla GatherV alla fine. Inoltre il master calcola quante e quali righe deve ricevere ongi processore dalla matrice principale.

  3.2. Esecuzione parallela
All'inizio di ogni passo i processi comunicano tra loro per l'invio asincrono delle righe ai bordi, che dunque dipendono da altri processi, nel frattempo che le comunicazioni si concludano, ogni processo lavora sulle righe intermedie della matrice, per poi attendere la fine della comunicazione e lavorare sulla prima e ultima riga, appena ricevute tramite le comunicazioni.


//comunicazione con altri processi
if (me==0){
  if(nproc>1){
    MPI_Isend(&localB[(local_rcv_num-2) * n], n, MPI_INT,
        me+1,  0 ,MPI_COMM_WORLD,&request[0]);
    MPI_Irecv(&localB[(local_rcv_num-1) * n], n ,MPI_INT,
        me+1,  1 ,MPI_COMM_WORLD,&request[1]);    
  }
} else if(me==nproc-1){    
  MPI_Isend(&localB[n], n, MPI_INT,
      me-1,  1 ,MPI_COMM_WORLD,&request[1]);
  MPI_Irecv( &localB[0], n ,MPI_INT,
      me-1, 0 ,MPI_COMM_WORLD,&request[0]);
  }else {  
  MPI_Isend(&localB[n], n, MPI_INT,
      me-1,  1 ,MPI_COMM_WORLD,&request[1]);
  MPI_Isend(&localB[(local_rcv_num-2) * n], n, MPI_INT,
      me+1,  0 ,MPI_COMM_WORLD,&request[0]);
  MPI_Irecv( &localB[0], n ,MPI_INT,
      me-1, 0 ,MPI_COMM_WORLD,&request[0]);
  MPI_Irecv(&localB[(local_rcv_num-1) * n], n ,MPI_INT,
      me+1,  1 ,MPI_COMM_WORLD,&request[1]);
  }

//funzione check dei vicini delle celle appartenenti alle righe non di confine, non dipendono da altri processi

MPI_Waitall(2, request, MPI_STATUSES_IGNORE);    

//check dei vicini delle celle appartenenti alle righe di confine

La funzione check restituisce un valore intero "finito" che indica il numero di celle soddisfatte.
A seguire della computazione sulle sottomatrici viene effettuato una Allreduce che unisce tutti i valori "finito" per ogni processore per controllare se tutte le celle della matrice principale siano soddisfatte

La funzione check eseguita da ogni processore controlla per ogni cella della sottomatrice se i vicini (non vuoti) siano per almeno il 30% appartenenti al proprio gruppo. In caso contrario sposta il valore della cella in questione nella prima posizione libera della riga. Lo swap viene effettuato utilizzando le sottomatrici (scrittura e lettua) e un array di dimensione N. Quando un valorte deve essere cambiato, questo swap deve essere visibile solo nella sottomatrice in scrittura, quella in lettura non deve subire cambiamenti (dal momento che i vari processori condividono le preime e le ultime due righe, il cambiamento su una di queste dovrebbe essere visto anche dall'altro). L'array è utilizzato per copiare inizialmente la riga in esame ed effettuare lì i vari swap. 

  3.3. Raccolta dei dati
La parte di raccolta dei dati viene semplicemente gestita attraverso una GatherV, i cui parametri sono stati calcolati precedentemente.

4. Correttezza della soluzione
Riguardo la correttezza sono stati creati due file C, uno per l'esecuzione sequenziale e uno per l'esecuzone parallela. I due codici stampano ad ogni passo la matrice prodotta in un file txt. Un terzo file C confronta i due file di testo stampando il risultato del controllo.

Eseguire i seguenti comandi:

  mpicc -o sequenz corr_sequenz.c
  mpicc -o para corr_para.c 
  mpirun --allow-run-as-root ./sequenz M N S T 
  mpirun --allow-run-as-root -np P ./para M N S T

Dove M è il numero di righe, N è il numero di colonne, P il numero di processori, S il numero di passi, T (da 0 a 100) la percentuale di compiacimento. Successivamente, verranno prodotti due file .txt contenenti le stampe della matrice lungo i diversi passi. 

Eseguire dunque i seguenti comandi:

  gcc check_correctness.c -o check
  ./check

L'esecuzione restituerà l'esito del confronto.
