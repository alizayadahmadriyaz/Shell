# Shell 
A shell using unix-like system calls that implements command execution, foreground and background processes , I/O redirection and pipes.

## File Structure
```
.
├── include
│   ├── cd.h
│   ├── checkProc.h
│   ├── clock.h
│   ├── execute.h
│   ├── ls.h
│   ├── mylimits.h
│   ├── parseInput.h
│   ├── pinfo.h
│   ├── printPrompt.h
│   └── strip.h
├── README.md
└── src
    ├── cd.c
    ├── checkProc.c
    ├── clock.c
    ├── execute.c
    ├── getstatus.c
    ├── ls.c
    ├── main.c
    ├── Makefile
    ├── myshell
    ├── obj
    ├── parseInput.c
    ├── pinfo.c
    ├── printPrompt.c
    └── strip.c

3 directories, 26 files
```

## Run Using
```bash
cd src
make
./myshell
```

