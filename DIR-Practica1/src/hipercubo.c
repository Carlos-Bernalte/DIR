#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define XOR(a, b) (a ^ b)

#define D 3

enum NEIGHBOURS
{
    North = 0,
    East = 1,
    South = 2,
    West = 3
};

int readData(float listNumbers[]);
void getNeighbours(int rank, int neighbours[]);
float maximum(int rank, float number, int neighbours[]);

int main(int argc, char *argv[]){

    int rank, size, nNumbers, error, N = pow(2,D);
    float min, number;
    float listNumbers[N];
    int neighbours[4];

    MPI_Status status;
    MPI_Request request;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != N) {
        fprintf(stderr, "[ERROR] El número de nodos no es igual a las dimensiones del Toroide\n  -Nodos: %d\n  -Tamaño del Toroide: %d\n", size, N);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
       
        if ((nNumbers = readData(listNumbers)) != size) {
            fprintf(stderr, "[ERROR] No hay el mismo número de nodos que de números para repartir\n  -Nodos: %d\n  -Números para repartir: %d\n", size, nNumbers);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        for (int i = 0; i < nNumbers; i++){
            MPI_Send(&listNumbers[i], 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }         
    }

    MPI_Recv(&number, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
    getNeighbours(rank, neighbours);
    maximum(rank, number, neighbours);
        

    MPI_Finalize();
    return 0;
}

void getNeighbours(int rank, int neighbours[])
{
    int masc;
    for(int i=0;i<D;i++){
        masc = 1 << i;
        neighbours[i] = XOR(rank, masc);
    
    }
}

float maximum(int rank, float number, int neighbours[])
{

    MPI_Status status;
    MPI_Request request;
    float recived;

    for (int i = 0; i < D; i++){
        MPI_Isend(&number, 1, MPI_FLOAT, neighbours[i], 1, MPI_COMM_WORLD, &request);
        MPI_Recv(&recived, 1, MPI_FLOAT, neighbours[i], 1, MPI_COMM_WORLD, &status);

        number=MAX(number, recived);
    }
    if(rank==0)
        printf("El valor maximo es %3f\n", number);
}

int readData(float lista_numeros[]){
    FILE* File;
    char buffer[1024];
    int nNumbers=0;
    char *token;
    
    if ((File = fopen("datos.dat","r")) == NULL){
        fprintf(stderr,"Error en la lectura del archivo.\n");
        exit(EXIT_FAILURE);
    }
    if(fscanf(File,"%s",buffer)!=EOF){
        token = strtok(buffer,",");
        do{
            lista_numeros[nNumbers++] = atof(token);
        }while((token = strtok(NULL,",")) != NULL);
    }
    fclose(File);
    return nNumbers;
}