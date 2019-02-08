CC= gcc
CFLAGS= -Wall -O2
LDFLAGS= 

PROG= looplicht
OBJ= main.o i2c_ext.o pca9532.o

.PHONY: all clean install

all: $(PROG)

$(PROG): $(OBJ)
	 $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

%.o: %.c
	 $(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	-rm -rf $(PROG) $(OBJ)

install: $(PROG)
	mkdir -p $(prefix)/sbin/
	cp $(PROG) $(prefix)/sbin/
