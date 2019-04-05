all:player ringmaster

player:player.c potato.h 
	gcc -Wall -g -Werror -std=gnu99 player.c -o player 
ringmaster:ringmaster.c potato.h 
	gcc -Wall -g -Werror -std=gnu99 ringmaster.c -o ringmaster

clean:
	rm -f *.o ringmaster player 





