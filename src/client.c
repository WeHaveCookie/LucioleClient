///////////////////////////////////////////////////////////////////////////////
//			Application: Client				                            	 //
///////////////////////////////////////////////////////////////////////////////
//									                                         //
//			 programme  CLIENT				                                 //
//									                                         //
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//      Auteurs : Rachex / Blanc / Gerry                                     //
//      Date : 11/04/2015                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <signal.h>

// PACKAGE UNIX
#include <curses.h>         /* Primitives de gestion d'Žcran */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/fon.h" /* primitives de la boite a outils */

#include <pthread.h>
#include <SDL/SDL.h> // Chargement de la SDL pour l'affichage 2D
#include "../include/game.h"

#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "udp"
#define SERVEUR_DEFAUT "127.0.0.1"
#define HEIGHT 480
#define WIDTH 640
#define RUNUI 1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

int run;

int num_soc; // identificateur de la socket cliente

pthread_t thread_game;
pthread_t thread_message;

int id_client = 1;
int nbr_player = 2;

char tampon_Send[1];
char tampon_Receive[1];

typedef enum TypeMess TypeMess;
enum TypeMess{
    MOVE, INIT, CONNECT
};

int client_appli (char *serveur, char *service, char *protocole);

// Programme CLIENT ///

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    run = 1;

    char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
    char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */
    char *protocole= PROTOCOLE_DEFAUT; /* protocole par defaut */

    /* Permet de passer un nombre de parametre variable a l'executable */
    switch(argc)
    {
    case 1 :        /* arguments par defaut */
          printf("serveur par defaut: %s\n",serveur);
          printf("service par defaut: %s\n",service);
          printf("protocole par defaut: %s\n",protocole);
          break;
    case 2 :        /* serveur renseigne  */
          serveur=argv[1];
          printf("service par defaut: %s\n",service);
          printf("protocole par defaut: %s\n",protocole);
          break;
    case 3 :        /* serveur, service renseignes */
          serveur=argv[1];
          service=argv[2];
          printf("protocole par defaut: %s\n",protocole);
          break;
    case 4:         /* serveur, service, protocole renseignes */
          serveur=argv[1];
          service=argv[2];
          protocole=argv[3];
          break;
    default:
          printf("Usage:client serveur(nom ou @IP)  service (nom ou port)  protocole (TCP ou UDP)\n");
          exit(1);
    }

    return client_appli(serveur,service,protocole);
}

void sendMessage(int idPlayer, Move dpl, TypeMess t_mss);

void launchGame() {
    if (RUNUI) {
        // On charge le systeme d'affichage et le systeme de son
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1) {
            fprintf(stderr, "Erreur dans le chargement de la SDL\n");
            SDL_Quit(); // On arrete la SDL
            pthread_exit(NULL);
        }
        atexit(SDL_Quit);
        SDL_Event event;

        // On cree notre screen de 640x360 en 32bit couleur charge sur la carte graphiqe en mode double buffering
        SDL_Surface* screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF );
        if (screen == NULL) {
            fprintf(stderr, "Impossible de charger le mode video : %s\n", SDL_GetError());
            SDL_Quit(); // On arrete la SDL
            pthread_exit(NULL);
        }

        // On titre notre fenetre
        SDL_WM_SetCaption("Super Unicorn Run", NULL);

        // On active la repetition de touches. On attend 10ms avant d'activer la repetition et on renouvelle l'event toutes les 10ms
        SDL_EnableKeyRepeat(10, 10);

        /* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
        /* service le numero de port sur le serveur correspondant au  */
        /* service desire par le client */
        /* protocole le protocole qui sera utilise pour la communication */

        if (init(id_client,nbr_player,screen) == EXIT_FAILURE) {
            printf("Fail to init game\n");
            SDL_Quit(); // On arrete la SDL
            pthread_exit(NULL);
        }
        while(run)
        {
            // Gestionnaire d'evenements
            SDL_WaitEvent(&event);
            switch(event.type)
            {
                case SDL_QUIT:
                    //mutex pour run
                        run = 0;
                        // envoie du message au autres client que celui-ci se ferme
                        sendMessage(getIdPlayer(),UP,CONNECT);
                        break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_UP: // On appuie sur la touche haut
                            movePlayer(getIdPlayer(),UP);
                            sendMessage(getIdPlayer(),UP,MOVE);
                            break;
                        case SDLK_DOWN: // On appuie sur la touche bas
                            movePlayer(getIdPlayer(),DOWN);
                            sendMessage(getIdPlayer(),DOWN,MOVE);
                            break;
                        case SDLK_RIGHT: // On appuie sur la touche droite
                            movePlayer(getIdPlayer(),RIGHT);
                            sendMessage(getIdPlayer(),RIGHT,MOVE);
                            break;
                        case SDLK_LEFT: // On appuie sur la touche gauche
                            movePlayer(getIdPlayer(),LEFT);
                            sendMessage(getIdPlayer(),LEFT,MOVE);
                           break;
                        // Event de test pour l'ajout et la suppression de joueur.
                        case SDLK_a:
                            leavePlayer(2);
                            break;
                        case SDLK_z:
                            newPlayer(2);
                            break;
                    }
                    break;
            }
            // On efface l'ecran
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 255, 0));

            // On place les joueurs visible sur l'ecran
            int i;
            Player **linkedPlayer = getLinkedPlayer();
            for(i=0;i<getNbrPlayer();i++) {
                if (linkedPlayer[i]->visible == 1) {
                    SDL_BlitSurface(linkedPlayer[i]->image, NULL, screen, linkedPlayer[i]->position);
                }
            }

            // On met a jour l'affichage
            SDL_Flip(screen);
        }

        SDL_Quit(); // On arrete la SDL
        pthread_exit(NULL);
    }
    return EXIT_SUCCESS;
}

void sendMessage(int idPlayer, Move dpl, TypeMess t_mss) {
    uint8_t mess = 0x00;

    mess = idPlayer << 5;

    switch(t_mss) {
        case MOVE:
            mess = (dpl << 2) | MOVE;
            break;
        case CONNECT:
            // Le joueur envoie un message de déconnexion
            mess = 1 << 3 | CONNECT;
            break;
        default:
            break;
    }

    tampon_Send[0] = mess;
    h_writes(num_soc, tampon_Send, sizeof(char));

}

void receiveMessage(int mss) {
    int type = mss & 0x03;
    int id_player = ((mss & 0xe0) >> 5)+1;

    int status = 0;
    int dpl = 0;


    switch(type) {
        case MOVE:
            dpl = (mss & 0x0c) >> 2;
            movePlayer(id_player,dpl);
            break;
        case CONNECT:
            status = (mss & 0x04) >> 3;
            if(status)
                leavePlayer(id_player);
            else
                newPlayer(id_player);
            break;
        default:
            break;
    }
}

void *wait_message() {
    int mss;
    while(run){
        // Des qu'il recoit un message -> appel a move dans game
        h_reads(num_soc, tampon_Receive, sizeof(char));
        mss = tampon_Receive[0];
        receiveMessage(mss);
    }
}

int wait_init() {

    h_reads(num_soc, tampon_Receive, sizeof(char));
    int mss = tampon_Receive[0];

    int type = mss & 0x03;
    int id_player = (mss & 0xe0) >> 5;

    if(type == INIT) {
        id_client = id_player;
        nbr_player = (mss & 0x1c) >> 2;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}


///////////////////////////////////////////////////////////////////////////////
int client_appli (char *serveur,char *service,char *protocole) {

    SOCKET sock = h_socket(AF_INET,SOCK_STREAM);
    SOCKADDR_IN paramsock;
    paramsock.sin_addr.s_addr = inet_addr(serveur);
    paramsock.sin_family = AF_INET;
    paramsock.sin_port = htons(atoi(service));
    h_connect(sock,&paramsock);

    if(wait_init() == 0) {
        if(pthread_create(&thread_message,NULL,wait_message,NULL)) {
            //h_close(sock);
            return EXIT_FAILURE;
        } else {
    	//if(pthread_create(&thread_game,NULL,launchGame,NULL) == 0) {
    		// Permet de récuperer les messages envoyés par les autres joueurs
    		//wait_message();
    		launchGame();
    		// Attend que le thread du jeu soit fini pour fermer le programme
    		//pthread_join(thread_game,NULL);
    		//pthread_join(thread_message,NULL);
    		// On ferme le socket avant de terminer le programme
    		//h_close(sock);
    		return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}


