CC	= gcc
CFLAGS	= -g -I. -std=c99 -Wall -D__USE_FIXED_PROTOTYPES__

graphs: graphs.o builder.o inspect.o bheap.o paths.o maker.o printer.o rand.o pcg_basic.o map_tokens.o a_star.o
		$(CC) $(CFLAGS) -o $@ $^

graphs.o: graphs.c graphs.h builder.h bheap.h inspect.h maps/maker.h maps/paths.h maps/printer.h traversal/a_star.h utils/rand.h utils/utils.h
		$(CC) $(CFLAGS) -c -o $@ $<

builder.o: builder.c builder.h graphs.h utils/utils.h
		$(CC) $(CFLAGS) -c -o $@ $<

inspect.o: inspect.c inspect.h graphs.h bheap.h
		$(CC) $(CFLAGS) -c -o $@ $<

bheap.o: bheap.c bheap.h utils/utils.h
		$(CC) $(CFLAGS) -c -o $@ $<

rand.o: utils/rand.c utils/rand.h utils/utils.h
		$(CC) $(CFLAGS) -c -o $@ $<

pcg_basic.o: pcg_basic.c pcg_basic.h
		$(CC) $(CFLAGS) -c -o $@ $<

maker.o: maps/maker.c maps/maker.h utils/utils.h utils/rand.h
		$(CC) $(CFLAGS) -c -o $@ $<

paths.o: maps/paths.c maps/paths.h maps/maker.h utils/utils.h utils/rand.h
		$(CC) $(CFLAGS) -c -o $@ $<

printer.o: maps/printer.c maps/printer.h maps/maker.h maps/paths.h maps/printer/map_tokens.h utils/utils.h
		$(CC) $(CFLAGS) -c -o $@ $<

map_tokens.o: maps/printer/map_tokens.c maps/printer/map_tokens.h utils/token.h
		$(CC) $(CFLAGS) -c -o $@ $<

a_star.o: traversal/a_star.c traversal/a_star.h bheap.h maps/maker.h maps/paths.h utils/utils.h
		$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) graphs *.o **/**/*~
