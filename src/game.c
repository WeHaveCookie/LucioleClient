#include "../include/game.h"

///////////////////////////////////////////////////////////////////////////////////
int launch() {
/*    m_gameRun = 1;
    if(phtread_create(&m_displayer, NULL, threadDisplayer, NULL) == -1) {
        perror("");
        return EXIT_FAILURE;
    }*/
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
void threadDisplayer() {
//    while(m_gameRun == 1) {


//    }
}

///////////////////////////////////////////////////////////////////////////////////
void movePlayer(int idPlayer, Move dpl) {
    switch(dpl) {
        case LEFT: // On vas a gauche
            m_linkedPlayer[idPlayer-1]->position->x = ((m_linkedPlayer[idPlayer-1]->position->x - SPEED) < 0)?0:(m_linkedPlayer[idPlayer-1]->position->x - SPEED);
            break;
        case RIGHT: // On vas a droite
            m_linkedPlayer[idPlayer-1]->position->x = ((m_linkedPlayer[idPlayer-1]->position->x + m_linkedPlayer[idPlayer-1]->image->w + SPEED) > m_screen->w)?m_screen->w-m_linkedPlayer[idPlayer-1]->image->w:(m_linkedPlayer[idPlayer-1]->position->x + SPEED);
            break;
        case DOWN: // On vas en haut
            m_linkedPlayer[idPlayer-1]->position->y = ((m_linkedPlayer[idPlayer-1]->position->y + m_linkedPlayer[idPlayer-1]->image->h + SPEED) > m_screen->h)?m_screen->h-m_linkedPlayer[idPlayer-1]->image->h:(m_linkedPlayer[idPlayer-1]->position->y + SPEED);
            break;
        case UP: // On vas en bas
            m_linkedPlayer[idPlayer-1]->position->y = ((m_linkedPlayer[idPlayer-1]->position->y - SPEED) < 0)?0:(m_linkedPlayer[idPlayer-1]->position->y - SPEED);
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////
void newPlayer(int idPlayer) {
    m_linkedPlayer[idPlayer-1]->position->x = (m_screen->w - m_linkedPlayer[idPlayer-1]->image->w)/2;
    m_linkedPlayer[idPlayer-1]->position->y = (m_screen->h - m_linkedPlayer[idPlayer-1]->image->h)/2;
    m_linkedPlayer[idPlayer-1]->visible = 1;
}

///////////////////////////////////////////////////////////////////////////////////
void leavePlayer(int idPlayer) {
    m_linkedPlayer[idPlayer-1]->visible = 0;
}

///////////////////////////////////////////////////////////////////////////////////
Player **getLinkedPlayer() {return m_linkedPlayer;}

///////////////////////////////////////////////////////////////////////////////////
int getNbrPlayer() {return m_nbrPlayer;}

///////////////////////////////////////////////////////////////////////////////////
// Renvoie l'identificateur du joueur
int getIdPlayer() {return m_idPlayer;}

///////////////////////////////////////////////////////////////////////////////////
int initPlayer(Player *pl, int id) {
    //SDL_Surface* aspect = (SDL_Surface*) malloc(sizeof(SDL_Surface));
    // On positionne les bombers
    printf("Entree dans initPlayer\n");
    switch(id) {
        case 0:
                m_linkedPlayer[id]->image = SDL_LoadBMP("./ressource/BomberWhite.bmp");
                m_linkedPlayer[id]->position->x = 0;
                m_linkedPlayer[id]->position->y = 0;
                break;
            case 1:
                m_linkedPlayer[id]->image = SDL_LoadBMP("./ressource/BomberBlue.bmp");
                m_linkedPlayer[id]->position->x = 0;
                m_linkedPlayer[id]->position->y = m_screen->h-m_linkedPlayer[id]->image->h;
                break;
            case 2:
                m_linkedPlayer[id]->image = SDL_LoadBMP("./ressource/BomberRed.bmp");
                m_linkedPlayer[id]->position->x = m_screen->w-m_linkedPlayer[id]->image->w;
                m_linkedPlayer[id]->position->y = 0;
                break;
            case 3:
                m_linkedPlayer[id]->image = SDL_LoadBMP("./ressource/BomberBlack.bmp");
                m_linkedPlayer[id]->position->x = m_screen->w-m_linkedPlayer[id]->image->w;
                m_linkedPlayer[id]->position->y = m_screen->h-m_linkedPlayer[id]->image->h;
                break;
            default:
                m_linkedPlayer[id]->image = SDL_LoadBMP("./ressource/BomberWhite.bmp");
                m_linkedPlayer[id]->position->x = (m_screen->w - m_linkedPlayer[id]->image->w)/2;
                m_linkedPlayer[id]->position->y = (m_screen->h - m_linkedPlayer[id]->image->h)/2;
                break;
        }
        if (!m_linkedPlayer[id]->image) { return EXIT_FAILURE;}
        printf("Joueur n°%i | x = %i | y = %i\n",id,m_linkedPlayer[id]->position->x,m_linkedPlayer[id]->position->y);
        // On cree notre masque pour la transparence
        SDL_SetColorKey(m_linkedPlayer[id]->image, SDL_SRCCOLORKEY, SDL_MapRGB(m_linkedPlayer[id]->image->format, 0, 255, 0 ));
        printf("Masque cree\n");
        // On rend visible le bombers
        m_linkedPlayer[id]->visible = 1;
        printf("Pl visible\n");
        return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
int init(int idPlayer, int nbrPlayer, SDL_Surface* screen) {
    m_screen = screen;
    m_idPlayer = idPlayer;
    m_nbrPlayer = (nbrPlayer > MAXPLAYER)?MAXPLAYER:nbrPlayer;
    printf("Nbr de joueur : %i\n",m_nbrPlayer);
    int i;
    m_linkedPlayer = malloc(sizeof(Player*)*m_nbrPlayer);
    for(i=0;i<m_nbrPlayer;i++) {
        //initPlayer(&m_linkedPlayer[i],i);
        m_linkedPlayer[i] = malloc(sizeof(Player));
        m_linkedPlayer[i]->image = malloc(sizeof(SDL_Surface));
        m_linkedPlayer[i]->position = malloc(sizeof(SDL_Rect));
        if (initPlayer(m_linkedPlayer[i],i) == EXIT_FAILURE) {return EXIT_FAILURE;}
    }
    return EXIT_SUCCESS;
}
