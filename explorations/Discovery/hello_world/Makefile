PROG = hello_world
SRCS = hello_world.c
OBJS = $(SRCS:.c=.o)
CLEANFILES = $(PROG)
INSTALL_DIR = ./install_artifact/usr/local/bin

# Add / change option in CFLAGS if needed
# CFLAGS += <new option>

$(PROG):  $(OBJS) 
	echo $(CC)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

all: $(PROG)
 
 
clean:
	rm -f $(CLEANFILES) $(patsubst %.c,%.o, $(SRCS)) *~

install: $(PROG)
	mkdir -p $(INSTALL_DIR)
	install $(PROG) $(INSTALL_DIR)

#on fait ca pour configurer le pc en cross compilation / ne marche pas donc on l'appelle pas pr l'instant
cc_configure: 
	. ../../../../SDK/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi
