to compile all
make or make all	-	This compiles all the modules.

to compile specific
make dining_phil	-	This compiles dining_phil.c. dining_phil.c creates the forked process which exec host and philosopher.
make host			-	This compiles host.c. host is responsible for initializing data, i.e, setting up the table and forks for the philosophers.
make philosopher	-	This compiles philosopher.c. philosopher is responsible for doing the philosophers job, i.e, thinking eating and contending for forks.

to clean
make clean			-	This cleans up the PWD.

about the implementation
I've implemented the functionality in the lib_sem.h. These functions are called by host and philosopher. Appropriate comments are made in the source code files.


to run
./dining_phil < no. of philosophers > < no. of iterations >
example ./dining_ohil 5 2 for 5 philosophers and 2 iterations

**note	-	make sure there are atleast 2 philosophers. If the number of philosophers are less than 2, the program exits because of shortage of forks.
Atleast 2 forks are required to proceed and this is available only if the number of philosophers are greater than or equal to 2.