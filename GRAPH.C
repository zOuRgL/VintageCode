


#include <dos.h>
#include <time.h>
#include <conio.h>
#include <bios.h>
#include <alloc.h>
#include <string.h>

#include "graph.h"




 int KeyboardWait( void );







 /* ***************************************************************
     VertScroll: Scrolling vers la droite ou la gauche de N cars.
    ***************************************************************
     Parametres : (int)sens    : Sens du scroll (0=Gauche,1=Droite)
		  (int)nombre  : Nombre de caracteres … scroller
		  (int)couleur : Couleur/Attribut des cases vides
		  (int)colHg   : Ligne de d‚part
		  (int)ligHg   : Colonne de d‚part
		  (int)nbLong  : Longueur 
		  (int)nbLig   : Hauteur
       Retourne : Rien
    *************************************************************** */

 int VertScroll( int sens,
                 int nombre,
                 int couleur,
                 int lig,
                 int col,
                 int nbLong,
                 int nblig )
{
 int i,l,depart,chr,atr;
 char far *adr,*ptr,*ptrDest;
 char *buffer;

 MasqueSouris();

 if ( _GetVideoMode() == 7 )
    adr = (char far *)0xb0000000;
 else
    adr = (char far *)0xb8000000;

 lig--;
 col--;
 for( l=0; l<nblig; l++ ){

    if ( sens == LEFT_SCROLL ){

       if ( col-nombre >= 0 )
          ptr = (char far *)(adr+((lig+l)*160)+((col-nombre)*2));
       else
          ptr = (char far *)(adr+((lig+l)*160));

       for( i=0; i<nbLong*2; i++ )
          *(ptr+i) = *(ptr+(nombre*2)+i);

       _MemWriteChar( lig+l+1, col-nombre+nbLong+1, couleur, ' ' , nombre );

    }else{

       if ( (buffer = malloc( 162 )) == NULL ){
          AfficheSouris();
          return 1;
       }

       ptr = (char far *)(adr+((lig+l)*160)+(col*2));
       for( i=0; i<nbLong*2; i++ )
          *(buffer+i) = *(ptr+i);

       ptr = (char far *)(adr+((lig+l)*160)+((col+nombre)*2));
       for( i=0; i<nbLong*2 && col+nombre+(i/2) < 80; i++ )
          *(ptr+i) = *(buffer+i);

       _MemWriteChar( lig+l+1, col+1, couleur, ' ' , col+nombre>80 ? 80-col : nombre );
       free( buffer );

    }
 }
 AfficheSouris();
 return 0;
}








 /* ***************************************************************
     Scroll: Scrolling vers le haut ou bas de(s) ligne(s) ecran
    ***************************************************************
     Parametres : (int)sens    : Send du scroll (6=Haut,7=Bas)
                                 Constante: UP_SCROLL & DOWN_SCROLL
		  (int)nombre  : Nombre de lignes a scroller
		  (int)couleur : Couleur/Attribut des lignes vides
		  (int)ligHg   : Ligne coin superieur gauche
		  (int)colHg   : Colonne coin superieur gauche
		  (int)ligBd   : Ligne coin inferieur droit
		  (int)colBd   : Colonne coin inferieur droit
       Retourne : Rien
    *************************************************************** */

 void Scroll( int sens ,
	      int nombre ,
	      int couleur ,
	      int ligHg ,
	      int colHg ,
	      int ligBd ,
	      int colBd )
{
 union REGS Register;  /* Variable de registre pour appeler l'interruption */

 MasqueSouris();
 Register.h.ah = sens;                               /* Num‚ro de fonction */
 Register.h.al = nombre;                               /* Nombre de lignes */
 Register.h.bh = couleur;                      /* Couleur des lignes vides */
 Register.h.ch = ligHg-1;               /* Fixer coordonn‚es de la fenˆtre */
 Register.h.cl = colHg-1;                       /* d'‚cran … scroller ou … */
 Register.h.dh = ligBd-1;                                       /* effacer */
 Register.h.dl = colBd-1;
 int86(0x10, &Register, &Register);          /* Appeler interruption 10(h) */
 AfficheSouris();
}








 /* *************************************************************
     _Box: Affiche un rectangle avec attribut
    -------------------------------------------------------------
     Include : GRAPH.H,MEJFUNCT.H
    *************************************************************
     Parametres : (int)lig    : Ligne d'origine
		  (int)col    : Colonne d'origine
		  (int)larg   : Nombre de caracteres de longueur
		  (int)haut   : Nombre de caracteres de largeur
		  (int)color  : Couleur de texte
		  (int)mode   : Type de contour ( 0 … 4 )
                  (int)fill   : Remplissage de la boite ( == 0 )
                                Constantes: FILL_BOX || BORDER_ONLY
                  (int)shadow : Ombre ( == 0 )
                                Constantes:SHADOW || NOSHADOW
     Retourne   : Rien
    ************************************************************** */

 void _Box( int lig ,
            int col ,
            int larg ,
            int haut ,
            int color ,
            int mode ,
            int fill ,
            int shadow )
{
 int i,n;
 char attr[7];

 switch( mode ){
  case 1  : strcpy( attr , "ÖÄ·º½Ó" );
	    break;
  case 2  : strcpy( attr , "ÉÍ»º¼È" );
	    break;
  case 3  : strcpy( attr , "ÕÍ¸³¾Ô" );
	    break;
  case 4  : strcpy( attr , "      " );
	    break;
  default : strcpy( attr , "ÚÄ¿³ÙÀ" );
 }

 _MemWriteChar( lig , col , color , attr[0] , 1 );
 _MemWriteChar( lig , col+1 , color , attr[1] , larg-2 );
 _MemWriteChar( lig , col+larg-1 , color , attr[2] , 1 );

 for( i=0; i<haut-2; i++ ){
    _MemWriteChar( lig+1+i , col , color , attr[3] , 1 );
    if ( !fill )
       _MemWriteChar( lig+1+i , col+1 , color , ' ', larg-2 );
    _MemWriteChar( lig+i+1 , col+larg-1 , color , attr[3] , 1 );
 }
 _MemWriteChar( lig+haut-1 , col , color , attr[5] , 1 );
 _MemWriteChar( lig+haut-1 , col+1 , color , attr[1] , larg-2 );
 _MemWriteChar( lig+haut-1 , col+larg-1 , color , attr[4] , 1 );

 if ( !shadow ){
    n = col+larg;
    if ( lig+haut<26 )
       _MemChangeBack( lig+haut , col+2 , 7 , n<80 ? larg : larg-(n-79) );
    if ( n <= 80 ){
       for( i=1; i<haut+1; i++ ){
          if ( n == 80 )
             _MemChangeBack( lig+i , n , 7 , 1 );
          else
             _MemChangeBack( lig+i , n , 7 , 2 );
       }
    }
 }

} /* Fin Box */









 /* ****************************************************
     _SaveAllScreen: Sauvegarde de l'ecran
                     (la taille du buffer doit ˆtre de
                      4001 octets sinon BBOOOMMM! )   
    ----------------------------------------------------
     Parametres: (char *)BUFFER 
       Retourne: Rien
    **************************************************** */

 void _SaveAllScreen( char *buffer )
{
 int i;
 char far *ptr;
 char far *adr;

 MasqueSouris();
 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 ptr = adr;

 for( i=0; i<4000; i+=2 ){
    *(buffer++) = *(ptr+i);
    *(buffer++) = *(ptr+i+1);
 }
 AfficheSouris();

}






 /* ******************************************************
     _RestoreAllScreen: Restauration de l'ecran
    ------------------------------------------------------
     Parametres: (char *)BUFFER
       Retourne: Rien
    ****************************************************** */

 void _RestoreAllScreen( char *buffer )
{
 int i;
 char far *ptr;
 char far *adr;

 MasqueSouris();
 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 ptr = adr;

 for( i=0; i<4000; i+=2 ){
    *(ptr++) = *(buffer+i);
    *(ptr++) = *(buffer+i+1);
 }
 strcpy( buffer , "\0" );
 AfficheSouris();

}







 /* ******************************************************
     _SavePartScreen: Sauvegarde d'une partie de l'ecran
    ------------------------------------------------------
     Parametres: (int)lig       : Ligne de debut
		 (int)col       : Colonne de debut
		 (int)nblong    : Longueur du bloc
		 (int)nblig     : Hauteur du bloc
		 (char *)buffer : Buffer de sauvegarde
           Nota: Taille du buffer =
                 ((nblong * 2) * nblig) + 1
       Retourne: Rien
    ****************************************************** */

 void _SavePartScreen( int lig ,int col ,int nblong ,int nblig ,char *buffer)
{
 int i,l,c;
 char far *ptr;
 char far *adr;

 MasqueSouris();
 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 --lig;
 --col;

 for( l=0; l<nblig; l++ ){
    ptr = (char far *)(adr+((lig+l)*160)+(col*2));
    for( i=0; i<nblong*2; i+=2 ){
       *(buffer++) = *(ptr+i);
       *(buffer++) = *(ptr+i+1);
    }
 }
 AfficheSouris();

}






 /* ******************************************************
     _RestorePartScreen: Restaure une partie de l'ecran
    ------------------------------------------------------
     Parametres: (int)lig       : Ligne de debut
		 (int)col       : Colonne de debut
		 (int)nblong    : Longueur du bloc
		 (int)nblig     : Hauteur du bloc
		 (char *)buffer : *
       Retourne: Rien
    ****************************************************** */

 void _RestorePartScreen( int lig ,
			  int col ,
			  int nblong ,
			  int nblig ,
			  char *buffer )
{
 int c,i,l;
 char far *ptr;
 char far *adr;

 MasqueSouris();
 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 lig--;
 col--;
 c=0;
 for( l=0; l<nblig; l++ ){
    ptr = (char far *)(adr+((lig+l)*160)+(col*2));
    for( i=0; i<nblong*2; i+=2,c+=2 ){
       *(ptr++) = *(buffer+c);
       *(ptr++) = *(buffer+c+1);
    }
 }
 AfficheSouris();
}








 /* ****************************************************
     _MemChangeBack: Changement de couleur de fond
    ----------------------------------------------------
     Parametres: (int)lig   : Ligne
		 (int)col   : Colonne
		 (int)color : Couleur
		 (int)nbr   : Nombre de caracteres.
       Retourne: Rien
    **************************************************** */

 void _MemChangeBack( int lig ,int col ,int color, int nbr )
{
 int i;
 char far *ptr;
 char far *adr;

 MasqueSouris();

 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 ptr = (char far *)((adr+(lig*160)+(col*2))-162)+1;
 for( i=0; i<nbr*2; i+=2 )
    *(ptr+i) = color;

 AfficheSouris();
}








 /* ****************************************************
     _MemWriteText: Ecris une chaine de caracteres
		    directement dans la m‚moire ‚cran
    ----------------------------------------------------
     Parametres: (int)lig   : Ligne
		 (int)col   : Colonne
		 (int)color : Couleur
		 (char *)txt: Chaine
       Retourne: Rien
    **************************************************** */

 void _MemWriteText( int lig ,int col ,int color, char *txt )
{
 int i;
 char far *ptr;
 char far *adr;

 MasqueSouris();

 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 ptr = (char far *)(adr+(--lig*160)+(--col*2));
 for( i=0; i<strlen(txt); i++ ){
    *(ptr++) = *(txt+i);
    *(ptr++) = color;
 }
 AfficheSouris();
}






 /* *******************************************************
     _MemWriteChar: Ecris un caractere 1 ou plusieurs fois
		    directement dans la m‚moire ‚cran
    -------------------------------------------------------
     Parametres: (int)lig   : Ligne
		 (int)col   : Colonne
		 (int)color : Couleur
		 (int)chr   : Caractere
		 (int)nbr   : Nombre d'ecriture
       Retourne: Rien
    ******************************************************* */

 void _MemWriteChar( int lig ,int col ,int color ,int chr ,int nbr )
{
 int i;
 char far *ptr;
 char far *adr;

 MasqueSouris();

 if ( _GetVideoMode()==7 )
    adr = (char far *)0xB0000000;
 else
    adr = (char far *)0xB8000000;

 ptr = (char far *)(adr+(--lig*160)+(--col*2));
 for( i=0; i<nbr; i++ ){
    *(ptr++) = chr;
    *(ptr++) = color;
 }
 AfficheSouris();
}








 /* *******************************************************
     _GetVideoMode: Retourne le mode vid‚o en cours
    -------------------------------------------------------
     Parametres: Aucun
       Retourne: Numero du mode vid‚o
    ******************************************************* */

 int _GetVideoMode( void )
{
 union REGS Register;

 Register.h.ah = 0x0f;
 int86(0x10, &Register, &Register);
 return( Register.h.al );
}








 /* ****************************************************************
     FormAlert : Boite d'informations.
    ----------------------------------------------------------------
     Inclure   : _SavePartScreen(...)
                 _RestorePartScreen(...)
                 _MemWriteText(...)
                 _MemWriteChar(...)
                 _Box(...)
                 RegardeSouris(...)
                 Var. globale : BoutonSouris,LigSouris,ColSouris
    ----------------------------------------------------------------
     ParamŠtres: (char *)titre       : Titre de la boite
                 (char *)texte       : Texte des diff‚rentes lignes
                                       s‚par‚ par '|'
                 (char *)texteBouton : Texte des diff‚rents boutons
                                       s‚par‚ par '|'
                 (char *)lettreBouton: Caractere de selections de
                                       chaque bouton
                 (int)boutonActif    : Num‚ro du bouton actif
                                       (premier == 1 )
                 (char)numColor      : Palette de couleur ( 0 … 4 )
                                       Constante COLOR1 … COLOR5
       Retourne: (int)Num‚ro du bouton cliqu‚ ou -1 si 'ESC'
                 (numero de bouton entre 1 et nombre bouton)
    **************************************************************** */

 int FormAlert( char *titre ,
                char *texte ,
                char *texteBouton ,
                char *lettreBouton ,
                int  boutonActif,
                char numColor )
{              
 int c,i,j,l,num,nbLigne,nbBouton,longBouton,longLigneBouton,maxCars,lig,col;
 int colBouton[BOUTON_FA],chrBouton[BOUTON_FA],couleur,colBout;
 unsigned key,chr;
 char *tampon,*bouton[BOUTON_FA],*buffer[LIGNE_FA],*screen,lBouton[BOUTON_FA+1];

 if ( (tampon = (char *)malloc( 81 )) == NULL )
    return -1;

 _setcursortype( _NOCURSOR );
 if ( strlen(lettreBouton) > BOUTON_FA )
    return -1;                           /* Trop de lettres de selection */
 strcpy( lBouton , lettreBouton );
 strupr( lBouton );                      /* Chaine de comparaison en Maj */


 /* Transfert du texte des lignes d'affichages dans un tableau <buffer> */

 for( i = nbLigne = num = longBouton = maxCars = 0; i<strlen(texte) && num < 80; i++ ){
    if ( (c = *(texte+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(buffer+nbLigne) = malloc( strlen( tampon ) + 1 )) == NULL ){
          for( j=0; j<i; j++ )
             free( *(buffer+j) );
          free( tampon );
          return -1;
       }
       strcpy( *(buffer+nbLigne++) , tampon );
       if ( maxCars < (c = strlen( tampon )) )
          maxCars = c;
       num = 0;
       if ( nbLigne == LIGNE_FA )
          break;
    }
 }
 if ( i == strlen( texte ) ){
    *(tampon+num) = '\0';
    if ( ( *(buffer+nbLigne) = malloc( strlen( tampon ) + 1 )) == NULL ){
       for( j=0; j<i; j++ )
          free( *(buffer+j) );
       free( tampon );
       return -1;
    }
    strcpy( *(buffer+nbLigne++) , tampon );
    if ( maxCars < (c = strlen( tampon )) )
       maxCars = c;
 }

 /* Transfert du texte des boutons dans un tableau <bouton> */

 for( i = nbBouton = num = longBouton = longLigneBouton = 0; i<strlen(texteBouton) && num < 80; i++ ){
    if ( (c = *(texteBouton+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(bouton+nbBouton) = malloc( strlen( tampon ) + 1 )) == NULL ){
          for( j=0; j<nbLigne; j++ )
             free( *(buffer+j) );
          for( j=0; j<i; j++ )
             free( *(bouton+j) );
          free( tampon );
          return -1;
       }
       strcpy( *(bouton+nbBouton++) , tampon );
       if ( longBouton < (c = strlen( tampon )) )
          longBouton = c;
       longLigneBouton += c;
       num = 0;
       if ( nbBouton == BOUTON_FA )
          break;
    }
 }
 if ( i == strlen( texteBouton ) ){
    *(tampon+num) = '\0';
    if ( ( *(bouton+nbBouton) = malloc( strlen( tampon ) + 1 )) == NULL ){
       for( j=0; j<nbLigne; j++ )
          free( *(buffer+j) );
       for( j=0; j<i; j++ )
          free( *(bouton+j) );
       free( tampon );
       return -1;
    }
    strcpy( *(bouton+nbBouton++) , tampon );
    if ( longBouton < (c = strlen( tampon )) )
       longBouton = c;
    longLigneBouton += c;
 }

 longLigneBouton += 6 + ((nbBouton-1) * 3);
 if ( maxCars < longLigneBouton )
    maxCars = longLigneBouton;
 i = longLigneBouton/2;                         /* i = Variable temporaire */

 if ( maxCars < strlen( titre ) + 8 )
    maxCars = strlen( titre ) + 8;

 lig = 10-(nbLigne/2);
 col = 36-(maxCars/2);

 colBout = col+(maxCars/2)-i;        /* R‚cuperation variable temporaire i */

 if ( (screen = (char *)malloc( (nbLigne+9)*((maxCars+10)*2) ))==NULL ){
    for( j=0; j<nbLigne; j++ )
       free( *(buffer+j) );
    for( j=0; j<nbBouton; j++ )
       free( *(bouton+j) );
    free( tampon );
    return -1;
 }
 _SavePartScreen( lig , col , maxCars+8 , nbLigne+7 , screen );

 _Box( lig ,col ,maxCars+6 ,nbLigne+6 ,Coul_Border_Window[numColor] ,2 ,FILL_BOX ,SHADOW );

 if ( strlen( titre ) )
    _MemWriteText( lig , col+((maxCars+6)/2)-strlen(titre)/2+1 ,
                   Coul_Border_Window[numColor] , titre );

 _MemWriteText( lig , col+2 , Coul_Border_Window[numColor] , "[ ]" );
 _MemWriteChar( lig , col+3 , Coul_IconClose_Window[numColor] , 254 , 1 );

 for( i=0; i<nbLigne; i++ ){
    if ( (l = strlen( *(buffer+i) ))!=maxCars )
       _MemWriteText( lig+2+i, col+3+((maxCars/2)-(l/2)), Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
    else
       _MemWriteText( lig+2+i , col+3 , Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
 }

 if ( boutonActif > nbBouton )
    boutonActif = 1;
 for( i = l = 0; i<nbBouton; i++ ){
    chrBouton[i] = 0;
    for( j = 0; j < strlen( *(bouton+i) ); j++ ){
       if ( *( (*(bouton+i)) + j ) == *(lettreBouton+i) ){
          chrBouton[i] = j;
          break;
       }
    }
    colBouton[i] = 6+l;
    l += strlen( *(bouton+i) )+3;
    couleur = Coul_NormalButton_Window[numColor];
    if ( i == boutonActif-1 )
       couleur = Coul_SelectButton_Window[numColor];
    _MemWriteText( lig+3+nbLigne , colBout+colBouton[i] , couleur ,
                   *(bouton+i) );

    _MemWriteChar( lig+3+nbLigne ,
                   colBout + colBouton[i] + strlen( *(bouton+i) ) ,
                   Coul_NormalText_Window[numColor] , 220 , 1 );
    _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[i] ,
                   Coul_NormalText_Window[numColor] , 223 , strlen( *(bouton+i) ) );

    if ( chrBouton[i] != 0 ){
       couleur = Coul_NormalLetter_Window[numColor];
       if ( i == boutonActif-1 )
          couleur = Coul_SelectLetter_Window[numColor];
       _MemWriteChar(  lig+3+nbLigne , colBout+colBouton[i]+chrBouton[i] ,
                       couleur , *(lettreBouton+i) , 1 );
    }
 }

 do{
    do{
       key = KeyboardWait();

       RegardeSouris();
       if ( (RegardeSouris() & 1) == 1 ){               /* Bouton Gauche */
          if ( LigSouris == lig-1 && ColSouris == col+2 )
             key = 283;                                 /* Esc */
          for( i=0; i<nbBouton; i++ ){
             if ( LigSouris == lig+2+nbLigne &&
                  ( ColSouris+1 >= colBout+colBouton[i] && ColSouris+1 <
                  colBout+colBouton[i]+strlen( *(bouton+i) ) ) )
                     break;
          }
          if ( i < nbBouton ){
             boutonActif = i+1;
             _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_NormalButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             _MemWriteChar(  lig+3+nbLigne ,
                   colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                   Coul_NormalLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );

             /*boutonActif = i+1;*/  /* Mis plus haut */
             _MemWriteChar( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_NormalText_Window[numColor] , ' ' ,
                            strlen( *(bouton+(boutonActif-1)) ) );
             _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[boutonActif-1] ,
                            Coul_NormalText_Window[numColor] , ' ' ,
                            strlen( *(bouton+(boutonActif-1)) ) );
             _MemWriteText( lig+3+nbLigne , colBout+1+colBouton[boutonActif-1] ,
                            Coul_SelectButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             Pause( 3 );
             _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_SelectButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             _MemWriteChar( lig+3+nbLigne ,
                            colBout+colBouton[i]+strlen( *(bouton+i) ) ,
                            Coul_NormalText_Window[numColor] , 220 , 1 );
             _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[i] ,
                            Coul_NormalText_Window[numColor] , 223 ,
                            strlen( *(bouton+i) ) );
             _MemWriteChar( lig+3+nbLigne ,
                   colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                   Coul_SelectLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );
             Pause( 3 );
             key = 7181;                             /* Entr‚e */
          }
       }
       if ( (BoutonSouris & 2) == 2 )                /* Bouton Droit */
	  key = 283;                                 /* Esc */
    }while( !key );
    chr = key & 0x00ff;
    if ( chr > 96 && chr <123 )
       chr -= 32;
    if ( chr == 9 || ( key >> 8 == 77 && !chr ) ){ /* Tab ou FlŠche droite */
       _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
              Coul_NormalButton_Window[numColor] , *(bouton+(boutonActif-1)) );
       _MemWriteChar( lig+3+nbLigne ,
               colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
               Coul_NormalLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );
       if ( boutonActif < nbBouton )
          boutonActif++;
       else
          boutonActif = 1;
       _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                      Coul_SelectButton_Window[numColor] ,
                      *(bouton+(boutonActif-1)) );
       _MemWriteChar( lig+3+nbLigne ,
                      colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                      Coul_SelectLetter_Window[numColor] , *(lettreBouton+boutonActif-1),1);
    }
    if ( !chr )
       chr = 1;
 }while( chr!=13 && chr!=27 && strchr( lBouton , chr )==NULL );
 _RestorePartScreen( lig , col , maxCars+8 , nbLigne+7 , screen );

 free( tampon );
 for( i=0; i<nbLigne; i++ )
    free( *(buffer+i) );
 for( i=0; i<nbBouton; i++ )
    free( *(bouton+i) );
 free( screen );

 if ( chr == 27 )
    return -1;
 if ( chr == 13 )
    return boutonActif;
 for( i=0; i<BOUTON_FA; i++ ){
    if ( *(lBouton+i) == chr )
       return i+1;
 }
 return -1;
}











/* **********************************************************
    STRINSTR : Recherche d'une chaine dans une autre
    STRNINSTR: Idem mais seulement dans les N premiers cars
   ----------------------------------------------------------
    Include : STRING.H
   **********************************************************
    Parametres : (char *)CHAINE = Chaine principale
		 (char *)TXT    = Chaine a rechercher
		 (int)MAX       = Recherche sur N cars
    Retourne   : (int) Position du premier caractere ou -1
   ********************************************************** */

 int strinstr( char *chaine , char *txt )
{
 return(  strninstr( chaine , txt , strlen(chaine)) );
}


 int strninstr( char *chaine , char *txt , int max )
{
 int i,j,len1,len2;

 len1 = max;
 len2 = strlen(txt);

 if ( len1 >= len2 ){
    for( i=0,j=0; i<len1; i++){
       if ( *(chaine+i) == *(txt+j) ){
	  if ( j<len2 )
	     ++j;
       }else{
	  if ( j == len2 )
	     return( i - len2 );
	  else
	     j = 0;
       }
    }
    if ( j == len2 )
       return( i - len2 );
    return( -1 );
 }
 return( -1 );
}






 /* ***************************************************************
     InitSouris: Initialise le gestionnaire souris
    ---------------------------------------------------------------
     Parametres : Aucun
       Retourne : (int)0 si Ok ou 1
    *************************************************************** */

 int InitSouris( void )
{
 union REGS Register;  /* Variable de registres pour appeler l'interruption */

 Register.x.ax = 0;
 int86(0x33, &Register, &Register);
 if ( !Register.x.ax )
    return 1;
 else
    return 0;
}








 /* ***************************************************************
     AfficheSouris: Affiche le curseur de la souris
    ---------------------------------------------------------------
     Parametres : Aucun
       Retourne : Rien
    *************************************************************** */

 void AfficheSouris( void )
{
 union REGS Register;  /* Variable de registres pour appeler l'interruption */

 Register.x.ax = 0x1;
 int86(0x33, &Register, &Register);
}






 /* ***************************************************************
     MasqueSouris: Cache le curseur de la souris
    ---------------------------------------------------------------
     Parametres : Aucun
       Retourne : Rien
    *************************************************************** */

 void MasqueSouris( void )
{
 union REGS Register;  /* Variable de registres pour appeler l'interruption */

 Register.x.ax = 0x2;
 int86(0x33, &Register, &Register);
}





 /* ***************************************************************
     RegardeSouris: Etat souris
    ---------------------------------------------------------------
     Parametres : Aucun
       Retourne : Etat des boutons de la souris.
    *************************************************************** */

 int RegardeSouris( void )
{
 union REGS Register;  /* Variable de registres pour appeler l'interruption */

 Register.x.ax = 0x3;
 int86(0x33, &Register, &Register);
 LigSouris = Register.x.dx >> 3;
 ColSouris = Register.x.cx >> 3;
 BoutonSouris = Register.x.bx;
 return Register.x.bx;                                  /* Bouton Souris */
}






 /* ***************************************************************
     ReplaceSouris: Deplace la souris
    ---------------------------------------------------------------
     Parametres : (int)lig : Ligne d'affichage du curseur
		  (col)col : Colonne    '       '    '
       Retourne : Rien
    *************************************************************** */

 void ReplaceSouris( int lig ,int col )
{
 union REGS Register;  /* Variable de registres pour appeler l'interruption */

 Register.x.ax = 0x04;
 Register.x.dx = lig << 3;
 Register.x.cx = col << 3;
 int86(0x33, &Register, &Register);

}








 /* ***************************************************************
     Pause: *
    ---------------------------------------------------------------
     Parametres : (clock_t)delai : Temps d'attente ( 18 == 1 Sec )
       Retourne : Rien
    *************************************************************** */

 void Pause( clock_t delai )
{
 clock_t but;

 but = delai + clock();
 while( but > clock() );

}





 /* ****************************************************************
     strajt : Ajoute un caracteres 1 ou plusieurs fois dans une
              chaine.
    ----------------------------------------------------------------
     ParamŠtres: (char *)chaine   : Texte … traiter
                 (char)chr        : Caractere … ajouter
                 (int)nbr         : Nombre d'ajout
       Retourne: (int)Num‚ro du bouton cliqu‚ ou -1 si 'ESC'
    **************************************************************** */

 void strajt( char *chaine , char chr , int nbr )
{
 int i,s ;

 for ( i = (s = strlen(chaine)); i<(nbr+s); i++ )
    *(chaine+i) = chr;
 *(chaine+i) = '\0';
}







 /* ****************************************************************
     RadioBox  : Boite de selection par bouton radio.
    ----------------------------------------------------------------
     Inclure   : _SavePartScreen(...)
                 _RestorePartScreen(...)
                 _MemWriteText(...)
                 _MemWriteChar(...)
                 _Box(...)
                 RegardeSouris(...)
                 Var. globale : BoutonSouris,LigSouris,ColSouris
    ----------------------------------------------------------------
     ParamŠtres: (char *)titre       : Titre de la boite
                 (char *)texte       : Texte des diff‚rentes lignes
                                       s‚par‚ par '|'
                 (int)ligneActive    : Premiere == 1
                 (char *)texteBouton : Texte des diff‚rents boutons
                                       s‚par‚ par '|'
                 (char *)lettreBouton: Caractere de selections de
                                       chaque bouton
                 (int *)boutonActif  : Num‚ro du bouton actif
                                       (premier == 1 )
                 (char)numColor      : Palette de couleur ( 0 … 4 )
                                       Constantes COLOR1 … COLOR5
       Retourne: T_ENDOK, T_ENDERR ou T_ENDESC si fin tache
                 avec parametrage dans ligneActive et boutonActif
                 EVNT_KEYBRD … EVNT_MOUSE si sortie provisoire
    **************************************************************** */

 int RadioBox( char *titre ,
               char *texte ,
               int  *ligActive ,
               char *texteBouton ,
               char *lettreBouton ,
               int  *boutActif,
               char numColor )
{
 int c,i,j,l,num,nbLigne,nbBouton,longBouton,longLigneBouton,maxCars,lig,col;
 int colBouton[BOUTON_RB],chrBouton[BOUTON_RB],couleur,colBout;
 int lenChoix,colChoix,ligneActive,boutonActif;
 char *tampon,*buffer[LIGNE_RB],*bouton[BOUTON_RB],*screen,lBouton[BOUTON_RB+1];
 unsigned key,chr;

 if ( (tampon = (char *)malloc( 81 )) == NULL )
    return -1;

 ligneActive = *ligActive;
 boutonActif = *boutActif;

 _setcursortype( _NOCURSOR );
 if ( strlen(lettreBouton) > BOUTON_RB ){
    free( tampon );
    return -1;
 }

 strcpy( lBouton , lettreBouton );
 strupr( lBouton );                      /* Chaine de comparaison en Maj */


 /* Transfert du texte des lignes d'affichages dans un tableau <buffer> */

 for( i = nbLigne = num = longBouton = maxCars = 0; i<strlen(texte) && num < 80; i++ ){
    if ( (c = *(texte+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(buffer+nbLigne) = malloc( 81 )) == NULL ){
          for( j=0; j<i; j++ )
             free( *(buffer+j) );
          free( tampon );
          return -1;
       }
       strcpy( *(buffer+nbLigne++) , tampon );
       if ( maxCars < (c = strlen( tampon )) )
          maxCars = c;
       num = 0;
       if ( nbLigne == LIGNE_RB )
          break;
    }
 }
 if ( i == strlen( texte ) ){
    *(tampon+num) = '\0';
    if ( ( *(buffer+nbLigne) = malloc( 81 )) == NULL ){
          for( j=0; j<i; j++ )
             free( *(buffer+j) );
          free( tampon );
          return -1;
    }
    strcpy( *(buffer+nbLigne++) , tampon );
    if ( maxCars < (c = strlen( tampon )) )
       maxCars = c;
 }

 for( i = 0; i < nbLigne; i++ ){
    c = (maxCars + 4) - strlen( *(buffer+i) );
    for( j=0; j < c; j++ )
       strcat( *(buffer+i) , "." );
    strcat( *(buffer+i) , " ( )" );
 }
 maxCars += 8;
 lenChoix = maxCars;

 /* Transfert du texte des boutons dans un tableau <bouton> */

 for( i = nbBouton = num = longBouton = longLigneBouton = 0; i<strlen(texteBouton) && num < 80; i++ ){
    if ( (c = *(texteBouton+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(bouton+nbBouton) = malloc( strlen( tampon ) + 1 )) == NULL ){
          for( j=0; j < nbLigne; j++ )
             free( *(buffer+j) );
          for( j=0; j<i; j++ )
             free( *(bouton+j) );
          free( tampon );
          return -1;
       }
       strcpy( *(bouton+nbBouton++) , tampon );
       if ( longBouton < (c = strlen( tampon )) )
          longBouton = c;
       longLigneBouton += c;
       num = 0;
       if ( nbBouton == BOUTON_RB )
          break;
    }
 }
 if ( i == strlen( texteBouton ) ){
    *(tampon+num) = '\0';
    if ( ( *(bouton+nbBouton) = malloc( strlen( tampon ) + 1 )) == NULL ){
       for( j=0; j < nbLigne; j++ )
          free( *(buffer+j) );
       for( j=0; j<i; j++ )
          free( *(bouton+j) );
       free( tampon );
       return -1;
    }
    strcpy( *(bouton+nbBouton++) , tampon );
    if ( longBouton < (c = strlen( tampon )) )
       longBouton = c;
    longLigneBouton += c;
 }

 longLigneBouton += 6 + ((nbBouton-1) * 3);
 if ( maxCars < longLigneBouton )
    maxCars = longLigneBouton;
 i = longLigneBouton/2;                         /* i = Variable temporaire */

 if ( maxCars < strlen( titre ) + 8 )
    maxCars = strlen( titre ) + 8;

 lig = 11-(nbLigne/2);
 col = 36-(maxCars/2);

 colBout = col+(maxCars/2)-i;        /* R‚cuperation variable temporaire i */
 colChoix = col+3+((maxCars/2)-(lenChoix/2))+lenChoix-2;

 if ( (screen = (char *)malloc( (nbLigne+9)*((maxCars+10)*2) )) == NULL ){
    for( j=0; j < nbLigne; j++ )
       free( *(buffer+j) );
    for( j=0; j < nbBouton; j++ )
       free( *(bouton+j) );
    free( tampon );
    return -1;
 }
 _SavePartScreen( lig , col , maxCars+8 , nbLigne+7 , screen );

 _Box( lig ,col ,maxCars+6 ,nbLigne+6 ,Coul_Border_Window[numColor] ,2 ,FILL_BOX , SHADOW );

 if ( strlen( titre ) )
    _MemWriteText( lig , col+((maxCars+8)/2)-strlen(titre)/2 ,
                   Coul_Border_Window[numColor] , titre );

 _MemWriteText( lig , col+2 , Coul_Border_Window[numColor], "[ ]" );
 _MemWriteChar( lig , col+3 , Coul_IconClose_Window[numColor] , 254 , 1 );

 for( i=0; i<nbLigne; i++ ){
    if ( (l=strlen( *(buffer+i) ))!=maxCars )
       _MemWriteText( lig+2+i , col+3+((maxCars/2)-(l/2)) , Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
    else
       _MemWriteText( lig+2+i , col+3 , Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
 }
 _MemWriteChar( lig+1+ligneActive, colChoix, Coul_NormalText_Window[numColor], 7, 1 );

 if ( boutonActif > nbBouton )
    boutonActif = 1;
 for( i=l=0; i<nbBouton; i++ ){
    chrBouton[i] = 0;
    for( j=0; j<strlen( *(bouton+i) ); j++ ){
       if ( *(*(bouton+i)+j) == *(lettreBouton+i) ){
          chrBouton[i] = j;
          break;
       }
    }
    colBouton[i] = 6+l;
    l += strlen( *(bouton+i) )+3;
    couleur = Coul_NormalButton_Window[numColor];
    if ( i == boutonActif-1 )
       couleur = Coul_SelectButton_Window[numColor];
    _MemWriteText( lig+3+nbLigne , colBout+colBouton[i] , couleur ,
                   *(bouton+i) );

    _MemWriteChar( lig+3+nbLigne ,
                   colBout+colBouton[i]+strlen( *(bouton+i) ) ,
                   Coul_NormalText_Window[numColor] , 220 , 1 );
    _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[i] ,
                   Coul_NormalText_Window[numColor] , 223 , strlen( *(bouton+i) ) );

    if ( chrBouton[i] != 0 ){
       couleur = Coul_NormalLetter_Window[numColor];
       if ( i == boutonActif-1 )
          couleur = Coul_SelectLetter_Window[numColor];
       _MemWriteChar(  lig+3+nbLigne , colBout+colBouton[i]+chrBouton[i] ,
                       couleur , *(lettreBouton+i) , 1 );
    }
 }

 do{
    do{
       key = KeyboardWait();

       RegardeSouris();
       if ( (RegardeSouris() & 1) == 1 ){               /* Bouton Gauche */
          if ( LigSouris == lig-1 && ColSouris == col+2 )
             key = 283;                                 /* Esc */
          if ( ( LigSouris > lig && LigSouris < lig+nbLigne+1 )
               && ColSouris+1 == colChoix ){
             _MemWriteChar( lig+1+ligneActive , colChoix ,
                            Coul_NormalText_Window[numColor] , ' ' , 1 );
             ligneActive = LigSouris-lig;
             _MemWriteChar( lig+1+ligneActive , colChoix ,
                            Coul_NormalText_Window[numColor] , 7 , 1 );
          }
          for( i=0; i<nbBouton; i++ ){
             if ( LigSouris == lig+2+nbLigne &&
                  ( ColSouris+1 >= colBout+colBouton[i] && ColSouris+1 <
                  colBout+colBouton[i]+strlen( *(bouton+i) ) ) )
                     break;
          }
          if ( i < nbBouton ){
             _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_NormalButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             _MemWriteChar(  lig+3+nbLigne ,
                   colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                   Coul_NormalLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );

             boutonActif = i+1;
             _MemWriteChar( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_NormalText_Window[numColor] , ' ' ,
                            strlen( *(bouton+(boutonActif-1)) ) );
             _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[boutonActif-1] ,
                            Coul_NormalText_Window[numColor] , ' ' ,
                            strlen( *(bouton+(boutonActif-1))) );
             _MemWriteText( lig+3+nbLigne , colBout+1+colBouton[boutonActif-1] ,
                            Coul_SelectButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             Pause( 3 );
             _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_SelectButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             _MemWriteChar( lig+3+nbLigne ,
                            colBout+colBouton[i]+strlen( *(bouton+i) ) ,
                            Coul_NormalText_Window[numColor] , 220 , 1 );
             _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[i] ,
                            Coul_NormalText_Window[numColor] , 223 ,
                            strlen( *(bouton+i)) );
             _MemWriteChar( lig+3+nbLigne ,
                   colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                   Coul_SelectLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );
             Pause( 3 );
             key = 7181;                             /* Entr‚e */
          }
       }
       if ( (BoutonSouris & 2) == 2 )                /* Bouton Droit */
	  key = 283;                                 /* Esc */
    }while( !key );
    chr = key & 0x00ff;
    if ( chr > 96 && chr <123 )
       chr -= 32;

    switch( key >> 8 ){
     case 80 :                                       /* Down */
               _MemWriteChar( lig+1+ligneActive , colChoix ,
                              Coul_NormalText_Window[numColor] , ' ' , 1 );
               if ( ligneActive < nbLigne )
                  ligneActive++;
               else
                  ligneActive = 1;
               _MemWriteChar( lig+1+ligneActive , colChoix ,
                              Coul_NormalText_Window[numColor] , 7 , 1 );
               break;
     case 72 :                                       /* Up */
               _MemWriteChar( lig+1+ligneActive , colChoix ,
                              Coul_NormalText_Window[numColor] , ' ' , 1 );
               if ( ligneActive > 1 )
                  ligneActive--;
               else
                  ligneActive = nbLigne;
               _MemWriteChar( lig+1+ligneActive , colChoix ,
                              Coul_NormalText_Window[numColor] , 7 , 1 );
               break;
    }

    if ( chr == 9 || ( key >> 8 == 77 && !chr ) ){ /* Tab ou FlŠche droite */
       _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
              Coul_NormalButton_Window[numColor] , *(bouton+(boutonActif-1)) );
       _MemWriteChar( lig+3+nbLigne ,
               colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
               Coul_NormalLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );
       if ( boutonActif < nbBouton )
          boutonActif++;
       else
          boutonActif = 1;
       _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                      Coul_SelectButton_Window[numColor] ,
                      *(bouton+(boutonActif-1)) );
       _MemWriteChar( lig+3+nbLigne ,
                      colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                      Coul_SelectLetter_Window[numColor] , *(lettreBouton+boutonActif-1),1);
    }
    if ( !chr )
       chr = 1;
 }while( chr!=13 && chr!=27 && strchr( lBouton , chr )==NULL );
 _RestorePartScreen( lig , col , maxCars+8 , nbLigne+7 , screen );
 free( screen );

 free( tampon );
 for( i=0; i<nbLigne; i++ )
    free( *(buffer+i) );
 for( i=0; i<nbBouton; i++ )
    free( *(bouton+i) );

 *ligActive = ligneActive;
 *boutActif = boutonActif;

 if ( chr == 27 )
    return -1;
 if ( chr != 13 )
    return 0;
 for( i=0; i<BOUTON_RB; i++ ){
    if ( *(lBouton+i) == chr )
       *boutActif = i+1;
 }
 return 0;
}







 /* ****************************************************************
     SelectBox  : Boite de selection par bouton radio.
    ----------------------------------------------------------------
     Inclure   : _SavePartScreen(...)
                 _RestorePartScreen(...)
                 _MemWriteText(...)
                 _MemWriteChar(...)
                 _Box(...)
                 RegardeSouris(...)
                 Var. globale : BoutonSouris,LigSouris,ColSouris
    ----------------------------------------------------------------
     ParamŠtres: (char *)titre       : Titre de la fenetre
                 (char *)texte       : Texte des diff‚rentes lignes
                                       s‚par‚ par '|'
                 (int *)preselect    : Tableau des pr‚-selection
                 (char *)texteBouton : Texte des diff‚rents boutons
                                       s‚par‚ par '|'
                 (char *)lettreBouton: Caractere de selections de
                                       chaque bouton
                 (int *)boutonActif  : Num‚ro du bouton actif
                                       (premier == 1 )
                 (char)numColor      : Numero de palette ( 0 … 4 )
                                       Constante COLOR1 … COLOR5
       Retourne: T_ENDOK, T_ENDERR ou T_ENDESC si fin tache
                 avec parametrage dans 'preselect' et boutonActif
                 EVNT_KEYBRD … EVNT_MOUSE si sortie provisoire
    **************************************************************** */

 int SelectBox( char *titre ,
                char *texte ,
                int  *preselect ,
                char *texteBouton ,
                char *lettreBouton ,
                int  *boutActif,
                char numColor )
{
 int c,i,j,l,num,nbLigne,nbBouton,longBouton,longLigneBouton,maxCars,lig,col;
 int colBouton[BOUTON_SB],chrBouton[BOUTON_SB],couleur,colBout,ligCur;
 char *tampon,*buffer[LIGNE_SB],*bouton[BOUTON_SB],*screen,lBouton[BOUTON_SB+1];
 int lenChoix,colChoix,boutonActif;
 unsigned key,chr;

 boutonActif = *boutActif;
 _setcursortype( _NOCURSOR );
 if ( strlen(lettreBouton) > BOUTON_SB )
    return 1;                      /* Trop de lettres de selection */

 if ( (tampon = (char *)malloc( 81 )) == NULL )
    return -1;

 strcpy( lBouton , lettreBouton );
 strupr( lBouton );                      /* Chaine de comparaison en Maj */


 /* Transfert du texte des lignes d'affichages dans un tableau <buffer> */

 for( i = nbLigne = num = longBouton = maxCars = 0; i<strlen(texte) && num < 80; i++ ){
    if ( (c = *(texte+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(buffer+nbLigne) = malloc( 81 )) == NULL ){
          free( tampon );
          for( j=0; j<i; j++ )
             free( *(buffer+j) );
          return -1;
       }
       strcpy( *(buffer+nbLigne++) , tampon );
       if ( maxCars < (c = strlen( tampon )) )
          maxCars = c;
       num = 0;
       if ( nbLigne == LIGNE_SB )
          break;
    }
 }
 if ( i == strlen( texte ) ){
    *(tampon+num) = '\0';
    if ( ( *(buffer+nbLigne) = malloc( 81 )) == NULL ){
       free( tampon );
       for( j=0; j<i; j++ )
          free( *(buffer+j) );
       return -1;
    }
    strcpy( *(buffer+nbLigne++) , tampon );
    if ( maxCars < (c = strlen( tampon )) )
       maxCars = c;
 }

 for( i = 0; i < nbLigne; i++ ){
    c = (maxCars + 4) - strlen( *(buffer+i) );
    for( j=0; j < c; j++ )
       strcat( *(buffer+i) , "." );
    strcat( *(buffer+i) , " [ ]" );
 }
 maxCars += 8;
 lenChoix = maxCars;

 /* Transfert du texte des boutons dans un tableau <bouton> */

 for( i = nbBouton = num = longBouton = longLigneBouton = 0; i<strlen(texteBouton) && num < 80; i++ ){
    if ( (c = *(texteBouton+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(bouton+nbBouton) = malloc( strlen( tampon ) + 1 )) == NULL ){
          free( tampon );
          for( j=0; j<nbLigne; j++ )
             free( *(buffer+j) );
          for( j=0; j<i; j++ )
             free( *(bouton+j) );
          return -1;
       }
       strcpy( *(bouton+nbBouton++) , tampon );
       if ( longBouton < (c = strlen( tampon )) )
          longBouton = c;
       longLigneBouton += c;
       num = 0;
       if ( nbBouton == BOUTON_RB )
          break;
    }
 }
 if ( i == strlen( texteBouton ) ){
    *(tampon+num) = '\0';
    if ( ( *(bouton+nbBouton) = malloc( strlen( tampon ) + 1 )) == NULL ){
       free( tampon );
       for( j=0; j<nbLigne; j++ )
          free( *(buffer+j) );
       for( j=0; j<i; j++ )
          free( *(bouton+j) );
       return -1;
    }
    strcpy( *(bouton+nbBouton++) , tampon );
    if ( longBouton < (c = strlen( tampon )) )
       longBouton = c;
    longLigneBouton += c;
 }

 longLigneBouton += 6 + ((nbBouton-1) * 3);
 if ( maxCars < longLigneBouton )
    maxCars = longLigneBouton;
 i = longLigneBouton/2;                         /* i = Variable temporaire */

 if ( maxCars < strlen( titre ) + 8 )
    maxCars = strlen( titre ) + 8;

 lig = 11-(nbLigne/2);
 col = 36-(maxCars/2);
 ligCur = 0;

 colBout = col+(maxCars/2)-i;         /* R‚cuperation variable temporaire i */
 colChoix = col+3+((maxCars/2)-(lenChoix/2))+lenChoix-2;

 if ( (screen = (char *)malloc( (nbLigne+9)*((maxCars+10)*2) )) == NULL ){
    free( tampon );
    for( j=0; j<nbLigne; j++ )
       free( *(buffer+j) );
    for( j=0; j<nbBouton; j++ )
       free( *(bouton+j) );
    return -1;
 }
 _SavePartScreen( lig , col , maxCars+8 , nbLigne+7 , screen );

 _Box( lig ,col ,maxCars+6 ,nbLigne+6 ,Coul_Border_Window[numColor] ,2 ,FILL_BOX ,SHADOW );

 if ( strlen( titre ) )
    _MemWriteText( lig , col+((maxCars+8)/2)-strlen(titre)/2 ,
                   Coul_Border_Window[numColor] , titre );

 _MemWriteText( lig , col+2 , Coul_Border_Window[numColor] , "[ ]" );
 _MemWriteChar( lig , col+3 , Coul_IconClose_Window[numColor] , 254 , 1 );

 for( i=0; i<nbLigne; i++ ){
    if ( (l=strlen( *(buffer+i) ))!=maxCars )
       _MemWriteText( lig+2+i , col+3+((maxCars/2)-(l/2)) , Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
    else
       _MemWriteText( lig+2+i , col+3 , Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
 }

 for( i=0; i<nbLigne; i++ ){
    if ( *(preselect+i) )
       _MemWriteChar( lig+2+i , colChoix , Coul_NormalText_Window[numColor] , 'X' , 1 );
 }
 _MemWriteChar( lig+2 , colChoix-1 , Coul_Select_Window[numColor]+128 , 16 , 1 );
 _MemWriteChar( lig+2 , colChoix+1 , Coul_Select_Window[numColor]+128 , 17 , 1 );

 if ( boutonActif > nbBouton )
    boutonActif = 1;
 for( i=l=0; i<nbBouton; i++ ){
    chrBouton[i] = 0;
    for( j=0; j<strlen( *(bouton+i) ); j++ ){
       if ( *(*(bouton+i)+j) == *(lettreBouton+i) ){
          chrBouton[i] = j;
          break;
       }
    }
    colBouton[i] = 6+l;
    l += strlen( *(bouton+i) )+3;
    couleur = Coul_NormalButton_Window[numColor];
    if ( i == boutonActif-1 )
       couleur = Coul_SelectButton_Window[numColor];
    _MemWriteText( lig+3+nbLigne , colBout+colBouton[i] , couleur ,
                   *(bouton+i) );

    _MemWriteChar( lig+3+nbLigne ,
                   colBout+colBouton[i]+strlen( *(bouton+i)) ,
                   Coul_NormalText_Window[numColor] , 220 , 1 );
    _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[i] ,
                   Coul_NormalText_Window[numColor] , 223 , strlen( *(bouton+i)) );

    if ( chrBouton[i] != 0 ){
       couleur = Coul_NormalLetter_Window[numColor];
       if ( i == boutonActif-1 )
          couleur = Coul_SelectLetter_Window[numColor];
       _MemWriteChar(  lig+3+nbLigne , colBout+colBouton[i]+chrBouton[i] ,
                       couleur , *(lettreBouton+i) , 1 );
    }
 }

 do{
    do{
       key = KeyboardWait();

       RegardeSouris();
       if ( (RegardeSouris() & 1) == 1 ){               /* Bouton Gauche */
          if ( LigSouris == lig-1 && ColSouris == col+2 )
             key = 283;                                 /* Esc */
          if ( ( LigSouris > lig && LigSouris < lig+nbLigne+1 )
               && ColSouris+1 == colChoix ){
             _MemWriteChar( lig+2+ligCur ,colChoix-1 ,Coul_Select_Window[numColor] ,'[' , 1 );
             _MemWriteChar( lig+2+ligCur ,colChoix+1 ,Coul_Select_Window[numColor] ,']' , 1 );
             ligCur = LigSouris-lig-1;
             _MemWriteChar( lig+2+ligCur ,colChoix-1 ,Coul_Select_Window[numColor]+128 ,16 ,1 );
             _MemWriteChar( lig+2+ligCur ,colChoix+1 ,Coul_Select_Window[numColor]+128 ,17 ,1 );
             *(preselect+ligCur) = !*(preselect+ligCur);
             if ( *(preselect+ligCur) )
                _MemWriteChar( lig+2+ligCur ,colChoix ,Coul_Select_Window[numColor] ,'X' ,1 );
             else
                _MemWriteChar( lig+2+ligCur ,colChoix ,Coul_Select_Window[numColor] ,' ' ,1 );
             Pause( 5 );
          }
          for( i=0; i<nbBouton; i++ ){
             if ( LigSouris == lig+2+nbLigne &&
                  ( ColSouris+1 >= colBout+colBouton[i] && ColSouris+1 <
                  colBout+colBouton[i]+strlen( *(bouton+i)) ) )
                     break;
          }
          if ( i < nbBouton ){
             _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_NormalButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             _MemWriteChar(  lig+3+nbLigne ,
                   colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                   Coul_NormalLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );

             boutonActif = i+1;
             _MemWriteChar( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_NormalText_Window[numColor] , ' ' ,
                            strlen(*(bouton+(boutonActif-1))) );
             _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[boutonActif-1] ,
                            Coul_NormalText_Window[numColor] , ' ' ,
                            strlen(*(bouton+(boutonActif-1))) );
             _MemWriteText( lig+3+nbLigne , colBout+1+colBouton[boutonActif-1] ,
                            Coul_SelectButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             Pause( 3 );
             _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                            Coul_SelectButton_Window[numColor] ,
                            *(bouton+(boutonActif-1)) );
             _MemWriteChar( lig+3+nbLigne ,
                            colBout+colBouton[i]+strlen( *(bouton+i) ) ,
                            Coul_NormalText_Window[numColor] , 220 , 1 );
             _MemWriteChar( lig+4+nbLigne , colBout+1+colBouton[i] ,
                            Coul_NormalText_Window[numColor] , 223 ,
                            strlen(*(bouton+i)) );
             _MemWriteChar( lig+3+nbLigne ,
                   colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                   Coul_SelectLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );
             Pause( 3 );
             key = 7181;                             /* Entr‚e */
          }
       }
       if ( (BoutonSouris & 2) == 2 )                /* Bouton Droit */
	  key = 283;                                 /* Esc */
    }while( !key );
    chr = key & 0x00ff;
    if ( chr > 96 && chr <123 )
       chr -= 32;

    switch( key >> 8 ){
     case 80 :                                       /* Down */
        _MemWriteChar( lig+2+ligCur ,colChoix-1 ,Coul_Select_Window[numColor] ,'[' , 1 );
        _MemWriteChar( lig+2+ligCur ,colChoix+1 ,Coul_Select_Window[numColor] ,']' , 1 );
        if ( ligCur < nbLigne-1 )
           ligCur++;
        else
           ligCur = 0;
        _MemWriteChar( lig+2+ligCur ,colChoix-1 ,Coul_Select_Window[numColor]+128 ,16 ,1 );
        _MemWriteChar( lig+2+ligCur ,colChoix+1 ,Coul_Select_Window[numColor]+128 ,17 ,1 );
        break;
     case 72 :                                       /* Up */
        _MemWriteChar( lig+2+ligCur ,colChoix-1 ,Coul_Select_Window[numColor] ,'[' , 1 );
        _MemWriteChar( lig+2+ligCur ,colChoix+1 ,Coul_Select_Window[numColor] ,']' , 1 );
        if ( ligCur )
           ligCur--;
        else
           ligCur = nbLigne-1;
        _MemWriteChar( lig+2+ligCur ,colChoix-1 ,Coul_Select_Window[numColor]+128 ,16 ,1 );
        _MemWriteChar( lig+2+ligCur ,colChoix+1 ,Coul_Select_Window[numColor]+128 ,17 ,1 );
        break;
    }

    if ( chr == 32 ){                         /* Selection avec 'ESPACE' */
       *(preselect+ligCur) = !*(preselect+ligCur);
       if ( *(preselect+ligCur) )
          _MemWriteChar( lig+2+ligCur ,colChoix ,Coul_Select_Window[numColor] ,'X' ,1 );
       else
          _MemWriteChar( lig+2+ligCur ,colChoix ,Coul_Select_Window[numColor] ,' ' ,1 );
    }
    if ( chr == 9 || ( key >> 8 == 77 && !chr ) ){ /* Tab ou FlŠche droite */
       _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
              Coul_NormalButton_Window[numColor] , *(bouton+(boutonActif-1)) );
       _MemWriteChar( lig+3+nbLigne ,
               colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
               Coul_NormalLetter_Window[numColor] , *(lettreBouton+boutonActif-1) , 1 );
       if ( boutonActif < nbBouton )
          boutonActif++;
       else
          boutonActif = 1;
       _MemWriteText( lig+3+nbLigne , colBout+colBouton[boutonActif-1] ,
                      Coul_SelectButton_Window[numColor] ,
                      *(bouton+(boutonActif-1)) );
       _MemWriteChar( lig+3+nbLigne ,
                      colBout+colBouton[boutonActif-1]+chrBouton[boutonActif-1] ,
                      Coul_SelectLetter_Window[numColor] , *(lettreBouton+boutonActif-1),1);
    }
    if ( !chr )
       chr = 1;
 }while( chr!=13 && chr!=27 && strchr( lBouton , chr )==NULL );
 _RestorePartScreen( lig , col , maxCars+8 , nbLigne+7 , screen );
 free( screen );

 free( tampon );
 for( i=0; i<nbLigne; i++ )
    free( *(buffer+i) );
 for( i=0; i<nbBouton; i++ )
    free( *(bouton+i) );

 *boutActif = boutonActif;

 if ( chr == 27 )
    return 1;
 if ( chr == 13 )
    return 0;
 for( i=0; i<BOUTON_SB; i++ ){
    if ( *(lBouton+i) == chr )
       return i+1;
 }
 return 0;
}









 /* *************************************************************
     _BoxTitle: Affiche une boite avec un titre et un icone de
                fermeture
    -------------------------------------------------------------
     Include : GRAPH.H,MEJFUNCT.H
    *************************************************************
     Parametres : (char *)titre : Titre de la boite
                  (int)lig      : Ligne d'origine
		  (int)col      : Colonne d'origine
		  (int)larg     : Nombre de caracteres de longueur
		  (int)haut     : Nombre de caracteres de largeur
		  (int)color    : Couleur de texte
                  (int)iconColor: Couleur icone de fermeture
		  (int)mode     : Type de contour ( 0 … 4 )
                  (int)contour  : Remplissage de la boite ( == 0 )
                                  Constantes: FILL_BOX || BORDER_ONLY
                  (int)ombre    : Ombre ( == 0 )
                                  Constantes:SHADOW || NOSHADOW
     Retourne   : Rien
    ************************************************************** */

 void _BoxTitle( char *titre ,
                 int lig ,
                 int col ,
                 int larg ,
                 int haut ,
                 int color ,
                 int iconColor ,
                 int mode ,
                 int contour,
                 int ombre )
{
 int len;

 len = strlen(titre);

 if ( len+12 > larg )
    larg = len+12;

 _Box( lig,col,larg,haut,color,mode,contour,ombre );

 if ( strlen( titre ) )
    _MemWriteText( lig , col+1+(larg/2)-strlen(titre)/2 ,
                   color , titre );

 _MemWriteText( lig , col+2 , color , "[ ]" );
 _MemWriteChar( lig , col+3 , iconColor , 254 , 1 );

} /* Fin _BoxTitle */








 /* **********************************************************************
     AfficheBouton : *
    ----------------------------------------------------------------------
     ParamŠtres : (int)lig         : Ligne d'affichage
                  (int)col         : Colonne d'affichage
                  (int)coulBouton  : Couleur du bouton
                  (int)coulOmbre   : Couleur de l'ombre
                  (char *)texte    : Texte du bouton
                  (int)etat        : Etat ( Normal ou appuy‚ )
                                     NORMAL_BOUTON | PUSH_BOUTON
    ********************************************************************** */

 int AfficheBouton( int lig ,
                    int col ,
                    int coulBouton ,
                    int coulOmbre ,
                    char *texte ,
                    int etat )
{
 if ( etat == NORMAL_BUTTON ){
    _MemWriteText( lig , col , coulBouton , texte );
    _MemWriteChar( lig , col+strlen(texte) , coulOmbre , 220 , 1 );
    _MemWriteChar( lig+1 , col+1 , coulOmbre , 223 , strlen(texte) );
 }else{
    _MemWriteChar( lig , col , coulOmbre , ' ' , 1 );
    _MemWriteChar( lig+1 , col+1 , coulOmbre , ' ' , strlen(texte) );
    _MemWriteText( lig , col+1 , coulBouton , texte );
    Pause( 3 );
    _MemWriteText( lig , col , coulBouton , texte );
    _MemWriteChar( lig , col+strlen(texte) , coulOmbre , 220 , 1 );
    _MemWriteChar( lig+1 , col+1 , coulOmbre , 223 , strlen(texte) );
    Pause( 3 );
 }
 return 0;
}








 /* ****************************************************************
     FormInfos : Boite d'informations sans sauvegarde d'ecran
                 ni attente de saisie pour sortir
    ----------------------------------------------------------------
     ParamŠtres: (char *)titre       : Titre de la boite
                 (char *)texte       : Texte des diff‚rentes lignes
                                       s‚par‚ par '|'
                 (char)numColor      : Palette de couleur ( 0 … 4 )
                                       Constante COLOR1 … COLOR5
       Retourne: FALSE si OK ou TRUE
    **************************************************************** */

 int FormInfo( char *titre ,
               char *texte ,
               char numColor )
{              
 int c,i,j,l,num,nbLigne,maxCars,lig,col,couleur;
 unsigned key,chr;
 char *tampon,*buffer[LIGNE_FA];

 if ( (tampon = (char *)malloc( 81 )) == NULL )
    return TRUE;

 _setcursortype( _NOCURSOR );

 /* Transfert du texte des lignes d'affichages dans un tableau <buffer> */

 for( i = nbLigne = num = maxCars = 0; i<strlen(texte) && num < 80; i++ ){
    if ( (c = *(texte+i)) != '|' )
       *(tampon+num++) = c;
    else{
       *(tampon+num) = '\0';
       if ( ( *(buffer+nbLigne) = malloc( strlen( tampon ) + 1 )) == NULL ){
          for( j=0; j<i; j++ )
             free( *(buffer+j) );
          free( tampon );
          return TRUE;
       }
       strcpy( *(buffer+nbLigne++) , tampon );
       if ( maxCars < (c = strlen( tampon )) )
          maxCars = c;
       num = 0;
       if ( nbLigne == LIGNE_FA )
          break;
    }
 }
 if ( i == strlen( texte ) ){
    *(tampon+num) = '\0';
    if ( ( *(buffer+nbLigne) = malloc( strlen( tampon ) + 1 )) == NULL ){
       for( j=0; j<i; j++ )
          free( *(buffer+j) );
       free( tampon );
       return TRUE;
    }
    strcpy( *(buffer+nbLigne++) , tampon );
    if ( maxCars < (c = strlen( tampon )) )
       maxCars = c;
 }

 if ( maxCars < strlen( titre ) + 8 )
    maxCars = strlen( titre ) + 8;

 lig = 10-(nbLigne/2);
 col = 36-(maxCars/2);

 _Box( lig ,col ,maxCars+6 ,nbLigne+4 ,Coul_Border_Window[numColor] ,2 ,FILL_BOX ,SHADOW );

 if ( strlen( titre ) )
    _MemWriteText( lig , col+((maxCars+6)/2)-strlen(titre)/2+1 ,
                   Coul_Border_Window[numColor] , titre );

 _MemWriteText( lig , col+2 , Coul_Border_Window[numColor] , "[ ]" );
 _MemWriteChar( lig , col+3 , Coul_IconClose_Window[numColor] , 254 , 1 );

 for( i=0; i<nbLigne; i++ ){
    if ( (l = strlen( *(buffer+i) ))!=maxCars )
       _MemWriteText( lig+2+i, col+3+((maxCars/2)-(l/2)), Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
    else
       _MemWriteText( lig+2+i , col+3 , Coul_NormalText_Window[numColor] ,
                      *(buffer+i) );
 }

 free( tampon );
 for( i=0; i<nbLigne; i++ )
    free( *(buffer+i) );

 return FALSE;
}




















