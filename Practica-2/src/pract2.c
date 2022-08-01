#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>
#define NIL (0)

#define NODOS 2
#define LONGITUD_FICHERO 400

#define NORMAL 1
#define SEPIA 2
#define CONTRASTE_BAJO 3

#define FOTO "foto.dat"
/*Variables Globales */

XColor colorX;
Colormap mapacolor;
char cadenaColor[] = "#000000";
Display *dpy;
Window w;
GC gc;

/*Funciones auxiliares */
void initX();
void dibujaPunto(int i, int j, int r, int g, int b);
void arguments_control(int argc, char *argv[], float filtro[3]);
void trabajadores(int rank, MPI_Comm commPadre);

/* Programa principal */

int main(int argc, char *argv[])
{
      float filtro[3]={1,1,1};
      int rank, size;
      MPI_Comm commPadre;
      MPI_Status status;
      int errcodes[NODOS];
      int buff_punto[5]; /*Indicamos en 0->j, 1->i, 2->R, 3->G, 4->B*/
      
      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent(&commPadre);
      if ((commPadre == MPI_COMM_NULL) && (rank == 0))
      {
            initX();
            arguments_control(argc, argv, filtro);
            MPI_Comm_spawn("bin/pract2", MPI_ARGV_NULL, NODOS, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &commPadre, errcodes);
            for (int i = 0; i < NODOS; i++)
            {
                  MPI_Send(&filtro, 3, MPI_FLOAT, i, 0, commPadre);
            }
            
            for (int i = 0; i < LONGITUD_FICHERO * LONGITUD_FICHERO; i++)
            {
                  MPI_Recv(&buff_punto, 5, MPI_INT, MPI_ANY_SOURCE, 0, commPadre, &status);
                  dibujaPunto(buff_punto[0], buff_punto[1], buff_punto[2], buff_punto[3], buff_punto[4]);
            }
            sleep(2);
      }
      else
      {     
            trabajadores( rank,commPadre);
      }
      MPI_Finalize();
}
void initX()
{

      dpy = XOpenDisplay(NIL);
      assert(dpy);

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

      w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                              400, 400, 0, blackColor, blackColor);
      XSelectInput(dpy, w, StructureNotifyMask);
      XMapWindow(dpy, w);
      gc = XCreateGC(dpy, w, 0, NIL);
      XSetForeground(dpy, gc, whiteColor);
      for (;;)
      {
            XEvent e;
            XNextEvent(dpy, &e);
            if (e.type == MapNotify)
                  break;
      }

      mapacolor = DefaultColormap(dpy, 0);
}

void dibujaPunto(int i, int j, int r, int g, int b)
{

      sprintf(cadenaColor, "#%.2X%.2X%.2X", r, g, b);
      XParseColor(dpy, mapacolor, cadenaColor, &colorX);
      XAllocColor(dpy, mapacolor, &colorX);
      XSetForeground(dpy, gc, colorX.pixel);
      XDrawPoint(dpy, w, gc, i, j);
      XFlush(dpy);
}

void arguments_control(int argc, char *argv[], float filtro[3])
{
      if (argc != 2)
      {
            printf("Error en el número de argumentos. %d %s\n", argc, argv[0]);
            exit(EXIT_FAILURE);
      }
      switch (atoi(argv[1]))
      {
      case NORMAL:
            filtro[0] = 1;
            filtro[1] = 1;
            filtro[2] = 1;
            break;
      case SEPIA:
            filtro[0] = 0.439;
            filtro[1] = 0.259;
            filtro[2] = 0.078;
            break;
      case CONTRASTE_BAJO:
            filtro[0] = 0.333;
            filtro[1] = 0.333;
            filtro[2] = 0.333;
            break;

      default:
            filtro[0] = 1;
            filtro[1] = 1;
            filtro[2] = 1;
            break;
      }
}
void trabajadores(int rank, MPI_Comm commPadre){
      MPI_File file;
      MPI_Status status;
      unsigned char rgb[3];     
      int buff_punto[5], principo,final, filas_nodo;
      float filtro[3];

      filas_nodo = LONGITUD_FICHERO / NODOS;
      principo = rank * filas_nodo; 

      if (rank == NODOS - 1)
      {
            final = LONGITUD_FICHERO - 1;
      }else{
            final = ((rank + 1) * filas_nodo) - 1;
      }

      MPI_Offset area_rank = filas_nodo * LONGITUD_FICHERO * 3 * sizeof(unsigned char); 
      MPI_Offset total_area = area_rank * rank;  

      MPI_Recv(&filtro, 3, MPI_FLOAT, 0, 0, commPadre, &status);                                                                                        

      MPI_File_open(MPI_COMM_WORLD, FOTO, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);

      MPI_File_set_view(file, total_area, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native", MPI_INFO_NULL);

      for (int i = principo; i <= final; i++)
      {
            for (int j = 0; j < LONGITUD_FICHERO; j++)
            {

                  MPI_File_read(file, rgb, 3, MPI_UNSIGNED_CHAR, &status);
                  buff_punto[0] = j;
                  buff_punto[1] = i;
                  buff_punto[2] = rgb[0]*filtro[0];
                  buff_punto[3] = rgb[1]*filtro[1];
                  buff_punto[4] = rgb[2]*filtro[2];

                  MPI_Bsend(buff_punto, 5, MPI_INT, 0, 0, commPadre);
            }
      }

      MPI_File_close(&file);
}
