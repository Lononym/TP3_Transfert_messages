/*
TP Executif temps-reel manipulation 3 : Transfert de messages
Mardi 11 avril 2022
BRIARD Mathis & DUFRENE Victor
ETN4 G2
*/

/*
Elements VxWorks requis par l'application :
    - 3 tâches (Estime_Heure, Corrige_Heure et Affiche)
    - 1 semaphore (H10)
    - 2 files de messages (FMHeureReelle et FMHeureLocale)
    - 2 variables partagées (DivH10 et Horaire)
*/

/*inclusion des bibliotheques*/
// #include "vxWorks.h"
// #include "semLib.h"
// #include "taskLib.h" 
// #include "msgQLib.h"
// #include "TP3_Environnement.h"
#include "string.h"
#include "stdio.h"
#include "time.h" 


#define STACK_SIZE (size_t)20000
#define DivH10MAX 9

/*Declarations des semaphores*/
SEM_ID H10;

/*Declarations des files de messages*/
MSG_Q_ID FMHeureReelle;
MSG_Q_ID FMHeureLocale;

/*Declaration des TID*/
TASK_ID tidEstime_Heure;
TASK_ID tidCorrige_Heure;
TASK_ID tidAffiche;

/*Declarations des types*/
typedef struct  {
        int seconde;
        int minute;
        int heure;
        
} Horaire_t;

typedef struct{
        char provenance[10];
        Horaire_t horaire;
} HeureLocale_t;


/*Declaration des variables partagees*/
int DivH10;
Horaire_t horaire;

/*-----------------------------------*/
/*------------Fonctions--------------*/


/*-----------Estime Heure----------*/
void Estime_Heure(void)
{
        //Declaration des variables locales
        HeureLocale_t Heure_Locale;        
        //Attente du semaphore d'eveil et procedure
        while(1)
        {
                semTake(H10, WAIT_FOREVER);
                if(DivH10<DivH10MAX)
                {
                        DivH10++;
                }else{
                        DivH10=0;
                        (horaire.seconde++)%60;
                        if(horaire.seconde==0){
                                (horaire.minute++)%60;
                                if(horaire.minute == 0){
                                        (horaire.heure++)%24;
                                }
                        }							
                        char chaine[]="Estimee";
                        strcpy((Heure_Locale.provenance),chaine);
                        Heure_Locale.horaire=horaire;
                        msgQSend(FMHeureLocale, &Heure_Locale, sizeof(HeureLocale_t), WAIT_FOREVER, MSG_PRI_NORMAL);
                }
        }
}

/*----------Corrige Heure----------*/
void Corrige_Heure(void)
{
        HeureLocale_t Heure_Locale;
        Horaire_t HeureReelle;
        while(1)
        {
                msgQReceive(FMHeureReelle, &HeureReelle, sizeof(Horaire_t),WAIT_FOREVER);
                horaire=HeureReelle; // Peut être faire champ à champ
                DivH10=0;
                char chaine[]="Corrigee";
                strcpy((Heure_Locale.provenance),chaine);
                Heure_Locale.horaire=horaire;
                msgQSend(FMHeureLocale, &Heure_Locale, sizeof(HeureLocale_t), WAIT_FOREVER, MSG_PRI_NORMAL);
        }
}


/*-------------Affiche------------*/
void Affiche(void){
        //Declaration des variables locales
        HeureLocale_t Heure_Locale;
        //Procedure
        while(1){
                msgQReceive(FMHeureLocale, &Heure_Locale, sizeof(HeureLocale_t),WAIT_FOREVER);
                printf("Origine du message : %s\n", Heure_Locale.provenance);
                printf("l'heure est: %d heure, %d min, %d sec\n", Heure_Locale.horaire.heure, Heure_Locale.horaire.minute, Heure_Locale.horaire.seconde);
        }
}

/*-------------Application------------*/
int start()
{
        //Creation des semaphores
        H10=semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
        FMHeureReelle=msgQCreate(1, sizeof(Horaire_t), MSG_Q_FIFO); 
        FMHeureLocale=msgQCreate(1, sizeof(HeureLocale_t), MSG_Q_FIFO);

        
        //Demarrage des taches
        printf("Demarrage de l'estimation de l'horloge\n");
        tidEstime_Heure=taskSpawn("Estime_Heure", 13,0, STACK_SIZE, (FUNCPTR) Estime_Heure,0,0,0,0,0,0,0,0,0,0);
        
        printf("Demarrage de la correction de l'horloge\n");
        tidCorrige_Heure=taskSpawn("Corrige_Heure", 14,0, STACK_SIZE, (FUNCPTR)Corrige_Heure,0,0,0,0,0,0,0,0,0,0);
        
        printf("Demarrage de l'affichage\n");
        tidAffiche=taskSpawn("Affiche", 15,0, STACK_SIZE, (FUNCPTR)Affiche,0,0,0,0,0,0,0,0,0,0);
        
        startEnvironment(H10, FMHeureReelle);
        
        printf("Fin de start\n");
        return (EXIT_SUCCESS);
}

/*--------------Fermeture-------------*/
int stop()
{
        //printf("Debut de stop\n");
        stopEnvironment();
        
        //Destruction des taches
        taskDelete(tidEstime_Heure);
        taskDelete(tidCorrige_Heure);
        taskDelete(tidAffiche);
        
        //Desctruction des semaphores
        semDelete(H10);
        msgQDelete(FMHeureReelle);
        msgQDelete(FMHeureLocale);
        
        printf("Fin de stop\n");
        return(EXIT_SUCCESS);
}
