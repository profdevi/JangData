#v1.29 copyright Comine.com 20160108F1043
# Simple Make file for nmake
#  -Revised to exclude extra search libraries

OUTPUT=main.exe
LIBS=
CC=cl.exe
CLINKER=link.exe
BASELIBS=
FLAGS=-nologo -DWIN32 -O2 -DNDEBUG -MT -DUNICODE -D_UNICODE -D_CONSOLE -Iinclude -EHsc
CFLAGS=-I . 
CLINKFLAGS=                                 # /NODEFAULTLIB:LIBCMT 
DSNNAME=UnitTest
BUILDDBSQL=dbbuild.tmp
DROPDBSQL=dbdrop.tmp


########### Main.exe ##############
all : ResourceFiles $(OUTPUT)
	@banner "Finished creating $(OUTPUT)"


###################################
ResourceFiles:
	mrcgen -a


########### Main.lib ##############
library: *.obj
	-rm -f $(OUTPUT)
	lib /out:$(OUTPUT) *.obj
	@banner "Finished creating library $(OUTPUT)"


############ Compile Object Files ################
.cpp.obj:
	$(CC) -c $(CFLAGS)  $(FLAGS) $**
	-$(CC) -c $(CFLAGS)  $(FLAGS) *.c
	cp setargv.vc14  setargv.obj


############ Compile Object Files ################
.c.obj:
	$(CC) -c $(CFLAGS)  $(FLAGS) $**
	-$(CC) -c $(CFLAGS)  $(FLAGS) *.cpp
	cp setargv.vc14  setargv.obj


########### Compile Resource Files ################
*.res: *.rc
	$(RC) -r $(RCFLAGS) $**


############ Link Object Files  ##################
$(OUTPUT) : *.obj *.res
	$(CLINKER) $(CLINKFLAGS)  $**  $(LIBS)  $(BASELIBS)  -OUT:$@


############# Clean ###############
clean : 
	-rm -f *.obj /q
	

############ Clean All Exe+Objs #######
cleanall:
	-rm -f *.obj 
	-rm -f *.tli
	-rm -f *.tlh
	-rm -f $(OUTPUT)
	-rm -f $(BUILDDBSQL)
	-rm -f $(DROPDBSQL)
	-rm $(DSNNAME).mdb
	-MODBCDSN -type=access -remove -dsn=$(DSNNAME)



#####################################################
DBCreate:
	-rm -f $(BUILDDBSQL)
	-rm -f $(DROPDBSQL)
	-mcat *Build.sql -o $(BUILDDBSQL)
	-mcat *Drop.sql -o $(DROPDBSQL)
	-sqlexec dsn=$(DSNNAME) $(DROPDBSQL)
	-MODBCDSN -type=access -remove -dsn=$(DSNNAME)
	-rm $(DSNNAME).mdb
	MODBCDSN -type=access -createdb -file=$(DSNNAME).mdb
	MODBCDSN -type=access -add -dsn=$(DSNNAME) -file=$(DSNNAME).mdb
	sqlexec dsn=$(DSNNAME) $(BUILDDBSQL)
	@banner "Executed BUILD SQL for Database"



#####################################################
DBDestroy:
	-rm -f $(BUILDDBSQL)
	-rm -f $(DROPDBSQL)
	-mcat *Build.sql -o $(BUILDDBSQL)
	-mcat *Drop.sql -o $(DROPDBSQL)
	-sqlexec dsn=$(DSNNAME) $(DROPDBSQL)
	@banner "Executed Drop SQL for Database"




############ bison and lex processing  #########
yacc:
	-bison -d -o bison.cpp bison.yac
	-mv bison.hpp bison.h
	-flex -oflex.cpp flex.lex




