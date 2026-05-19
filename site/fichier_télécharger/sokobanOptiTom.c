#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define TAILLE  12
#define TAILLE_DEP 1000 
typedef struct{
    int c_lSokoban;
    int c_cSokoban; 
} t_place;
typedef char t_plateau[TAILLE][TAILLE];
t_plateau plateau;
typedef char t_deplacement[TAILLE_DEP];
t_deplacement deplacement;
t_deplacement opti;
typedef t_place t_placement[TAILLE_DEP];
t_placement placement;
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
const char ESPACE = ' ' ;
const char OUI = 'O';
const char NON = 'N';

void charger_partie(t_plateau plateau, char fichier[]) ;
void enregistrer_partie(t_plateau plateau, char fichier[]) ;
int kbhit() ;
void affiche_plateau(t_plateau plateau) ;
void deplacer (char touche , t_plateau plateau , int *nbCoup, int *supp
    , int *iPlace);
void deplacer_haut (int lSokoban,int cSokoban,t_plateau plateau);
void deplacer_bas (int lSokoban , int cSokoban , t_plateau plateau);
void deplacer_droite (int lSokoban , int cSokoban , t_plateau plateau);
void deplacer_gauche (int lSokoban , int cSokoban , t_plateau plateau);
bool gagne(t_plateau plateau) ;
void deplacer_droite_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void deplacer_bas_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void deplacer_gauche_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void deplacer_haut_caisse(int lSokoban , int cSokoban , t_plateau plateau);
void chargerDeplacements(t_deplacement t, char fichier[], int * nb);
void enregistrerDeplacements(t_deplacement t, int nb, char fic[]);

int main(){
    char fichierJeux[20];
    char fichierDep[20];
    char fichierEnregistre[20];
    char touche;
    char reponse = NON;
    int supp;
    int nbCoup = 0;
    int nb;
    int iOpti = 0;
    int iPlace = 0;
    int temp;
    int tempIPlace = 0;
    int tempIOpti = 0;
    int aSupp = 0;
    
    printf("entrer le nom du fichier (ne pas oublier le .sok a la fin ;) )");
    scanf("%s", fichierJeux) ;
    charger_partie(plateau , fichierJeux) ;
    printf("entrer le nom du fichier (ne pas oublier le .dep a la fin ;) )");
    scanf("%s", fichierDep) ;
    chargerDeplacements( deplacement, fichierDep, &nb);
    for(int i = 0; i < nb; i++){
        touche = deplacement[i];
        deplacer(touche, plateau, &nbCoup, &supp, &iPlace);
        usleep(500000);
        affiche_plateau(plateau);
        if(supp == 0){
            opti[iOpti] = deplacement[i];
            iOpti++;
        }
        
        tempIPlace = iPlace;
        
        for (int j = 0; j <= tempIPlace - 1; j++){
            if ((placement[j].c_lSokoban == placement[iPlace].c_lSokoban) 
            && (placement[j].c_cSokoban == placement[iPlace].c_cSokoban)){
                temp = iOpti - (iPlace - j );
                placement[0].c_lSokoban = placement[iPlace].c_lSokoban;
                placement[0].c_cSokoban = placement[iPlace].c_cSokoban;
                iPlace = 0;
                aSupp = 1 ;
            }
        }
        if ((touche == DEPLA_B_C) || (touche == DEPLA_H_C) ||
             (touche == DEPLA_D_C) || (touche == DEPLA_G_C)){
                placement[0].c_lSokoban = placement[iPlace].c_lSokoban;
                placement[0].c_cSokoban = placement[iPlace].c_cSokoban;
                iPlace = 0;
            }
        tempIOpti = iOpti;
        if (aSupp == 1){
            for (int j = temp; j < tempIOpti; j++){
                opti[j] = ESPACE;
                if (j == temp){
                    iOpti = j;
                }
            }
        }
        aSupp = 0;
        
    }
    if (gagne(plateau) == true){
        printf("la suite de deplacement %s\n",fichierDep);
        printf("est bien solution de %s\n",fichierJeux);
        printf("Elle contient initialement %d caractères.\n",nbCoup);
        printf("Après optimisation elle contient %d caractères.\n",iOpti);
        printf("Souhaitez-vous l’enregistrer (O/N) ?\n");
        scanf(" %c",&reponse);
        if (reponse == OUI){
            printf("entre nom du nouveau fichier\n");
            scanf("%s",fichierEnregistre);
            enregistrerDeplacements(opti, iOpti, fichierEnregistre);
        }
    }
    else {
        printf("la suite de deplacement %s\n",fichierDep);
        printf("n'est pas solution de %s\n",fichierJeux);
    }
    printf("suite fini %s\n",opti);
    return EXIT_SUCCESS ;
}

void enregistrerDeplacements(t_deplacement t, int nb, char fic[]){
    FILE * f;

    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}
void chargerDeplacements(t_deplacement t, char fichier[], int * nb){
    FILE * f;
    char dep;
    *nb = 0;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("FICHIER NON TROUVE\n"); 
    } else {
        fread(&dep, sizeof(char), 1, f);
        if (feof(f)){
            printf("FICHIER VIDE\n");
        } else {
            while (!feof(f)){
                t[*nb] = dep;
                (*nb)++;
                fread(&dep, sizeof(char), 1, f);
            }
        }
    }
    fclose(f);
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
void deplacer_haut_caisse(int lSokoban , int cSokoban , t_plateau plateau){
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
}
void deplacer_gauche_caisse(int lSokoban , int cSokoban , t_plateau plateau){
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

}
void deplacer_bas_caisse(int lSokoban , int cSokoban , t_plateau plateau){
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
     
}
void deplacer_droite_caisse(int lSokoban , int cSokoban , t_plateau plateau){
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

}
void deplacer_haut (int lSokoban,int cSokoban,t_plateau plateau){
        //fait le deplecement pour le haut et regarde si il y a une caisse
        if ((plateau[lSokoban-1][cSokoban] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;
        }
        else if ((plateau[lSokoban-1][cSokoban] == CIBLE)){
            plateau[lSokoban-1][cSokoban] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;
        }
        else if ((plateau[lSokoban-1][cSokoban] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban-1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;

        }
        else if ((plateau[lSokoban-1][cSokoban] == ESPACE)){
            plateau[lSokoban-1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;

        }
}
void deplacer_bas (int lSokoban , int cSokoban , t_plateau plateau){
        //fait le deplecement pour le bas et regarde si il y a une caisse
        if ((plateau[lSokoban+1][cSokoban] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;

        }
        else if ((plateau[lSokoban+1][cSokoban] == CIBLE)){
            plateau[lSokoban+1][cSokoban] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;

        }
        else if ((plateau[lSokoban+1][cSokoban] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban+1][cSokoban] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;

        }
        else if ((plateau[lSokoban+1][cSokoban] == ESPACE)){
            plateau[lSokoban+1][cSokoban] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;

        }
}
void deplacer_gauche (int lSokoban , int cSokoban , t_plateau plateau){
        //fait le deplecement pour la gauche et regarde si il y a une caisse
        if ((plateau[lSokoban][cSokoban-1] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;

        }
        else if ((plateau[lSokoban][cSokoban-1] == CIBLE)){
            plateau[lSokoban][cSokoban-1] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;

        }
        else if ((plateau[lSokoban][cSokoban-1] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban-1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;

        }
        else if ((plateau[lSokoban][cSokoban-1] == ESPACE)){
            plateau[lSokoban][cSokoban-1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;

        }
        
}
void deplacer_droite (int lSokoban , int cSokoban , t_plateau plateau){
        //fait le deplecement pour la droite et regarde si il y a une caisse
        if ((plateau[lSokoban][cSokoban+1] == CIBLE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE;
                plateau[lSokoban][cSokoban] = CIBLE ;
            
        }
        else if ((plateau[lSokoban][cSokoban+1] == CIBLE)){
            plateau[lSokoban][cSokoban+1] = SOKOBAN_CIBLE;
            plateau[lSokoban][cSokoban] = ESPACE ;
      
        }
        else if ((plateau[lSokoban][cSokoban+1] == ESPACE) 
            && (plateau[lSokoban][cSokoban] == SOKOBAN_CIBLE)){
                plateau[lSokoban][cSokoban+1] = SOKOBAN ;
                plateau[lSokoban][cSokoban] = CIBLE ;
              
        }
        else if ((plateau[lSokoban][cSokoban+1] == ESPACE)){
            plateau[lSokoban][cSokoban+1] = SOKOBAN ;
            plateau[lSokoban][cSokoban] = ESPACE ;
            
        }
}
void deplacer (char touche , t_plateau plateau , int *nbCoup, int *supp
    , int *iPlace){
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
                    cSokoban = j ;}}}
        *supp = 0;
        if ((touche == DEPLA_H) && ((plateau[lSokoban-1][cSokoban] == ESPACE) 
        || plateau[lSokoban-1][cSokoban] == CIBLE)){
            deplacer_haut (lSokoban , cSokoban ,  plateau );
            (*nbCoup)++;
            *iPlace = *iPlace + 1;
            placement[*iPlace].c_lSokoban = lSokoban;
            placement[*iPlace].c_cSokoban = cSokoban;}
        else if ((touche == DEPLA_B) && (plateau[lSokoban+1][cSokoban] == ESPACE 
            || plateau[lSokoban+1][cSokoban] == CIBLE)){
            deplacer_bas (lSokoban , cSokoban ,  plateau );
            (*nbCoup)++;
            *iPlace = *iPlace + 1;
            placement[*iPlace].c_lSokoban = lSokoban;
            placement[*iPlace].c_cSokoban = cSokoban;}
        else if ((touche == DEPLA_D) && (plateau[lSokoban][cSokoban+1] == ESPACE 
            || plateau[lSokoban][cSokoban+1] == CIBLE)){
            deplacer_droite (lSokoban , cSokoban ,  plateau);
            (*nbCoup)++;
            *iPlace = *iPlace + 1;
            placement[*iPlace].c_lSokoban = lSokoban;
            placement[*iPlace].c_cSokoban = cSokoban;}
        else if ((touche == DEPLA_G) && (plateau[lSokoban][cSokoban-1] == ESPACE 
            || plateau[lSokoban][cSokoban-1] == CIBLE)){
            deplacer_gauche (lSokoban , cSokoban ,  plateau);
            (*nbCoup)++;
            *iPlace = *iPlace + 1;
            placement[*iPlace].c_lSokoban = lSokoban;
            placement[*iPlace].c_cSokoban = cSokoban;}
        else if ((touche == DEPLA_H_C) && (plateau[lSokoban-2][cSokoban] == ESPACE 
            || plateau[lSokoban-2][cSokoban] == CIBLE)
        && ((plateau[lSokoban-1][cSokoban] == CAISSE) 
            || (plateau[lSokoban-1][cSokoban] == CAISSE_CIBLE))){
                deplacer_haut_caisse (lSokoban , cSokoban ,  plateau );
                (*nbCoup)++;
                *iPlace = *iPlace + 1;
                placement[*iPlace].c_lSokoban = lSokoban;
                placement[*iPlace].c_cSokoban = cSokoban;}
        else if ((touche == DEPLA_B_C) && (plateau[lSokoban+2][cSokoban] == ESPACE 
            || plateau[lSokoban+2][cSokoban] == CIBLE)
        && ((plateau[lSokoban+1][cSokoban] == CAISSE) 
            || (plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE))){
                deplacer_bas_caisse (lSokoban , cSokoban ,  plateau );
                (*nbCoup)++;
                *iPlace = *iPlace + 1;
                placement[*iPlace].c_lSokoban = lSokoban;
                placement[*iPlace].c_cSokoban = cSokoban;
               }
        else if ((touche == DEPLA_D_C) && (plateau[lSokoban][cSokoban+2] == ESPACE 
            || plateau[lSokoban][cSokoban+2] == CIBLE)
        && ((plateau[lSokoban][cSokoban+1] == CAISSE)
            || (plateau[lSokoban][cSokoban+1] == CAISSE_CIBLE))){
                deplacer_droite_caisse (lSokoban , cSokoban ,  plateau);
                (*nbCoup)++;
                *iPlace = *iPlace + 1;
                placement[*iPlace].c_lSokoban = lSokoban;
                placement[*iPlace].c_cSokoban = cSokoban;
                }
        else if ((touche == DEPLA_G_C) && (plateau[lSokoban][cSokoban-2] == ESPACE 
            || plateau[lSokoban][cSokoban-2] == CIBLE)
            && ((plateau[lSokoban][cSokoban-1] == CAISSE) 
            || (plateau[lSokoban][cSokoban-1] == CAISSE_CIBLE))){
                deplacer_gauche_caisse (lSokoban , cSokoban ,  plateau);
                (*nbCoup)++;
                *iPlace = *iPlace + 1;
                placement[*iPlace].c_lSokoban = lSokoban;
                placement[*iPlace].c_cSokoban = cSokoban;
            }
        else {
            *supp=1;}
}
void affiche_plateau(t_plateau plateau){
    // affiche le plateau et applique un zoom si necesaire
    char temp ;
    int   i  ;
    int j ; 
    for ( i=0;i<TAILLE;i++){
        printf("\n");
        for (j=0;j<TAILLE;j++){
        temp=plateau[i][j];
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

