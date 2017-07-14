lib = -pthread

all : dining_phil.c philosopher.c host.c lib_sem.c lib_sem.h
	cc $(lib) -o dining_phil dining_phil.c
	cc $(lib) -o host host.c
	cc $(lib) -o philosopher philosopher.c

dining_phil : dining_phil.c lib_sem.h lib_sem.c
		cc $(lib) -o dining_phil dining_phil.c

host : host.c lib_sem.h lib_sem.c
	cc $(lib) host host.c

philosopher : philosopher.c lib_sem.c lib_sem.h
		cc $(lib) philosopher philosopher.c

clean :
	rm dining_phil host philosopher *.dat
