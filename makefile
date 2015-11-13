.SILENT:
AT=ApplicTerminaux
LT=LibrairieTerminaux
ST=ServeurTerminaux
OBJS=LibrairieTerminaux.o
CPP=c++ -Wno-deprecated -I$(AT) -I$(LT) -I$(ST)
all:	$(OBJS)	Main ApplicTerminaux.out ServeurTerminaux.out

LibrairieTerminaux.o:	$(LT)/LibrairieTerminaux.cxx $(LT)/LibrairieTerminaux.h
	echo Creation de LibrairieTerminaux.o ...
	$(CPP) $(LT)/LibrairieTerminaux.cxx -c

Main:	LibrairieTerminaux.o $(LT)/Main.cxx
	echo Creation de Main ...
	$(CPP) $(LT)/Main.cxx $(OBJS) -o Main

ApplicTerminaux.out:	$(AT)/ApplicTerminaux.cxx $(AT)/ApplicTerminaux.h
	echo Creation de ApplicTerminaux ...
	$(CPP) $(AT)/ApplicTerminaux.cxx $(OBJS) -lpthread -D_REENTRANT -lsocket -lnsl -o ApplicTerminaux.out

ServeurTerminaux.out:	$(ST)/ServeurTerminaux.cxx $(ST)/ServeurTerminaux.h
	echo Creation de ServeurTerminaux ...
	$(CPP)  $(ST)/ServeurTerminaux.cxx $(OBJS) -lpthread -D_REENTRANT -lsocket -lnsl -o ServeurTerminaux.out

clean:
	rm -f $(OBJS) core Main ApplicTerminaux.out ServeurTerminaux.out

clobber:	clean
	rm -f tags $(PROGRAM) *.log
