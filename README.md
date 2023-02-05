[Schelling’s model of segregation](https://github.com/antoniogenovese/Schelling-s-model-of-segregation-mpi)

A parallel implementation of the Shelling's model using OpenMPI.

![](shelling_model.gif)

Realizzato per il corso Programmazione Concorrente e Parallela su Cloud
# **1. Introduzione al problema**
Il modello di compone di una griglia di agenti divisi in due gruppi. Ogni agente può occupare uno spazio alla volta e desidera avere un vicinato la cui componente (ignorando spazi bianchi) sia per almeno il 30% appartenente al suo gruppo.

In ogni round l'agente controlla che il suo vicinato soddisfi questo criterio, in caso contrario l'agente si trasferisce in uno spazio libero. La simulazione si interrompe dopo un numero massimo di S step oppure quando tutti gli agenti sono soddisfatti.
# **2. Soluzione proposta**
La soluzione prevede che tutti i processori lavorino equamente su sottomatrici (quasi) uguali. Possiamo distinguere la soluzione in tre parti: suddivisione della matrice, esecuzione parallela sulle sottomatrici e raccolta della matrice finale.
##   **2.1. Suddivisione della matrice**
Data la matrice iniziale M\*N e P processori, i processori 0 e P-1 riceveranno (M/P)+1 righe, mentre per i restanti vengono assegnate (M/P)+2 righe. Inoltre, nel caso in cui N non sia multiplo di P assegniamo ai primi R processori una riga in più. 
##   **2.2. Esecuzione parallela**
La soluzione prevede che in ogni passo i processori lavorino dalla seconda alla penultima riga della sottomatrice assegnatagli. La prima e l’ultima riga sono utilizzate esclusivamente per il controllo dei vicini delle righe adiacenti. Essendo però queste due righe oggetto di lavoro di altri processori, che quindi possono cambiarle, i processori devono aspettare che gli altri P gli inviino le righe aggiornate. Per ottenere un'esecuzione più efficiente le righe non interessate a questo scambio iniziano il lavoro controllando se per ogni cella il vicinato rispetta i requisiti desiderati. Una volta ricevuti gli aggiornamenti dagli altri processori, si controllano le righe non ancora ispezionate. Si ripete l'esecuzione finché tutte le celle non sono soddisfatte o per un numero massimo di S passi.
##   **2.3. Raccolta dei dati**
Al termine degli S passi si riuniscono tutte le sottomatrici e si stampa il tempo totale di esecuzione. Quest'ultimo viene calcolato a partire dalla suddivisione della matrice fino alla raccolta delle sottomatrici da parte del processo master.
# **3. Dettagli implementativi**
La struttura dati scelta è una semplice matrice di interi per rappresentare la griglia del modello.

##   **3.1. Distribuzione dei dati**

Ogni processo lavora su due sottomatrici (lettura e scrittura) di dimensioni uguali e utilizzerà un array di dimensione N per lo scambio degli agenti.

Ogni processo chiama la funzione “def\_var” dove calcola la dimensione della sua sottomatrice, il numero di righe che lavorano in maniera sincrona, il numero di righe che riceve dalla ScatterV e il numero di righe che deve inviare nella GatherV. Inoltre, il master calcola quante e quali righe deve ricevere ogni processore dalla matrice principale, variabili utili per ScatterV e GatherV.
##   **3.2. Esecuzione parallela**
All'inizio di ogni passo i processi comunicano tra loro per l'invio asincrono delle righe ai bordi, che dunque dipendono da altri processi, nel frattempo che le comunicazioni si concludano, ogni processo lavora sulle righe intermedie della matrice, per poi attendere la fine della comunicazione e lavorare sulla prima e ultima riga, appena ricevute.
```
//comunicazione  tra processi

if (me==0){

     MPI\_Isend(&localB[(local\_rcv\_num-2) \* n], n, MPI\_INT,

         me+1,  0 ,MPI\_COMM\_WORLD,&request[0]);
        MPI\_Irecv(&localB[(local\_rcv\_num-1) \* n], n ,MPI\_INT,

          me+1,  1 ,MPI\_COMM\_WORLD,&request[1]);    

} else if(me==nproc-1){    

     MPI\_Isend(&localB[n], n, MPI\_INT,

         me-1,  1 ,MPI\_COMM\_WORLD,&request[1]);

     MPI\_Irecv( &localB[0], n ,MPI\_INT,

         me-1, 0 ,MPI\_COMM\_WORLD,&request[0]);

     }else {  

     MPI\_Isend(&localB[n], n, MPI\_INT,

         me-1,  1 ,MPI\_COMM\_WORLD,&request[1]);

     MPI\_Isend(&localB[(local\_rcv\_num-2) \* n], n, MPI\_INT,

        me+1,  0 ,MPI\_COMM\_WORLD,&request[0]);

     MPI\_Irecv( &localB[0], n ,MPI\_INT,

        me-1, 0 ,MPI\_COMM\_WORLD,&request[0]);

     MPI\_Irecv(&localB[(local\_rcv\_num-1) \* n], n ,MPI\_INT,

        me+1,  1 ,MPI\_COMM\_WORLD,&request[1]);

     }

//funzione check: controllo dei vicini delle celle appartenenti alle righe non di confine, non dipendono da altri processi

MPI\_Waitall(2, request, MPI\_STATUSES\_IGNORE);    

//check dei vicini delle celle appartenenti alle righe di confine
```
La funzione check restituisce un valore intero "finito" che indica il numero di celle soddisfatte.

A seguire della computazione sulle sottomatrici viene effettuato una Allreduce che unisce tutti i valori "finito" per ogni processore per controllare se tutte le celle della matrice principale siano soddisfatte.

La funzione check eseguita da ogni processore controlla per ogni cella della sottomatrice se i vicini (non vuoti) siano per almeno il 30% appartenenti al proprio gruppo. In caso contrario sposta il valore della cella in questione nella prima posizione libera della riga. Lo swap viene effettuato utilizzando le sottomatrici (scrittura e lettua) e un array di dimensione N. Quando un valore deve essere cambiato, questo swap deve essere visibile solo nella sottomatrice in scrittura, quella in lettura non deve subire cambiamenti (dal momento che i vari processori condividono le prime e le ultime due righe, il cambiamento su una di queste dovrebbe essere visto anche dall'altro). L'array è utilizzato per copiare inizialmente la riga in esame ed effettuare lì i vari swap. 
##    **3.3. Raccolta dei dati**
La parte di raccolta dei dati viene semplicemente gestita attraverso una GatherV, i cui parametri sono stati calcolati precedentemente.
# **4. Correttezza della soluzione**
Riguardo la correttezza sono stati creati due file C, uno per l'esecuzione sequenziale e uno per l'esecuzione parallela. I due codici stampano ad ogni passo la matrice prodotta in un file txt. Un terzo file C confronta i due file di testo stampando il risultato del controllo.
# **5. Esecuzione correttezza**
Eseguire i seguenti comandi:

     mpicc -o sequenz corr\_sequenz.c

     mpicc -o para corr\_para.c 

     mpirun --allow-run-as-root ./sequenz M N S T 

     mpirun --allow-run-as-root -np P ./para M N S T

Dove M è il numero di righe, N è il numero di colonne, P il numero di processori, S il numero di passi, T (da 0 a 100) la percentuale di compiacimento. Successivamente, verranno prodotti due file .txt contenenti le stampe della matrice lungo i diversi passi. 

Eseguire dunque i seguenti comandi:

     gcc check\_correctness.c -o check

     ./check

L'esecuzione restituerà l'esito del confronto.
# **6 Esecuzione Benchmarks**
Riguardo l'esecuzione dei file per i benchmarks è necessario eseguire i seguenti comandi:

     mpicc sequenz\_bench.c -o sequenz

     mpicc para\_bench.c -o para

     mpirun --allow-run-as-root ./sequenz M N S T

     mpirun --allow-run-as-root -np P ./para M N S T
# **7. Raccolta Benchmark**
Per la fase di benchmarking è stato testato il comportamento sia in termini di scalabilità forte (la dimensione dell'input è costante mentre il numero di processori varia), sia in termini di scalabilità debole (per ogni test ongi processore ha sempre lo stesso carico di lavoro). Le analisi sono state effettuate su un cluster GCP di 6 nodi (e2-standard-4), ognuno con 4 vCPU e 16GB di RAM. Dunque, si hanno due file: para\_bench.c e forest\_bench\_seq.c che prevedono solo la stampa del tempo di esecuzione. I dati raccolti sono stati calcolati ponendo il numero di passi totali S = 100 e la percentuale di compiacimento T = 30.

Per la strong scalability è stata testata una matrice 2000\*2000,

per la weack scalability ogni processore lavora ad una sottomatrice 2000\*160.
##        **7.1. Strong scalability**


|**#Processori**|**Time**|**Speedup**|**Efficiency**|
| :-: | :-: | :-: | :-: |
|1|29,4352|-|1|
|2|15,852|1,85|0,92|
|3|20,5184|1,43|0,47|
|4|15,4952|1,89|0,47|
|5|12,5444|2,34|0,46|
|6|10,4892|2,80|0,46|
|7|9,2844|3,17|0,45|
|8|8,424|3,49|0,43|
|9|7,578|3,88|0,43|
|10|6,8816|4,27|0,42|
|11|6,3076|4,66|0,42|
|12|5,8568|5,02|0,41|
|13|5,4292|5,42|0,41|
|14|4,938|5,96|0,42|
|15|4,7456|6,20|0,41|
|16|4,4404|6,62|0,41|
|17|4,354|6,76|0,39|
|18|4,13|7,12|0,39|
|19|3,8732|7,59|0,39|
|20|3,6904|7,97|0,39|
|21|3,5652|8,25|0,39|
|22|3,4784|8,46|0,38|
|23|3,4504|8,53|0,37|
|24|3,4116|8,62|0,35|

![](speedup.png)

![](efficiency.png)

Dalla tabella sopra riportata si evince uno speedup rallentato dall'overhead di comunicazione, che prevede ad ogni step lo scambio delle righe di confine e delle variabili necessarie per verificare che l'intera matrice sia vuota. Inoltre, come ci si aspettava, all'aumentare del numero di processi, il tempo di esecuzione decresce stabilmente appiattendosi verso la fine, sintomo dell'aumento dell'overhead di comunicazione rispetto alle righe da computare per ogni worker che invece decrescono.

Si può invece notare come l'efficienza sia stabile lungo le diverse esecuzioni, evidenziando un utilizzo quasi costante del singolo core, ciò dovuto al numero costante di elementi che vengono scambiati. Infatti, indipendentemente dal numero di processi, il processo i-esimo comunicherà sempre e solo le 4 righe di confine e la variabile empty\_counter.
##   **7.2. Weak scalability**
Come già affermato precedentemente, il workload scelto per ogni processo è di una matrice di dimensioni 2000\*160, dunque ogni processo va a lavorare su 320.000 celle, gli altri valori invece, sono gli stessi della strong scalability. N.B. Ogni esperimento ha previsto l'incremento delle sole colonne della matrice( 2000x160 per 1 p, 2000x320 per 2 p, 2000\*480 per 3 p...).

|**#Processori**|**Time**|
| :-: | :-: |
|1|1,4688|
|2|1,308|
|3|2,2544|
|4|2,3512|
|5|2,5216|
|6|2,7224|
|7|3,076|
|8|3,312|
|9|3,5944|
|10|3,6256|
|11|3,8064|
|12|4,0976|
|13|4,2336|
|14|4,4336|
|15|4,5416|
|16|4,8424|
|17|5,0848|
|18|5,1096|
|19|5,2792|
|20|5,8192|
|21|5,8512|
|22|5,9056|
|23|6,208|
|24|6,2152|

# **8. Conclusioni**

Per concludere, l'algoritmo giova sicuramente dalla parallelizzazione, anche se non appieno, ciò è dovuto alle comunicazioni che vengono effettuate ad ogni step e che limitano sicuramente quello che è lo speedup, di certo, evitando il controllo sull'assenza di alberi nella matrice si potrebbero ottenere dei miglioramenti. Per il resto, l'utilizzo di procedure non bloccanti e strategie atte al "risparmio" di memoria ha permesso di ottenere comunque buoni risultati.
