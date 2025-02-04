#include <sys/shm.h>
#include "semaphores2.h"

//structure de la mémoire partagée
typedef struct data{
int nbEmbarques;
int nbDebarques;
int nbTourneeMax;
int nbClient;
}shdata;

//fonction chargement
void chargement(int n)
{
	printf("Nouveau chargement, tournée numéro %d \n",n);
}

//fonction rouler
void rouler()
{
	printf(" tshout tshout !!! le train roule \n");
	printf(" __________ \n");
	printf("|   train  \\_ \n");
	printf(" o-o-o-o-o-o-| \n");
	sleep(2);
	printf(" __________    \t       \t   __________\n");
	printf("|   train  \\_ \t ----> \t  |   train  \\_ \n");
	printf(" o-o-o-o-o-o-| \t  \t   o-o-o-o-o-o-| \n");
	sleep(2);
	printf(" __________    \t        \t   __________  \t          \t   __________\n");
	printf("|   train  \\_ \t  ----> \t  |   train  \\_  \t ----> \t  |   train  \\_ \n");
	printf(" o-o-o-o-o-o-| \t        \t   o-o-o-o-o-o-| \t    \t   o-o-o-o-o-o-| \n");
	sleep(2);
}

//fonction Decharger
void Decharger()
{
	printf("\n Train prêt pour le déchargement \n");
}


/*****************************PROGRAMME PRINCIPALE****************************/


int main(int argc, char *argv[])
{
int n = 1; int p = 4;
int i,j,att1,att2;
int semEmbarquement = 0; int semDebarquement = 1;
int semTousDehors = 2; int semTousAbord = 3;

int nb = (int) atoi(argv[1]);

//mettre dans ce fichier la création et initialisation de tout les sémaphores nécessaires.
//creation d'une clé
key_t key = ftok("/home/izan/Desktop",'u');

//création de l"espace mémoire partagé
int shmid = shmget(key,sizeof(shdata), IPC_CREAT | IPC_EXCL | 0666);
if(shmid==-1){
//la zone existe deja !
shmid=shmget(key,sizeof(shdata),0); //recuperer son id
}

//attacher le processus a la zone de mémoire partagée
shdata *sd= NULL;
sd = shmat(shmid,sd,0);

//initialiser les variables partagées
sd->nbEmbarques = 0;
sd->nbDebarques = 0;
sd->nbClient = 0;
sd->nbTourneeMax = nb;

//création des sémaphores
//groupe de 4 semaphores
int semid = semcreate(key,4);

//initialiser les sémaphores
seminit(semid , semEmbarquement , 0);
seminit(semid , semDebarquement , 0);
seminit(semid , semTousDehors , 0);
seminit(semid , semTousAbord , 0);


/****************************************************************************************/


while (1){

chargement(n); //afficher un message, nouveau chargement tournée numéro i
n++;

if(sd->nbClient < p) { att1 = sd->nbClient; att2 = att1; }
else { att1 = p; att2 =  p; }

for( i = 1 ; i <=att1 ; i++)
{
	V(semid, semEmbarquement );
} //la il va débloquer p processus client bloques dans la file d'attente du sémaphore semEmbarquement

printf("nbClient en attente = %d \n",att2);
Ptimed(semid, semTousAbord); //ici le processus voiture se bloque et attend d’être libéré par le dernier client qui monte, pour ensuite rouler ….
 if(att2 <= p/3)
{
	printf("\n\t******* train annulé !!! pas assé de passagers *******\n");
	Decharger(); //afficher message, processus voiture prêt pour le déchargement
}
else{
	
	rouler(); //afficher un message et sleep(4).
	Decharger(); //afficher message, processus voiture prêt pour le déchargement
}




for( j = 1 ; j <= p ; j++)
{
V(semid, semDebarquement); //il va libérer tout les processus clients qui font la queue pour descendre 
					//(c'est a dire bloques dans l’opération P(semDebarquement)).
}

Ptimed(semid, semTousDehors); //le processus voiture se bloque ici et attend que le dernier client qui descent le reveil
}

//semdelete(semid);
return 0;
}
