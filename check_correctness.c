#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    
    FILE *file_par, *file_seq;

    file_par = fopen("corr_para.txt", "r");
    file_seq = fopen("corr_sequenz.txt", "r");

   

    char seq_word[11];
    char par_word[11];
    
    if ((file_par = fopen("corr_para.txt", "r")) != NULL && 
    (file_seq = fopen("corr_sequenz.txt", "r")) != NULL){
        
        while((fgets(par_word, 11, file_par) != NULL) &&
        (fgets(seq_word, 11, file_seq) != NULL)){
            if(strcmp(par_word, seq_word) != 0){
                printf("Le matrici sono diverse! \n");
                fclose(file_par);
                fclose(file_seq);
                return 0;
            }
        }
    } 
    else {
        printf("FILE OPENING ERROR!\n");
    }
    printf("Le matrici sono uguali!\n");
    
    fclose(file_par);
    fclose(file_seq);

    return 0;
}
