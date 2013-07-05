#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

typedef struct
{
	int turn;
	bool flag[2];
}deckdata;

int main()
{
	int shmid, status;
	deckdata *ch, *pa;

	shmid = shmget(IPC_PRIVATE, sizeof(deckdata), 0777|IPC_CREAT);

	pa = (deckdata *) shmat(shmid, NULL, 0);
	pa->flag[0] = false;
	pa->flag[1] = false;

	if(fork()==0)
	{
		//CHILD Process process 0
		ch = (deckdata *) shmat(shmid, NULL, 0);

		ch->flag[0] = true;
		ch->turn = 1;
		
		while(ch->turn==1&&ch->flag[1]==true);
		//Busy Wait

		cout <<"Entering Critical section of process 0" << endl;
		sleep(1+rand()%5);
		cout <<"Exit Critical section of process 0" << endl;

		ch->flag[0] = false;

		shmdt(ch);
		exit(0);
	}
	else
	{
		pa = (deckdata *) shmat(shmid, NULL, 0);

		pa->flag[1] = true;
		pa->turn = 0;
		
		while(pa->turn==0 && pa->flag[0]==true);
		//Busy Wait

		cout <<"Entering Critical section of process 1" << endl;
		sleep(1+rand()%5);
		cout <<"Exit Critical section of process 1" << endl;

		pa->flag[1] = false;

		shmdt(pa);
	}

	wait(&status);

	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
