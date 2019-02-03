MODELCHR=s
CC=wcc
CFLAGS=-0 -bt=dos -m$(MODELCHR) -I. -q -s -oh -os
LD=wlink
LDFLAGS=system dos option quiet

dumpcis.exe: dumpcis.obj
	$(LD) $(LDFLAGS) file $<

dumpcis.obj: dumpcis.c
	$(CC) $(CFLAGS) $<

xfer:
	nc -w1 -c localhost 12345 < dumpcis.exe

clean:
	rm -rf *.exe *.obj
