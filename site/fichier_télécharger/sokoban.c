#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define TAILLE  12
#define TAILLE_DEP 1000 

typedef char t_plateau[TAILLE][TAILLE];
t_plateau plateau;
typedef char t_deplacement[TAILLE_DEP];
t_deplacement deplacement;
const char UNDO = 'u';
const char DEPLA_G = 'g';
const char DEPLA_D = 'd';
const char DEPLA_B = 'b';
const char DEPLA_H = 'h';
const char DEPLA_G_C = 'G';
const char DEPLA_D_C = 'D';
const char DEPLA_B_C = 'B';
const char DEPLA_H_C = 'H';
const char SOKOBAN = '@' ;
const char CAISSE = '$'  ; 
const char GAUCHE = 'q' ;
const char SOKOBAN_CIBLE = '+' ; 
const char CAISSE_CIBLE = '*' ;
const char CIBLE = '.' ;
const char MUR = '#' ;
const char HAUT = 'z' ;
const char BAS = 's' ; 
const char DROITE = 'd' ; 
const char RETRY = 'r' ; 
const char ABANDON = 'x' ;
const char ESPACE = ' ' ;
const char ZOOM = '+';
const char DEZOOM = '-';

void charger_partie(t_plateau plateau, char fichier[]) ;
void enregistrer_partie(t_plateau plateau, char fichier[]) ;
int kbhit() ;
void affiche_plateau(t_plateau plateau, int zoom) ;
void deplacer (char touche , t_plateau plateau , 
    t_deplacement deplacement,int  *iDeplacement) ;
void deplacer_haut (int lSokoban,int cSokoban,t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void deplacer_bas (int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void deplacer_droite (int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void deplacer_gauche (int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
bool gagne(t_plateau plateau) ;
void deplacer_droite_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void deplacer_bas_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void deplacer_gauche_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void deplacer_haut_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement);
void retour_arriere(int *iDeplacement,t_deplacement  deplacement);
void undo_haut_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void undo_bas_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void undo_gauche_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void undo_droite_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void enregistrerDeplacements(t_deplacement t, int nb, char fic[]);

int main(){
    char fichier[20];
    char touche ;
    int sauvegarde ;
    int nbCoup = 0 ;
    int iDeplacement = 0 ;
    int zoom = 1;
    int save ;
    char save_move[20];
    printf("entrer le nom du fichier (ne pas oublier le .sok a la fin ;) )");
    scanf("%s", fichier) ;
    charger_partie(plateau , fichier) ;
    printf("Entrez la touche pour vous déplacer"); 
    while (kbhit() == 0){
    }
    touche = getchar();
    affiche_plateau( plateau, zoom);
    while ((gagne( plateau) == false) && (touche != ABANDON)){
        printf("Entrez la touche pour vous déplacer\n");
        while (kbhit() == 0){
        }
        touche = getchar();
        if ((touche == HAUT) | (touche == DROITE) 
            | (touche == GAUCHE) | (touche == BAS)){
                deplacer(touche,plateau,deplacement,&iDeplacement);
                nbCoup = nbCoup + 1 ;
            }
        else if (touche == RETRY){
            charger_partie(plateau , fichier) ;
        }
        else if (touche == UNDO){
            retour_arriere(&iDeplacement, deplacement);
        }
        else if ((touche == ZOOM)){
            zoom++;
        }
        else if ((touche == DEZOOM) && (zoom!=1)){
            zoom = zoom - 1 ;
        }
        
        affiche_plateau( plateau,zoom);
        printf("le nombre de coup est : %d\n" , nbCoup);
    }
    affiche_plateau( plateau, zoom);
    printf("voulez vous sauvegarder vos mouvement(1 oui)");
    scanf("%d", &save);
    if (save==1){
        printf("mettre nom du fichier\n");
        scanf("%s", save_move);
        enregistrerDeplacements(deplacement, iDeplacement, save_move);
    }
    if (gagne( plateau) == true){
        printf(" gagner \n");
    }
    if (touche == ABANDON){
        printf("voulez vous sauvegarder (1 = oui ,autre touche = non)\n");
        scanf("%d" , &sauvegarde);
        if (sauvegarde == 1){
            printf("nom se la sauvegarde (.sok a la fin)\n");
            scanf("%s" , fichier);
            enregistrer_partie(plateau, fichier);
            nbCoup = 0 ;
            iDeplacement = 0 ;
        }

    }
    return EXIT_SUCCESS ;
}

void retour_arriere(int *iDeplacement,t_deplacement deplacement){
    //permet de regarder quelle retour arriere faire 
    //et enlever le deplacement fait en dernier
    int lSokoban ;
    int cSokoban ;
    int i ;
    int j ;
    // la boucle permet de localiser le sookban
    for (i = 0 ; i < TAILLE ; i++){
        for (j = 0 ; j < TAILLE ; j++){
            if ((plateau[i][j] == SOKOBAN) | (plateau[i][j] == SOKOBAN_CIBLE)){
                lSokoban = i ;
                cSokoban = j ;
            }
        }
    }
    if (deplacement[(*iDeplacement)-1] == DEPLA_B){
        deplacer_haut(lSokoban,cSokoban,plateau,deplacement,&(*iDeplacement));
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    else if (deplacement[(*iDeplacement)-1] == DEPLA_H){
        deplacer_bas(lSokoban,cSokoban,plateau,deplacement,&(*iDeplacement));
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    else if (deplacement[(*iDeplacement)-1] == DEPLA_G){
        deplacer_droite(lSokoban,cSokoban,plateau,
            deplacement,&(*iDeplacement));
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    else if (deplacement[(*iDeplacement)-1] == DEPLA_D){
        deplacer_gauche(lSokoban,cSokoban,plateau,
            deplacement,&(*iDeplacement));
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    else if (deplacement[(*iDeplacement)-1] == DEPLA_H_C){
        undo_haut_caisse(lSokoban,cSokoban,plateau);
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    else if (deplacement[(*iDeplacement)-1] == DEPLA_G_C){
        undo_gauche_caisse(lSokoban,cSokoban,plateau);
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    } 
    else if (deplacement[(*iDeplacement)-1] == DEPLA_D_C){
        undo_droite_caisse(lSokoban,cSokoban,plateau);
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    else if (deplacement[(*iDeplacement)-1] == DEPLA_B_C){
        undo_bas_caisse(lSokoban,cSokoban,plateau);
        deplacement[*iDeplacement]=ESPACE;
        (*iDeplacement) = (*iDeplacement) - 1;
    }
    if (*iDeplacement<0){
        (*iDeplacement) = 0;
    }
}

bool gagne(t_plateau plateau){
    // regerde si on a gagner et retourne resultat
    int i ;
    int j ;
    bool resultat = true;
    // la boucle permet de localiser si il reste une cible sans caiise
    for (i = 0 ; i < TAILLE ; i++){
        for (j = 0 ; j < TAILLE ; j++){
            if ((plateau[i][j] == CIBLE ) 
                | (plateau[i][j] == SOKOBAN_CIBLE)
                    |(plateau[i][j] == CAISSE)){
                        resultat = false ;
            }
        }
    }
    return resultat ;
}

void undo_haut_caisse(int lSokoban , int cSokoban , t_plateau plateau){
    // fait le retour arriere pour une caisse si on est aller en haut
        if ((plateau[lSokoban+1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban-1][cSokoban] = CIBLE ;
                
            }
        else if ((plateau[lSokoban+1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban-1][cSokoban] == CAISSE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban-1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban+1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban-1][cSokoban] == CAISSE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban-1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban+1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban-1][cSokoban] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban-1][cSokoban] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban-1][cSokoban] == CAISSE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban-1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban-1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban-1][cSokoban] == CAISSE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban-1][cSokoban] = ESPACE;
            }
}

void undo_bas_caisse(int lSokoban , int cSokoban , t_plateau plateau){
    // fait le retour arriere pour une caisse si on est aller en bas
        if ((plateau[lSokoban-1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] = CIBLE ;
            }
        else if ((plateau[lSokoban-1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban+1][cSokoban] == CAISSE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban-1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban+1][cSokoban] == CAISSE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban+1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban-1][cSokoban] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban+1][cSokoban] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban+1][cSokoban] == CAISSE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban+1][cSokoban] = ESPACE ;
            }
        else if ((plateau[lSokoban+1][cSokoban] == CAISSE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban+1][cSokoban] = ESPACE;
            }
}

void undo_gauche_caisse(int lSokoban , int cSokoban , t_plateau plateau){
    // fait le retour arriere pour une caisse si on est aller a gauche
        if ((plateau[lSokoban][cSokoban+1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban-1] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban+1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban-1] == CAISSE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban-1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban+1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban][cSokoban-1] == CAISSE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban-1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban+1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban-1] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban-1] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban-1] == CAISSE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban-1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban-1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban-1] == CAISSE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban-1] = ESPACE;
            }
}

void undo_droite_caisse(int lSokoban , int cSokoban , t_plateau plateau){
    // fait le retour arriere pour une caisse si on est aller a droite
        if ((plateau[lSokoban][cSokoban-1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban-1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban+1] == CAISSE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban-1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban][cSokoban+1] == CAISSE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban+1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban-1] == CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN)
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = CIBLE ;
            }
        else if ((plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)
            && (plateau[lSokoban][cSokoban+1] == CAISSE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban-1] = ESPACE ;
            }
        else if ((plateau[lSokoban][cSokoban+1] == CAISSE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CAISSE ;
                plateau[lSokoban][cSokoban-1] = ESPACE;
            }
}

void deplacer_haut_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        // fait le deplacement avec une caisse pour le  haut
        if ((plateau[lSokoban - 2][cSokoban] == CIBLE) 
            && (plateau[lSokoban - 1][cSokoban] == CAISSE_CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban - 2][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban - 1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban - 2][cSokoban] == CIBLE) 
            && (plateau[lSokoban - 1][cSokoban] == CAISSE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban - 2][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban - 1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban - 2][cSokoban] == ESPACE) 
            && (plateau[lSokoban - 1][cSokoban] == CAISSE_CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban - 2][cSokoban] = CAISSE ;
                plateau[lSokoban - 1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban - 2][cSokoban] == ESPACE) 
            && plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE){
                plateau[lSokoban - 2][cSokoban] = CAISSE;
                plateau[lSokoban - 1][cSokoban] = SOKOBAN;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban - 2][cSokoban] == CIBLE) 
            && (plateau[lSokoban - 1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban - 2][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban - 1][cSokoban] =SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] =ESPACE ;   
        }
        else if ((plateau[lSokoban - 2][cSokoban] == ESPACE) 
            && (plateau[lSokoban - 1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban - 2][cSokoban] = CAISSE ;
                plateau[lSokoban - 1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = ESPACE ;
        }
        else if (plateau[lSokoban - 2][cSokoban] == CIBLE){
            plateau[lSokoban - 2][cSokoban] = CAISSE_CIBLE ;
            plateau[lSokoban - 1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }    
        else if (plateau[lSokoban - 2][cSokoban] == ESPACE){
            plateau[lSokoban - 2][cSokoban] = CAISSE ;
            plateau[lSokoban - 1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }
        deplacement[*iDeplacement] = DEPLA_H_C;
}

void deplacer_gauche_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        // fait le deplacement avec une caisse pour la gauche
        if ((plateau[lSokoban][cSokoban - 2] == CIBLE) 
            && (plateau[lSokoban][cSokoban - 1] == CAISSE_CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban - 2] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban - 1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban - 2] == CIBLE) 
            && (plateau[lSokoban][cSokoban - 1] == CAISSE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban - 2] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban - 1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban - 2] == ESPACE) 
            && (plateau[lSokoban][cSokoban - 1] == CAISSE_CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban - 2] = CAISSE ;
                plateau[lSokoban][cSokoban - 1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban - 2] == ESPACE) 
            && plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE){
                plateau[lSokoban][cSokoban - 2] = CAISSE;
                plateau[lSokoban][cSokoban - 1] = SOKOBAN;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban-2] == CIBLE) 
            && (plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban-2] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban-1] =SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] =ESPACE ;    
        }
        else if ((plateau[lSokoban][cSokoban - 2] == ESPACE) 
            && (plateau[lSokoban][cSokoban - 1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban - 2] = CAISSE ;
                plateau[lSokoban][cSokoban - 1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = ESPACE ;
        }
        else if (plateau[lSokoban][cSokoban - 2] == CIBLE){
            plateau[lSokoban][cSokoban - 2] = CAISSE_CIBLE ;
            plateau[lSokoban][cSokoban - 1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }    
        else if (plateau[lSokoban][cSokoban - 2] == ESPACE){
            plateau[lSokoban][cSokoban - 2] = CAISSE ;
            plateau[lSokoban][cSokoban - 1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }
        deplacement[*iDeplacement] = DEPLA_G_C;

}

void deplacer_bas_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        // fait le deplacement avec une caisse pour le bas
        if ((plateau[lSokoban+2][cSokoban] == CIBLE) 
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+2][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban+2][cSokoban] == CIBLE) 
            && (plateau[lSokoban+1][cSokoban] == CAISSE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+2][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban+2][cSokoban] == ESPACE) 
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+2][cSokoban] = CAISSE ;
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban+2][cSokoban] == ESPACE) 
            && plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE){
                plateau[lSokoban+2][cSokoban] = CAISSE;
                plateau[lSokoban+1][cSokoban] = SOKOBAN;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban+2][cSokoban] == CIBLE) 
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban+2][cSokoban] = CAISSE_CIBLE ;
                plateau[lSokoban+1][cSokoban] =SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] =ESPACE ;     
        }
        else if ((plateau[lSokoban+2][cSokoban] == ESPACE) 
            && (plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE)){
                plateau[lSokoban+2][cSokoban] = CAISSE ;
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = ESPACE ;
        }
        else if (plateau[lSokoban+2][cSokoban] == CIBLE){
            plateau[lSokoban+2][cSokoban] = CAISSE_CIBLE ;
            plateau[lSokoban+1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }    
        else if (plateau[lSokoban+2][cSokoban] == ESPACE){
            plateau[lSokoban+2][cSokoban] = CAISSE ;
            plateau[lSokoban+1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }
        deplacement[*iDeplacement] = DEPLA_B_C;

}

void deplacer_droite_caisse(int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        // fait le deplacement avec une caisse pour la droite
        if ((plateau[lSokoban][cSokoban+2] == CIBLE) 
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+2] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban+2] == CIBLE) 
            && (plateau[lSokoban][cSokoban+1] == CAISSE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+2] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban+2] == ESPACE) 
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+2] = CAISSE ;
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban+2] == ESPACE) 
            && plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE){
                plateau[lSokoban][cSokoban+2] = CAISSE;
                plateau[lSokoban][cSokoban+1] = SOKOBAN;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban][cSokoban+2] == CIBLE) 
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban+2] = CAISSE_CIBLE ;
                plateau[lSokoban][cSokoban+1] =SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] =ESPACE  ;      
        }
        else if ((plateau[lSokoban][cSokoban+2] == ESPACE) 
            && (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)){
                plateau[lSokoban][cSokoban+2] = CAISSE ;
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE ;
                plateau[lSokoban][cSokoban] = ESPACE ;
        }
        else if (plateau[lSokoban][cSokoban+2] == CIBLE){
            plateau[lSokoban][cSokoban+2] = CAISSE_CIBLE ;
            plateau[lSokoban][cSokoban+1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }    
        else if (plateau[lSokoban][cSokoban+2] == ESPACE){
            plateau[lSokoban][cSokoban+2] = CAISSE ;
            plateau[lSokoban][cSokoban+1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }
        deplacement[*iDeplacement] = DEPLA_D_C;

}



void deplacer_haut (int lSokoban,int cSokoban,t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        //fait le deplecement pour le haut et regarde si il y a une caisse
        if (((plateau[lSokoban-1][cSokoban] == CAISSE)
            ||(plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE))   
            && (plateau[lSokoban-2][cSokoban] != MUR)){
                deplacer_haut_caisse(lSokoban , cSokoban , plateau ,
                     deplacement, &(*iDeplacement));
        }

        else if ((plateau[lSokoban-1][cSokoban] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_H;
        }
        else if ((plateau[lSokoban-1][cSokoban] == CIBLE)){
            plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_H;
        }
        else if ((plateau[lSokoban-1][cSokoban] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_H;
        }
        else if ((plateau[lSokoban-1][cSokoban] == ESPACE)){
            plateau[lSokoban-1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_H;
        }
}

void deplacer_bas (int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        //fait le deplecement pour le bas et regarde si il y a une caisse
        if (((plateau[lSokoban+1][cSokoban] == CAISSE)
            ||(plateau[lSokoban+1][cSokoban] == CAISSE_CIBLE))   
            && (plateau[lSokoban+2][cSokoban] != MUR)){
                deplacer_bas_caisse(lSokoban , cSokoban , plateau,
                     deplacement, &(*iDeplacement));
        }

        else if ((plateau[lSokoban+1][cSokoban] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_B;
        }
        else if ((plateau[lSokoban+1][cSokoban] == CIBLE)){
            plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_B;
        }
        else if ((plateau[lSokoban+1][cSokoban] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_B;
        }
        else if ((plateau[lSokoban+1][cSokoban] == ESPACE)){
            plateau[lSokoban+1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_B;
        }
}

void deplacer_gauche (int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        //fait le deplecement pour la gauche et regarde si il y a une caisse
    if (((plateau[lSokoban][cSokoban-1] == CAISSE)
            ||(plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE))
            && (plateau[lSokoban][cSokoban-2] != MUR)){
                deplacer_gauche_caisse(lSokoban , cSokoban , plateau, 
                    deplacement, &(*iDeplacement));
        }

        else if ((plateau[lSokoban][cSokoban-1] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_G ;
        }
        else if ((plateau[lSokoban][cSokoban-1] == CIBLE)){
            plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_G ;
        }
        else if ((plateau[lSokoban][cSokoban-1] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_G ;
        }
        else if ((plateau[lSokoban][cSokoban-1] == ESPACE)){
            plateau[lSokoban][cSokoban-1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_G ;
        }
        
}

void deplacer_droite (int lSokoban , int cSokoban , t_plateau plateau,
    t_deplacement deplacement ,int *iDeplacement){
        //fait le deplecement pour la droite et regarde si il y a une caisse
        if (((plateau[lSokoban][cSokoban+1] == CAISSE)
        ||(plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE)) 
            && (plateau[lSokoban][cSokoban+2] != MUR)){
                deplacer_droite_caisse(lSokoban , cSokoban , plateau, 
                    deplacement , &(*iDeplacement));

        }

        else if ((plateau[lSokoban][cSokoban+1] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_D;
        }
        else if ((plateau[lSokoban][cSokoban+1] == CIBLE)){
            plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_D;
        }
        else if ((plateau[lSokoban][cSokoban+1] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
                deplacement[*iDeplacement] = DEPLA_D;
        }
        else if ((plateau[lSokoban][cSokoban+1] == ESPACE)){
            plateau[lSokoban][cSokoban+1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
            deplacement[*iDeplacement] = DEPLA_D;
        }
}


void deplacer (char touche ,  t_plateau plateau , t_deplacement deplacement ,
    int *iDeplacement){
        // fait appelle au bonne fonction pour le deplacement 
        // pour la touche donner
        int lSokoban ;
        int cSokoban ;
        int i ;
        int j ;
        // la boucle permet de localiser le sookban
        for (i = 0 ; i < TAILLE ; i++){
            for (j = 0 ; j < TAILLE ; j++){
                if ((plateau[i][j] == SOKOBAN) | (plateau[i][j] == SOKOBAN_CIBLE)){
                    lSokoban = i ;
                    cSokoban = j ;
                }
            }
        }
        if (touche == HAUT){
            deplacer_haut (lSokoban , cSokoban ,  plateau , 
                deplacement, &(*iDeplacement));
        }
        else if (touche == BAS){
            deplacer_bas (lSokoban , cSokoban ,  plateau ,
                 deplacement , &(*iDeplacement));
        }
        else if (touche == DROITE){
            deplacer_droite (lSokoban , cSokoban ,  plateau , 
                deplacement, &(*iDeplacement));
        }
        
        else if (touche == GAUCHE){
            deplacer_gauche (lSokoban , cSokoban ,  plateau , 
                deplacement, &(*iDeplacement));
        }
        (*iDeplacement)++;
}

void affiche_plateau(t_plateau plateau, int zoom){
    // affiche le plateau et applique un zoom si necesaire
    char temp ;
    int   i  ;
    int j ; 
    for ( i=0;i<TAILLE*zoom;i++){
        printf("\n");
        for (j=0;j<TAILLE*zoom;j++){
        temp=plateau[i/zoom][j/zoom];
        if (temp == CAISSE_CIBLE){
            temp=CAISSE;
        }
        else if (temp == SOKOBAN_CIBLE){
            temp=SOKOBAN;
        }
        printf("%c",temp);
        }
    }
}

void charger_partie(t_plateau plateau, char fichier[]){
    // charge une partie
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}
    


void enregistrer_partie(t_plateau plateau, char fichier[]){
    // enregistre la partie
    FILE * f;
    char finDeLigne='\n';

    f = fopen(fichier, "w");
    for (int ligne=0 ; ligne<TAILLE ; ligne++){
        for (int colonne=0 ; colonne<TAILLE ; colonne++){
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}


void enregistrerDeplacements(t_deplacement t, int nb, char fic[]){
    // enregistre les deplacement
    FILE * f;

    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}


int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

