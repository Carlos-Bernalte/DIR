DIRBIN := bin/
DIRSRC := src/
CC:= mpicc
RUN:= mpirun
Nodes=8

all: dirs toroide hipercubo
	
dirs:
	mkdir -p $(DIRBIN)

toroide:
	$(CC) $(DIRSRC)toroide.c -o $(DIRBIN)Toroide

hipercubo:
	$(CC) $(DIRSRC)hipercubo.c -o $(DIRBIN)Hipercubo

run-toroide:
	$(RUN) --oversubscribe -n $(Nodes) $(DIRBIN)Toroide
	
run-hipercubo:
	$(RUN) --oversubscribe -n $(Nodes) $(DIRBIN)Hipercubo