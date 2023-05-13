#include "synchro.h"
#include "ensitheora.h"
#include <pthread.h>



extern bool fini;

/* les variables pour la synchro, ici */
//mutex
pthread_mutex_t taillemutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t texturemutex = PTHREAD_MUTEX_INITIALIZER;
//condition
pthread_cond_t taillecond = PTHREAD_COND_INITIALIZER;
pthread_cond_t texturecond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
//conditions d'attente
bool taille_calculated= false;
bool texture_pret = false;
bool window_pret = false;
//nombre de textures
int nbre_textures = 0;




/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
  pthread_mutex_lock(&taillemutex);
  windowsx = buffer[0].width;
  windowsy = buffer[0].height;
  taille_calculated = true;
  pthread_cond_signal(&taillecond);
  pthread_mutex_unlock(&taillemutex);
}

void attendreTailleFenetre() {
   pthread_mutex_lock(&taillemutex);
   while (!taille_calculated)
    pthread_cond_wait(&taillecond,&taillemutex);
   pthread_mutex_unlock(&taillemutex);
}

void signalerFenetreEtTexturePrete() {
   pthread_mutex_lock(&texturemutex);
   texture_pret = true;
   pthread_cond_signal(&texturecond);
   pthread_mutex_unlock(&texturemutex);
}

void attendreFenetreTexture() {
   pthread_mutex_lock(&texturemutex);
   while(!texture_pret){
      pthread_cond_wait(&texturecond,&texturemutex);
  }
   pthread_mutex_unlock(&texturemutex);
}

void debutConsommerTexture() {
  pthread_mutex_lock(&texturemutex);
  while(!nbre_textures){
        pthread_cond_wait(&cond, &texturemutex);
    }
  pthread_mutex_unlock(&texturemutex);
}

void finConsommerTexture() {
  pthread_mutex_lock(&texturemutex);
  nbre_textures--;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&texturemutex);

}


void debutDeposerTexture() {
  pthread_mutex_lock(&texturemutex);
  while (nbre_textures == NBTEX) {
    pthread_cond_wait(&cond, &texturemutex);
  }
  pthread_mutex_unlock(&texturemutex);
}

void finDeposerTexture() {
    pthread_mutex_lock(&texturemutex);
    nbre_textures++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&texturemutex);
}
