# This is the makefile for the simulator(s).  5/7/91  BRL
#
#
CC=cc 
RM=rm -f
INSTALL=install -c -s
CCOPTS=-O3 
BINDIR=/usr/local/bin
FTPDIR=/usr/local/ftp/pub/microprog/decmips
#
OPT=sim.o csio.o
OPT1=sim1.o micro1.o trace1.o logic1.o 
OPT2=sim2.o micro2.o trace2.o logic2.o 
OPT3=sim3.o micro3.o trace3.o logic3.o 
OPT4=sim4.o micro4.o trace4.o logic4.o 
ALLOPTS=$(OPT) $(OPT1) $(OPT2) $(OPT3) $(OPT4)
#
sim: sim1 sim2 sim3 sim4
#
.c.o:
	$(CC) -c $(CCOPTS) $<
#
sim1: $(OPT) $(OPT1)
	$(CC) $(CCOPTS) $(OPT) $(OPT1) -o sim1
#
sim2: $(OPT) $(OPT2)
	$(CC) $(CCOPTS) $(OPT) $(OPT2) -o sim2
#
sim3: $(OPT) $(OPT3)
	$(CC) $(CCOPTS) $(OPT) $(OPT3) -o sim3
#
sim4: $(OPT) $(OPT4)
	$(CC) $(CCOPTS) $(OPT) $(OPT4) -o sim4
#
clean:
	$(RM) $(ALLOPTS) core
#
tidy: realclean
#
realclean: clean
	$(RM) sim1 sim2 sim3 sim4 
#
install_bin: sim
	$(INSTALL) sim1 $(BINDIR)
	$(INSTALL) sim2 $(BINDIR)
	$(INSTALL) sim3 $(BINDIR)
	$(INSTALL) sim4 $(BINDIR)
#
install_ftp: sim
	$(INSTALL) sim1 $(FTPDIR)
	$(INSTALL) sim2 $(FTPDIR)
	$(INSTALL) sim3 $(FTPDIR)
	$(INSTALL) sim4 $(FTPDIR)
#
install: install_bin install_ftp
#
csio.o: sim.h
sim.o: sim.h mackey.c vaxkey.c lower.c version.h
sim1.o micro1.o trace1.o logic1.o: sim.h
sim1.o trace1.o logic1.o: micro1.h
sim2.o micro2.o trace2.o logic2.o: sim.h
sim3.o micro3.o trace3.o logic3.o: sim.h
sim4.o logic4.o micro4.o trace4.o: sim.h
sim1.o trace1.o logic1.o: micro1.h
sim2.o trace2.o logic2.o: micro2.h
sim3.o trace3.o logic3.o: micro3.h
sim4.o trace4.o logic4.o: micro4.h
trace1.o trace2.o trace3.o trace4.o: trace.c
micro1.o micro2.o micro3.o micro4.o: micro.c
