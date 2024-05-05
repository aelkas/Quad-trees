#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
typedef enum { FALSE, TRUE } bool;


typedef struct bloc_image
{   bool quatre; // Determine si diviser en 4. Sinon, les fils sont NULL
    struct bloc_image *hg, *hd, *bg, *bd;
} bloc_image ;

typedef bloc_image *image; // Si NULL, image noire


/**
 *  Produit une nouvelle image blanche. Crée un bloc dans le tas.
 *  @return Image blanche
*/
image Blanc() {
    image nouvelle_image = (image) malloc(sizeof (bloc_image));
    nouvelle_image->quatre = FALSE;
    nouvelle_image->hg = NULL;
    nouvelle_image->hd = NULL;
    nouvelle_image->bg = NULL;
    nouvelle_image->bd = NULL;
    return nouvelle_image;
}


/**
 *  Produit une nouvelle image noire. En pratique renvoie NULL.
 *  @return Image noire
*/
image Noir() {
   return NULL;
}


/**
 * Produit (dans le tas) l'image composée de celles passées en paramètres.
 * On ne fait pas de copie des valeurs pointées par les paramètres.
 * @param i0 sous-image haut-gauche.
 * @param i1 sous-image haut-droite.
 * @param i2 sous-image bas-gauche.
 * @param i3 sous-image bas-droite.
 * @return image composée.
*/
image Compose(image i0, image i1, image i2, image i3) {
   image nouvelle_image = (image) malloc(sizeof (bloc_image));
   nouvelle_image->quatre = TRUE;
   nouvelle_image->hg = i0;
   nouvelle_image->hd = i1;
   nouvelle_image->bg = i2;
   nouvelle_image->bd = i3;

   return nouvelle_image;
}


/**
 * Procédure auxilière pour l'affichage d'images.
 * @param img image à afficher.
 */
void AuxAffichage(image img) {
    if(img == NULL) { printf("X"); }
    else if(!(img->quatre)) { printf("o"); }
    else {
        AuxAffichage(img->hg);
        AuxAffichage(img->hd);
        AuxAffichage(img->bg);
        AuxAffichage(img->bd);
        printf("*");
    }

}

/**
 * Procédure qui affiche une image sous le format suivant:
 * - o pour une image blanche
 * - X pour une image noire
 * - x1x2x3x4* pour une image décomposée, avec x1, x2, x3, x4 les notations 
 * pour les sous images respectivement haut-gauche, haut-droite, bas-gauche, 
 * bas-droite
 * @param img image à afficher.
*/
void Affichage(image img) {
   AuxAffichage(img);
   printf("\n");
}


/**
 * Procédure auxilière pour l'affichage d'images en profondeur.
 * @param img image à afficher
 * @param profondeur argument in; profondeur de l'étape actuelle 
 */
void AuxAffichageProfondeur(image img, int profondeur) {
    if(img == NULL) { printf("X"); }
    else if(!(img->quatre)) { printf("o"); }
    else {
        profondeur ++;
        AuxAffichageProfondeur(img->hg, profondeur);
        AuxAffichageProfondeur(img->hd, profondeur);
        AuxAffichageProfondeur(img->bg, profondeur);
        AuxAffichageProfondeur(img->bd, profondeur);
        printf("*");
    }
    printf("%d ", profondeur);

}

/**
 * Procédure qui affiche une image sous le format suivant:
 * - o pour une image blanche
 * - X pour une image noire
 * - x1x2x3x4* pour une image décomposée, avec x1, x2, x3, x4 les notations 
 * pour les sous images respectivement haut-gauche, haut-droite, bas-gauche, 
 * bas-droite
 * - Après chaque symbole on affiche la profondeur de l'image corréspondante.
 * @param img image à afficher
*/
void AffichageProfondeur(image img) {
   AuxAffichageProfondeur(img, 0);
   printf("\n");
}


typedef struct bloc_lecture {
    image img;
    struct bloc_lecture *suivant;
} bloc_lecture; // Utilisé pour gerer la lecture postfixe des images.

typedef bloc_lecture *pile_lecture;


/**
 * Modifie une pile en lui ajoutant une image
 * pour qu'elle pointe vers le nouveau sommet de la pile.
 * @param pile Pile à modifier
 * @param img Nouvelle image à ajouter
 */
void Empile(image img, pile_lecture *pile) {
    pile_lecture tmp = (pile_lecture) malloc(sizeof (bloc_lecture));
    tmp->img = img;
    tmp->suivant = *pile;
    *pile = tmp;
}

/** 
 * Modifie une pile en retirant et renvoyant le sommet.
 * Ne vérifie pas si la pile est vide.
 * @param pile Pile à modifier
 * @return Valeur au sommet de la pile
*/
image Depile(pile_lecture *pile) {
    image valeur_sommet = (*pile)->img;
    pile_lecture tmp = *pile;
    *pile = (*pile)->suivant;
    free(tmp);
    return valeur_sommet;
}

/** 
 * Vide une pile. Libère la mémoire.
 * @param pile Pile à vider
*/
void VidePile(pile_lecture *pile) {
    while(*pile != NULL) {
        Depile(pile);
    }
}

/**
 * Produit une image à partir de caractères tapés au clavier.
 * @return image representée par la chaîne de caractères introduite
 */
image Lecture() {
    pile_lecture pile = NULL;
    image bd, bg, hd, hg;
    int code;
    do {
        code = getchar();
        switch (code) {
            case 42: // 42 est le code ASCII de *
                bd = Depile(&pile);
                bg = Depile(&pile);
                hd = Depile(&pile);
                hg = Depile(&pile);
                Empile(Compose(hg, hd, bg, bd), &pile);
                break;

            case 88: // 88 est le code ASCII de X
                Empile(Noir(), &pile);
                break;

            case 111: // 111 est le code ASCII de o
                Empile(Blanc(), &pile);
                break;

            case 40: case 41: // 40 et 41 sont les codes ASCCI de ( et )
                break; // Les paranthèses sont autorisés mais n'ont pas d'effet.
            
            case 33: // 33 est le code de ! , qui indique le fin de la lecture.
                break;

            default:
                code = -1;
                break;
        }
    } while(code != 33 && code != -1);
    if(pile == NULL || pile->suivant != NULL || code == -1) {
        printf("Image invalide\n");
        return NULL; 
        // Renvoie l'image noire, mais affiche un message dans le terminal.
        // Une autre option aurait été de créer un macro pour une addresse 
        // se corréspondant à <image invalide>
    }
    return Depile(&pile);
}

/**
 * Vérifie si une image est noire.
 * @param img image à vérifier
 * @return TRUE si l'image est noire, FALSE sinon
 */
bool EstNoire(image img) {
    if(img == NULL) { return TRUE; }
    else {
        return  img->quatre && // Si on n'a pas img->quatre, img est blanche.
                EstNoire(img->hg) &&
                EstNoire(img->hd) &&
                EstNoire(img->bg) &&
                EstNoire(img->bd);
    }
}

/**
 * Vérifie si une image est blanche.
 * @param img image à vérifier
 * @return TRUE si l'image est blanche, FALSE sinon
 */
bool EstBlanche(image img) {
    if(img == NULL) { return FALSE; }
    else {
        return !img->quatre || ( 
                    EstBlanche(img->hg) && 
                    EstBlanche(img->hd) &&
                    EstBlanche(img->bg) &&
                    EstBlanche(img->bd) );
    }
}


/**
 * Procédure qui calcule le pourcentage de sous-images noires d'une image.
 * @param img image dont on va extraire cette donnée
 * @param proportion_noires Recevra à la fin la quantité de sous-images noires
 * @param profondeur Profondeur de la couche qu'on est en train de calculer. 
 * À couches plus profondes, le pourcentage de l'image qui est en train d'être 
 * consideré est de moins en moins important
 */
void AuxPourcentageNoir(image img, float *proportion_noires, int profondeur) {
    if(img == NULL) {
        float denominateur = 1.0; 
        for(int i = 0; i < profondeur; i++) {
            denominateur = denominateur * 4;
        } // Un pixel à profondeur p occupe 1/(4**profondeur) de l'image.

        *proportion_noires += 1.0/denominateur; 
    }
    else {
        if(img->quatre) {
            profondeur++;
            AuxPourcentageNoir(img->hg, proportion_noires, profondeur);
            AuxPourcentageNoir(img->hd, proportion_noires, profondeur);
            AuxPourcentageNoir(img->bg, proportion_noires, profondeur);
            AuxPourcentageNoir(img->bd, proportion_noires, profondeur);
        }
    }
}

/**
 * Calcule le pourcentage d'une image qui est noire.
 * @param img image dont l'air on va calculer
 * @return Proportion de l'image qui est noire. Float entre 0 et 1.
 */
float Aire(image img) {
    float proportion_noires = 0;
    AuxPourcentageNoir(img, &proportion_noires, 0);
    return proportion_noires;
}

/**
 * Construit une image dont les pixels qui sont strictement en dessous
 * de la diagonale bas-gauche/haut-droit sont noirs.
 * @param p Profondeur de l'image (résolution)
 * @return image résultat
 */
image TriangleBD(int p) {
    if(p == 0) { return Blanc(); }
    else {
        p--;
        return Compose(Blanc(), TriangleBD(p), TriangleBD(p), Noir());
    }
}

/**
 * Proc qui arrondit une image a la profondeur p dependant de la concentration en Noir.
 * @param im pointeur vers image
 * @param p Profondeur de l'image
**/
void Arrondit(image* im, int p){
    if(p==0){
        if (Aire(*im)>=0.5){
            (*im)=Noir();
        }
        else{
            (*im)=Blanc();
        }
    }
    else{
        p=p-1;
        Arrondit(&(*im)->bg,p);
        Arrondit(&(*im)->bd,p);
        Arrondit(&(*im)->hg,p);
        Arrondit(&(*im)->hd,p);
    }
}

/**
 * Fonction qui rends TRUE si i1 et i2 representent le meme dessin.
 * @param i1 image 1
 * @param i2 image 2
 * @return TRUE si representent le meme dessin, FALSE sinon
**/
bool meme_image(image i1, image i2) {
    if (!i1){
        return EstNoire(i2);
    }
    else if (!(*i1).quatre){
        return EstBlanche(i2);
    }
    else if(!i2){
        return EstNoire(i1);
    }
    else if(!(*i2).quatre){
        return EstBlanche(i1);
    }
    else{
        return meme_image(i1->hg,i2->hg)&&meme_image(i1->hd,i2->hd)
        &&meme_image(i1->bg,i2->bg)&&meme_image(i1->bd,i2->bd);
    }
}

/**
 * Proc qui transforme deux images en leur intersection et leur union respectivement.
 * @param im1 pointeur vers image 1
 * @param im2 pointeur vers image 2
**/
void InterUnion(image* im1,image* im2){
    if(EstBlanche(*im1)||EstNoire(*im2)){
        //rien
    }
    else if(EstNoire(*im1)||EstBlanche(*im2)){
            image tmp=(*im1);
            (*im1)=(*im2);
            (*im2)=tmp;
    }
    else{
        InterUnion(&(*im1)->bd,&(*im2)->bd);
        InterUnion(&(*im1)->bg,&(*im2)->bg);
        InterUnion(&(*im1)->hd,&(*im2)->hd);
        InterUnion(&(*im1)->hg,&(*im2)->hg);
    }
}

/**
 * Fonction qui verifie si une image est une pixel
 * @param img image
 * @return VRAI ssi image est un pixel
**/
bool estPixel(image img){
    return (img==NULL)||(img->quatre==FALSE);
}
/**
 * Fonction qui verifie si on a un pixel Noir
 * @param img image
 * @return VRAI ssi img est un pixel Noir
**/
bool estPixelN(image img){
    return img==NULL;
}
/**
 * Fonction qui verifie si on a un pixel Blanc
 * @param img image
 * @return VRAI ssi img est un pixel Blanc
**/
bool estPixelB(image img){
    return (img!=NULL)&&(img->quatre==FALSE);
}
/**
 * Fonction qui verifie si une image est un damier
 * @param img image
 * @return VRAI ssi l'image est un damier
**/
bool estDamier(image img){
    return (!estPixel(img))
    &&((estPixelB(img->hg)&&estPixelN(img->hd)&&estPixelN(img->bg)&&estPixelB(img->bd))
    ||(estDamier(img->hg)&&estDamier(img->hd)&&estDamier(img->bg)&&estDamier(img->bd)));
}

/**
 * Procedure (non lineaire) qui compte le nombre de damiers a plusieurs hauteurs 
 * @param img image
 * @param cpt pointeur out compteur
**/
void CompteDamierBis(image img, int * cpt){
    if (estPixel(img)){
        *cpt=0;
    }
    else{
        int cpt1,cpt2,cpt3,cpt4;
        CompteDamierBis(img->hd,&cpt1);
        CompteDamierBis(img->hg,&cpt2);
        CompteDamierBis(img->bd,&cpt3);
        CompteDamierBis(img->bg,&cpt4);
        *cpt=cpt1+cpt2+cpt3+cpt4;
        if(estDamier(img->hg)&&estDamier(img->hd)&&estDamier(img->bg)&&estDamier(img->bd)){
            *cpt++;
        }
    }
}

/**
 * Procedure (lineaire) qui compte le nombre de damiers a plusieurs hauteurs 
 * @param img image
 * @param cpt pointeur inout compteur
 * @param flag pointeur out qui indique si le sous arbre est un damier
**/
void CompteDamierBis2(image img, int * cpt, bool* flag){
    if (estPixel(img)){
        *flag=FALSE;
    }
    else if(estPixelB(img->hg)&&estPixelN(img->hd)&&estPixelN(img->bg)&&estPixelB(img->bd)){
        *cpt++;
        *flag=TRUE;
    }
    else{
        bool f1,f2,f3,f4;
        CompteDamierBis2(img->hd,cpt,&f1);
        CompteDamierBis2(img->hg,cpt,&f2);
        CompteDamierBis2(img->bd,cpt,&f3);
        CompteDamierBis2(img->bg,cpt,&f4);
        if(f1&&f2&&f3&&f4){
            *cpt++;
            *flag=TRUE;
        }
        else{
            *flag=FALSE;
        }
    }
}


int CompteDamier(image img){
    int cpt=0;
    bool flag;
    CompteDamierBis2(img,&cpt,&flag);
    return cpt;
}

// Les deux types suivants sont utiles pour implementer la fonction Compresse()
typedef struct bloc_compression{ 
    image sous_image;
    struct bloc_compression *suivant; 
} bloc_compression;

typedef bloc_compression *pile_compression;

/**
 * Modifie une pile en lui ajoutant un pointeur vers une image
 * @param img image dont on va stocker l'addresse
 * @param pile Pile à modifier
 */
void EmpileC(image *img, pile_compression *pile){
    pile_compression tmp = (pile_compression) malloc(sizeof (bloc_compression));
    tmp->sous_image= *img;
    tmp->suivant = *pile;
    *pile = tmp;
}

/** 
 * Vide une pile. Libère la mémoire.
 * @param pile Pile à vider
*/
void VidePileC(pile_compression *pile) {
    while((*pile) != NULL) {
        pile_compression tmp = *pile;
        (*pile) = (*pile)->suivant; 
        free(tmp);
    }
}

/**
 * Fonction qui verifie deux images sont les memes
 * @param i1 image 1.
 * @param i2 image 2.
 * @return VRAI ssi image 1 et image 2 sont les *memes*.
**/
bool egale(image i1, image i2) {
    if (estPixelB(i1)){
        return estPixelB(i2);
    }
    else if (estPixelN(i1)){
        return estPixelN(i2);
    }
    else if((i2==NULL)||(i2->quatre==FALSE)){
        return FALSE;
    }
    else{
        return egale(i1->hg,i2->hg)&&egale(i1->hd,i2->hd)&&egale(i1->bg,i2->bg)&&egale(i1->bd,i2->bd);
    }
}

/**
 * fonction qui vérifie si on travaille sur un pixel ou pas.
 * Procédure récursive auxilière employée dans la compression d'images en DAG.
 * @param img image en format arbre
 * @param pile pile contenant les images auxquelles on va faire des comparaisons
 */


void AuxCompresse(image *img, pile_compression *pile) {
    pile_compression P = *pile;
    if (*img==NULL){
        //rien
    }
    else{
        image fils[4] = {(*img)->hg, (*img)->hd, (*img)->bg, (*img)->bd};
        bool pile_contient[4] = {FALSE, FALSE, FALSE, FALSE}; // hg, hd, bg, bd

        for(int i = 0; i < 4; i++) {
            while(P != NULL) {
                if(egale((P->sous_image), fils[i])) { 
                    //image tmp = fils[i]; // On gardera en mémoire la première sous-image trouvée.
                    fils[i] = P->sous_image;
                    //free(tmp);
                    pile_contient[i] = TRUE;
                }
                P = P->suivant;
            }
            P= *pile;
        }
        for(int i = 0; i < 4; i++) {
            if(!pile_contient[i]) { EmpileC(&fils[i], pile); }
        }
        for(int i = 0; i < 4; i++) {
                if(!pile_contient[i])
                    AuxCompresse(&fils[i], pile);
        }
    }
}

/**
 * Modifie une image en créant une version DAG de l'arbre qui la réprésente. 
 * @param img image en format arbre qui va être compressée
 */
void Compresse(image *img) {
    pile_compression pile = NULL;
    AuxCompresse(img, &pile);
    VidePileC(&pile);
}

/**
 * Procedure Dilate qui transforme un semi DAG en son arbre original
 * @param img qui est un pointeur vers l'image
 * sous fonction CopieBloc qui rend une copie d'un bloc
 * @param img qui est le bloc a copie
 * fonction d'hachage pour sauvegarder les addresses et y acceder en temps constant.
**/
//fonctions est procedure pour une table d'hachage
typedef struct {
   image addresse;
   int status;
} Data;

typedef struct {
  Data * tab;
  int size;
} HashTable;


HashTable * Init(int size) {
   HashTable * h;
   h = malloc(sizeof(HashTable));
   h->tab = malloc(sizeof(Data)* size);
   h->size = size;
   int i;
   for (i=0;i<size;i++) {
      h->tab[i].status = 0;
   }
   return h;
}
unsigned int HashFunction(image addr, int size){
     uintptr_t addrValue = (uintptr_t)addr;
    unsigned int n=(unsigned int)(addrValue * 1.6180339887);
    return n%size;
}

void insertIntoHashTable(HashTable* hashTable, image addr) {
    unsigned int index = HashFunction(addr,hashTable->size);
    if (hashTable->tab[index].status==0){
        hashTable->tab[index].addresse = addr;
        hashTable->tab[index].status=1;
    }
    else if ( hashTable->tab[index].addresse == addr){
        image intPointer = (image)index;
        insertIntoHashTable(hashTable, intPointer);
    }
    else{
        return ;
    }
}

int isInHashTable(HashTable* hashTable, image addr) {
    unsigned int index = HashFunction(addr,hashTable->size);
    
    if (hashTable->tab[index].status==0) {
        return 0;
    }
    else if (hashTable->tab[index].addresse == addr) {
        return 1;
    }
    else{
        image intPointer = (image)index;
        return isInHashTable(hashTable,intPointer);
    }
}


image CopieBloc(image img){ // Fait une copie, pas profonde, d'une image
    if(img==NULL){
        return NULL;
    }
    else if(!img->quatre){
        return Blanc();
    }
    else{
        image tmp = (image)malloc(sizeof(bloc_image));
        tmp->quatre = TRUE;
        tmp->bd = img->bd;
        tmp->bg = img->bg;
        tmp->hd = img->hd;
        tmp->hg = img->hg;
        return tmp;
    }
}

//on va reutiliser estpixel

void Dilate_helper(image* img, HashTable* hashTable){
    if(estPixel(*img)){
        //rien
    }
    else{
        image fils[4] = {(*img)->hg, (*img)->hd, (*img)->bg, (*img)->bd};
        for(int i=0;i<4;i++){
            if(isInHashTable(hashTable,fils[i])){
                image tmp=CopieBloc(fils[i]);
                fils[i]=tmp;
            }
            else{
                insertIntoHashTable(hashTable,fils[i]);
            }
        }
        Dilate_helper(&(*img)->bd, hashTable);
        Dilate_helper(&(*img)->bg, hashTable);
        Dilate_helper(&(*img)->hg, hashTable);
        Dilate_helper(&(*img)->hd, hashTable);
    }
}

void FreeHashTable(HashTable* hashTable) {
    if (hashTable != NULL) {
        if (hashTable->tab != NULL) {
            free(hashTable->tab);
        }
        free(hashTable);
    }
}
int max(int a,int b){
    if (a>b)
        return a;
    else return b;
}
int CalculProfondeur(image img) {
    if (img == NULL || !(img->quatre)) {
        return 0; // The image itself is a pixel
    } else {
        int count1,count2,count3,count4;
        count1 += CalculProfondeur(img->hg);
        count2 += CalculProfondeur(img->hd);
        count3 += CalculProfondeur(img->bg);
        count4 += CalculProfondeur(img->bd);
        return 1+max(count1,max(count2,max(count3,count4)));
    }
}

void Dilate(image * img){
    int intsize=CalculProfondeur(*img);
    HashTable* hashh= Init(1.2*4*intsize);
    Dilate_helper(img,hashh);
    FreeHashTable(hashh);
}

/**
 * Apartée,
 * ici on a des procédures qui compressent un arbre de sorte que l'image reste identique,
 * mais pas nécessairement l'arbre.
 * 
 * Pour mieux s'expliquer:
 * un arbre d'affichage oooo* est compressé au sense de l'énoncée, mais on peut le remplacer par
 *  o* qui donnera une image identique.
**/

void AuxCompresseImage(image *img, pile_compression *pile) {
    pile_compression P = *pile;
    if (EstNoire(*img)){
        //on simplifie le cas ou l'image est Noire et elle est de la forme TRUE; hg,hd,bg,bd noire.
            (*img)=NULL;
    }
    else if(EstBlanche(*img)){
        //on simplifie le cas ou l'image est blanche et elle est de la forme TRUE; hg,hd,bg,bd blanche.
        bool flag=FALSE;
        while(P!=NULL){
            if(EstBlanche(P->sous_image)){
                *img=P->sous_image;
                flag=!flag;
                P=NULL;
            }
            P=P->suivant;
        }
    }
    else{
        image fils[4] = {(*img)->hg, (*img)->hd, (*img)->bg, (*img)->bd};
        bool pile_contient[4] = {FALSE, FALSE, FALSE, FALSE}; // hg, hd, bg, bd

        for(int i = 0; i < 4; i++) {
            while(P != NULL) {
                if(egale((P->sous_image), fils[i])) { 
                    image tmp = fils[i]; // On gardera en mémoire la première sous-image trouvée.
                    fils[i] = (P->sous_image);
                    free(tmp);
                    pile_contient[i] = TRUE;
                }
                P = P->suivant;
            }
            P=*pile;
        }

        for(int i = 0; i < 4; i++) {
            if(!pile_contient[i]) { EmpileC(&fils[i], pile); }
        }
        for(int i = 0; i < 4; i++) {
            AuxCompresseImage(&fils[i], pile);
        }
    }
}

void CompresseImage(image *img) {
    pile_compression pile = NULL;
    image blan=Blanc();
    EmpileC(&blan,&pile);
    AuxCompresseImage(img, &pile);
    //ici faudrait vider tout sauf blanc
    VidePileC(&pile);
}


/********/
/* Main */
/********/
int main() {
    image img = TriangleBD(3);
    AffichageProfondeur(img);
    printf("%f\n", Aire(img));
    Compresse(&img);
    AffichageProfondeur(img);
    printf("%f\n", Aire(img));
    Dilate(&img);
    AffichageProfondeur(img);
    return 0;
}