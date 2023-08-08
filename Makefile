CC ::= gcc
FILES ::= base64
TESTDIR ::= test
TESTS ::= main base64
TESTS ::= $(TESTS:%=$(TESTDIR)/%)

test : $(FILES:%=%.o) $(TESTS:%=%.o)
	$(CC) -o $(TESTDIR)/run $^

%.o : %.c
	$(CC) -o $@ -c $^
