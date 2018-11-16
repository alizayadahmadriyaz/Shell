IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -g

ODIR=obj

_DEPS = ls.h cd.h strip.h pinfo.h clock.h printPrompt.h execute.h checkProc.h parseInput.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ls.o cd.o strip.o pinfo.o clock.o printPrompt.o execute.o checkProc.o parseInput.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

myshell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 