MODELCHR=s
CC=wcl
CFLAGS=-0 -bt=dos -m$(MODELCHR) -I. -q -s -oh -os
LD=wlink
LDFLAGS=system dos option quiet

all: dumpcis.exe dumppreg.exe

dumpcis.exe: dumpcis.obj pcmctrl.obj cisparse.obj
	$(LD) $(LDFLAGS) file { $^ }

dumppreg.exe: dumppreg.obj pcmctrl.obj
	$(LD) $(LDFLAGS) file { $^ }

%.obj: %.c
	$(CC) $(CFLAGS) -c $<

xfer:
	nc -w1 -c localhost 12345 < dumpcis.exe

xfer-reg:
	nc -w1 -c localhost 12345 < dumppreg.exe

clean:
	rm -rf *.exe *.obj

clean-wmake: .SYMBOLIC
	del *.exe *.obj
