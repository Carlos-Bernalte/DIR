DIRBIN := bin/
DIRSRC := src/
CC:= mpicc
RUN:= mpirun


all: dirs compilar

dirs:
	mkdir -p $(DIRBIN)

compilar:
	mpicc $(DIRSRC)pract2.c -o $(DIRBIN)pract2 -lX11

normal:
	$(RUN) -np 1 $(DIRBIN)pract2 1

sepia:
	$(RUN) -np 1 $(DIRBIN)pract2 2

contraste_bajo:
	$(RUN) -np 1 $(DIRBIN)pract2 3

