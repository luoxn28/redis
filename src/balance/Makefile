hipath=../../deps/hiredis

objs=balance.o sds.o zmalloc.o ae.o \
	 hiredis.o net.o
files=balance.c balance.h ../sds.c ../sds.h ../zmalloc.c ../zmalloc.h ../ae.c ../ae.h
hiredisfiles=$(hipath)/hiredis.c $(hipath)/hiredis.h $(hipath)/net.c $(hipath)/net.h

redis-balance: $(objs) $(hiredisfiles)
	gcc -g -o redis-balance $(objs)

redis-balance.o: $(files)
	gcc -c balance.c 
sds.o: $(files)
	gcc -c ../sds.c
zmalloc.o: $(files)
	gcc -c ../zmalloc.c
ae.o: $(files)
	gcc -c ../ae.c

hiredis.o: $(hiredisfiles)
	gcc -c ../../deps/hiredis/hiredis.c
net.o: $(hiredisfiles)
	gcc -c ../../deps/hiredis/net.c

clean:
	rm $(objs)
