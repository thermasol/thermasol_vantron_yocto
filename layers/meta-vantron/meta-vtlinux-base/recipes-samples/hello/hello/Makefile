PROG = hello
SRCS = hello.c
CLEANFILES = $(PROG)

all: $(PROG)
$(PROG): $(SRCS)
	$(CC) -o $@ $^

clean:
	rm -f $(CLEANFILES) $(patsubst %.c,%.o, $(SRCS))
