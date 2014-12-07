CC = gcc
CFLAGS = -lreadline
DEPS = inc.h

CShell: CShell.c AutoComplete.c $(DEPS)
	$(CC) -o CShell CShell.c $(CFLAGS)

clean:
	rm -f CShell 