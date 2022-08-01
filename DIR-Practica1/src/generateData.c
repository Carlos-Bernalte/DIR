#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DATA "datos.dat"

int control_args(int argc, char *argv[], int n);

int main(int argc, char *argv[]){
    FILE *fp;
    int n;
    float aux_number, min;
    n = control_args(argc, argv, n);
    
    if((fp=fopen(DATA,"wb"))==NULL){
        fprintf(stderr,"[Error] No se puede abrir el archivo %s.\n", DATA);
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    for(int i=0;i<n;i++){
        fprintf(fp,"%f",rand()%100);
        if(i!=n)
        fputs(",",fp);
    }
    fclose(fp);
    return EXIT_SUCCESS;
}
int control_args(int argc, char *argv[], int n){
    if(argc < 2){
        fprintf(stderr,"Incorrecto el número de argumentos\n");
        exit(EXIT_FAILURE);
    }
    n=atoi(argv[1]);
    if(n<=0){
        fprintf(stderr,"Introduce un número mayor que 0\n");
        exit(EXIT_FAILURE);
    }
    return n;
}