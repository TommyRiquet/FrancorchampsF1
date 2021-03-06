
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include "voiture.h"
#include "config.h"
#include "temps.h"


void final(Voiture *v,int numVoiture,sem_t *sem) {

    srand(getpid() + time(NULL));

    sem_wait(sem);
    v->num = numVoiture;
    v->status = 0;
    v->tour = 0;
    v->tempTotal = 0;
    sem_post(sem);


    while (v->tour <= FINALTOURS - 1) {

        sem_wait(sem);

        if (crachTest() || v->out == 1) {//Si crach
            if (v->out != 1) {
                switch(getRandomSecteur()) {
                    case 1 :
                        v->secteur[0] = 0;
                        v->secteur[1] = 0;
                        v->secteur[2] = 0;
                        v->out = 1;
                        v->status = 2;
                        break;
                    case 2 :
                        v->secteur[0] = genererTemps(v->tour);
                        v->secteur[1] = 0;
                        v->secteur[2] = 0;
                        v->out = 1;
                        v->status = 2;
                        break;
                    case 3 :
                        v->secteur[0] = genererTemps(v->tour);
                        v->secteur[1] = genererTemps(v->tour);
                        v->secteur[2] = 0;
                        v->out = 1;
                        v->status = 2;
                        break;
                }

            }

        } else {//Si pas crach

            v->secteur[0] = genererTemps(v->tour);
            v->secteur[1] = genererTemps(v->tour);

            //Arrèt au stand
            v->probaStand = 1;
            if (stand() == v->probaStand) {
                v->secteur[2] = genererTemps(v->tour) + randomStandTime();
                v->probaStand = PROBASTAND * 2;
                v->stand = 1;
                v->status = 1;
            } else {
                v->stand = 0;
                v->status = 0;
                v->secteur[2] = genererTemps(v->tour);
            }


            //Ajout du temps total
            v->tempTotal += v->secteur[0] + v->secteur[1] + v->secteur[2];

            //Ajout d'un tour
            v->tour = v->tour + 1;

            //Récupération et Allocation des meilleurs scores
            if (v->secteur[0] < v->bestSecteur[0] || v->bestSecteur[0] == 0) {
                v->bestSecteur[0] = v->secteur[0];
            }
            if (v->secteur[1] < v->bestSecteur[1] || v->bestSecteur[1] == 0) {
                v->bestSecteur[1] = v->secteur[1];
            }
            if (v->secteur[2] < v->bestSecteur[2] || v->bestSecteur[2] == 0) {
                v->bestSecteur[2] = v->secteur[2];
            }

            //Récupération du meilleur temps de tour
            if (v->secteur[0] + v->secteur[1] + v->secteur[2] < v->bestLap || v->bestLap == 0) {
                v->bestLap = v->secteur[0] + v->secteur[1] + v->secteur[2];
            }
        }

        sem_post(sem);

        sleep(DELAY);

    }
}

