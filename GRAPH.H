


 #include <time.h>

 #define TRUE           1
 #define FALSE          0

 #define NORMAL_BUTTON  0                     /* Etat dans AfficheBouton */
 #define PUSH_BUTTON    1

 #define UP_SCROLL      6                     /* Fonction Scroll */
 #define DOWN_SCROLL    7
 #define LEFT_SCROLL    0
 #define RIGHT_SCROLL   1

 #define COLOR1         0
 #define COLOR2         1
 #define COLOR3         2
 #define COLOR4         3
 #define COLOR5         4

 #define BORDER_ONLY    1                     /* type de contour pour _Box */
 #define FILL_BOX       0
 #define SHADOW         0
 #define NOSHADOW       1



 /* FormAlert */

 #define LIGNE_FA        15     /* Nombre maximum de lignes de texte */
 #define BOUTON_FA       10     /* Nombre maximum de bouton selectionnable */


 /* RadioBox */

 #define LIGNE_RB        15     /* Nombre maximum de lignes de texte */
 #define BOUTON_RB       10     /* Nombre maximum de bouton selectionnable */


 /* SelectBox */

 #define LIGNE_SB        15     /* Nombre maximum de lignes de texte */
 #define BOUTON_SB       10     /* Nombre maximum de bouton selectionnable */



 extern int BoutonSouris,ColSouris,LigSouris;


 extern int Coul_Border_Window[],
            Coul_NormalText_Window[],
            Coul_GetText_Window[],
            Coul_Second_Window[],
            Coul_IconCloseSecond_Window[],
            Coul_TextSecond_Window[],
            Coul_LiftSecond_Window[],      
            Coul_SelectTextSecond_Window[],
            Coul_Lift_Window[],      
            Coul_SelectBar_Window[], 
            Coul_IconClose_Window[],
            Coul_NormalButton_Window[],
            Coul_SelectButton_Window[],
            Coul_ShadowButton_Window[],
            Coul_NormalLetter_Window[],
            Coul_SelectLetter_Window[],
            Coul_Select_Window[];


 /* Prototype de fonction */


 int VertScroll( int sens,
		 int nombre,
		 int couleur,
		 int lig,
		 int col,
		 int nbLong,
		 int nblig );
 void Scroll( int sens ,
	      int nombre ,
	      int couleur ,
	      int colHg ,
	      int ligHg ,
	      int colBd ,
	      int ligBd );
 void _Box( int lig ,
	    int col ,
	    int larg ,
	    int haut ,
	    int color ,
	    int mode ,
	    int fill ,
	    int shadow );
 void _SaveAllScreen( char *buffer );
 void _RestoreAllScreen( char *buffer );
 void _SavePartScreen( int lig ,int col ,int nblong ,int nblig ,char *buffer);
 void _RestorePartScreen( int lig ,
			  int col ,
			  int nblong ,
			  int nblig ,
			  char *buffer );
 void _MemChangeBack( int lig ,int col ,int color, int nbr );
 void _MemWriteText( int lig ,int col ,int color, char *txt );
 void _MemWriteChar( int lig ,int col ,int color ,int chr ,int nbr );
 int _GetVideoMode( void );
 int _MemBoxCenter( char *txt , int color , int sec );
 int FormAlert( char *titre ,
		char *texte ,
		char *texteBouton ,
		char *lettreBouton ,
		int  boutonActif,
		char numColor );
 int AutoMenu( int lig,
	       int col,
	       char *titre ,
	       char **choix,
	       int nbLigne,
	       char *lettre,
	       int *ligneActive,
	       int sauveEcran,
	       int numColor );
 int strinstr( char *chaine , char *txt );
 int strninstr( char *chaine , char *txt , int max );
 int InitSouris( void );
 void AfficheSouris( void );
 void MasqueSouris( void );
 int RegardeSouris( void );
 void ReplaceSouris( int lig ,int col );
 void Pause( clock_t delai );
 void strajt( char *chaine , char chr , int nbr );
 int RadioBox( char *titre ,
	       char *texte ,
	       int  *ligActive ,
	       char *texteBouton ,
	       char *lettreBouton ,
	       int  *boutActif,
	       char numColor );
 int SelectBox( char *titre ,
		char *texte ,
		int  *preselect ,
		char *texteBouton ,
		char *lettreBouton ,
		int  *boutonActif,
		char numColor );
 void _BoxTitle( char *titre ,
		 int lig ,
		 int col ,
		 int larg ,
		 int haut ,
		 int color ,
		 int iconColor ,
		 int mode ,
		 int contour ,
		 int ombre );
 int AfficheBouton( int lig ,
		    int col ,
		    int coulBouton ,
		    int coulOmbre ,
		    char *texte ,
		    int etat );
