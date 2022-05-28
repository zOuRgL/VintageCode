
 #include <io.h>
 #include <stdlib.h>
 #include <process.h>
 #include <string.h>
 #include <fcntl.h>
 #include <sys\stat.h>
 #include <share.h>
 #include <dos.h>
 #include <bios.h>
 #include <conio.h>
 #include <stdio.h>
 #include <alloc.h>

 #include "graph.h"

 #define LPT1      0
 #define NBVOIE    8


 int Coul_Border_Window[5],         /* Contour des fenetres */
     Coul_NormalText_Window[5],     /* Texte */
     Coul_GetText_Window[5],        /* Couleur des saisies */
     Coul_Second_Window[5],         /* Deuxieme fenetre ( Liste taches... ) */
     Coul_IconCloseSecond_Window[5],/* Icone de fermeture seconde fenetre */
     Coul_TextSecond_Window[5],     /* Texte seconde fenetre */
     Coul_LiftSecond_Window[5],     /* Ascenseur seconde fenetre */
     Coul_SelectTextSecond_Window[5], /* Texte selectionn‚ */
     Coul_Lift_Window[5],           /* Ascenseur fenetre */
     Coul_SelectBar_Window[5],      /* Barre de selection fenetre */
     Coul_IconClose_Window[5],      /* Icone de fermeture */
     Coul_NormalButton_Window[5],   /* Bouton de selection */
     Coul_SelectButton_Window[5],   /* Bouton de selection actif */
     Coul_ShadowButton_Window[5];   /* Ombre des boutons */
     Coul_NormalLetter_Window[5],   /* Lettre des boutons */
     Coul_SelectLetter_Window[5],   /* Lettre de selection des boutons */
     Coul_Select_Window[5];         /* Caractere de positionnement */

 int LigSouris,ColSouris,BoutonSouris;
 int cvState[NBVOIE],CurrentPreset = -1;
 int CodeOn[NBVOIE],CodeOff[NBVOIE];
 int LigFB = 10,ColFB =  3;             // Fenetre principale des boutons M/A
 int LigPr =  2,ColPr =  3;             // Fenetre preselections
 int CurrentDelay = 100,DelayRate = 10; // Temporisateur preselections
 int PosDelay = 7,PrinterState = 0;

 char Tampon[501];                      // Buffer ...




 int KeyboardWait( void )
{
 unsigned key;

 key = bioskey( 1 );
 if ( !key )
    return FALSE;
 key = bioskey( 0 );
}






 int SuppEspFin( char *tampon )
{
 int i,n;

 for( i=0 ,n=-1; i<strlen(tampon); i++ ){
    if ( *(tampon+i) == ' ' && n == -1 )
       n = i;
    else{
       if ( *(tampon+i) != ' ' )
          n = -1;
    }
 }
 if ( n != -1 ){
    *(tampon+n) = '\0';
    return FALSE;
 }
 return TRUE;
}








 int LitFichierConfig( void )
{
 FILE *fp;
 int i,file,voie,codeOn,codeOff;
 int puiss2[] = { 1,2,4,8,16,32,64,128 };
 char tamp[101];

 /* Lecture des paramŠtres du fichier de configuration LIGHT.INI */

 if ( (fp = fopen( "C:\\LIGHT.INI" , "rt" ))==NULL ){
    file = open( "C:\\LIGHT.INI", O_CREAT | O_TRUNC ,  S_IREAD|S_IWRITE );
    strcpy( Tampon, "'Fichier de configuration LIGHT\n\n'Format de config : VOIE <numero 1-8>=<code decimal ON>,<code decimal OFF>\n\n" );
    write( file, Tampon, strlen( Tampon ) );
    for (i=0; i < NBVOIE; i++ ){
       sprintf( Tampon, "VOIE %d=0,0\n", i+1 );
       write( file, Tampon, strlen( Tampon ) );
    }
    close( file );
    file = open( "C:\\LIGHT.CFG", O_CREAT | O_TRUNC ,  S_IREAD|S_IWRITE );
    strcpy( Tampon, "\0" );
    for( i=0; i<NBVOIE; i++ )
       strcat( Tampon, "0" );
    write( file, Tampon, strlen( Tampon ) );
    close( file );
    printf("Erreur lors de l'accŠs aux fichiers de configuration!\n\nLes fichiers LIGHT.INI & LIGHT.CFG ont ‚t‚ cr‚‚s dans la racine de C:\n" );
    return TRUE;
 }

 file = open( "C:\\LIGHT.CFG", O_RDONLY );
 read( file, Tampon, NBVOIE );
 close( file );

 // Remise … jour des sorties
 for( i=0; i<NBVOIE; i++ ){
    cvState[i] = *(Tampon+i) - 48;
    if ( cvState[i] )
       PrinterState += puiss2[i];
 }
 _bios_printer( _PRINTER_WRITE, LPT1, PrinterState );

 do{

    fgets( Tampon , filelength(fileno(fp)) , fp );
    if ( strlen( Tampon ) > 500 )
       abort();
    *(Tampon+strlen(Tampon)-1) = '\0';
    strupr( Tampon );
    if ( *(Tampon) == 39 )   /* ' == Commentaire */
       continue;
    SuppEspFin( Tampon );                     /* EnlŠve les espaces de fin */

    if ( !strncmp( Tampon , "VOIE " , 5 ) ){
       if ( (voie = *(Tampon+5) - 48) > NBVOIE )
          continue;

       strcpy( Tampon, Tampon+7 );
       for( i=0; i < strlen( Tampon ) && *(Tampon+i) != ','; i++ )
          *(tamp+i) = *(Tampon+i);
       strcpy( Tampon, Tampon+i+1 );
       *(tamp+i) = '\0';
       CodeOn[voie-1] = (int)atoi( tamp );
       CodeOff[voie-1] = (int)atoi( Tampon );
    }

 }while( !feof(fp) );

 /*
 clrscr();
 for( i=0; i<NBVOIE; i++ )
    printf("Code on = %d  , off = %d\n", CodeOn[i] , CodeOff[i] );
 getch();
 */

 return FALSE;
}







 int InitialisationPrg( void )
{
 int i,state;
 state = 144;
 state = biosprint( _PRINTER_STATUS, 0, LPT1 );
 if ( !state || (state & 8) == 8 ){
    printf("Erreur lors de l'accŠs … la //\n" );
    return TRUE;
 }


 /* Definition des couleurs de base */
 for( i=0; i<3; i++ ){
    Coul_Border_Window[i]           = 127,
    Coul_NormalText_Window[i]       = 112,
    Coul_GetText_Window[i]          = 116,
    Coul_Second_Window[i]           =  49,
    Coul_IconCloseSecond_Window[i]  =  52,
    Coul_TextSecond_Window[i]       =  48,
    Coul_LiftSecond_Window[i]       =  49,
    Coul_SelectTextSecond_Window[i] =  52,
    Coul_Lift_Window[i]             = 112,
    Coul_SelectBar_Window[i]        =  32,
    Coul_IconClose_Window[i]        = 126,
    Coul_NormalButton_Window[i]     =  43,
    Coul_SelectButton_Window[i]     =  46,
    Coul_ShadowButton_Window[i]     = 112,
    Coul_NormalLetter_Window[i]     =  46,
    Coul_SelectLetter_Window[i]     =  43,
    Coul_Select_Window[i]           = 112;
 }
 Coul_Border_Window[1] = 47,
 Coul_IconClose_Window[1] = 46,
 Coul_NormalText_Window[1] = 47,
 Coul_GetText_Window[1]    = 36,
 Coul_NormalButton_Window[1] = 59,
 Coul_SelectButton_Window[1] = 62,
 Coul_ShadowButton_Window[1] = 32,
 Coul_NormalLetter_Window[1] = 62,
 Coul_SelectLetter_Window[1] = 59;

 Coul_Border_Window[2]       = 15,
 Coul_IconClose_Window[2]    = 14,
 Coul_NormalText_Window[2]   = 7,
 Coul_GetText_Window[2]      = 14,
 Coul_ShadowButton_Window[2] = 14;

 for( i=0; i<NBVOIE; i++ ){
    cvState[i] = 0;
    CodeOn[i] = CodeOff[i] = 0;
 }

 if ( LitFichierConfig() )
    return TRUE;

 if ( !InitSouris() )
    AfficheSouris();
 else{
    printf( "Le driver souris n'est pas install‚!\n" );
    return TRUE;
 }

 return FALSE;
}








 int AfficheEtatSortie( char numero )
{
 if ( !cvState[numero] ){
    _MemWriteText( LigFB+3, ColFB+4+((numero*9)), 114, "    " );  // Vert
    _MemWriteText( LigFB+4, ColFB+4+((numero*9)), 114, "    " );
 }else{
    _MemWriteText( LigFB+3, ColFB+4+((numero*9)), 116, "±±±±" );  // Rouge
    _MemWriteText( LigFB+4, ColFB+4+((numero*9)), 116, "±±±±" );
 }

}







 void ChangeEtatSortie( char numero )
{
 int fp;
 int puiss2[] = { 1,2,4,8,16,32,64,128 };

 cvState[numero] = 1 - cvState[numero];
 AfficheEtatSortie( numero );

 if ( !cvState[numero] )
    PrinterState -= puiss2[numero];
 else
    PrinterState += puiss2[numero];
 _bios_printer( _PRINTER_WRITE, LPT1, PrinterState );

 if ( CurrentPreset == -1 ){
    fp = open( "C:\\LIGHT.CFG", O_RDWR );
    read( fp, Tampon, NBVOIE );
    *(Tampon+numero) = 48+cvState[numero];
    lseek( fp, 0L, SEEK_SET );
    write( fp , Tampon, NBVOIE );
    close( fp );
 }

}






 int AfficheEcran( void )
{
 int i;

 clrscr();

 // Fenetre principale bouton M/A

 _BoxTitle( " Gestion des E/S " , LigFB,ColFB,76,10,
           Coul_Border_Window[COLOR1],Coul_IconClose_Window[COLOR1],2,FILL_BOX,SHADOW );

 for( i=0; i< NBVOIE; i++ ){

    _MemWriteText( LigFB+2, ColFB+3+((i*9)), Coul_Border_Window[COLOR1], "ÚÄÄÄÄ¿" );
    _MemWriteText( LigFB+3, ColFB+3+((i*9)), Coul_Border_Window[COLOR1], "³    ³" );
    _MemWriteText( LigFB+4, ColFB+3+((i*9)), Coul_Border_Window[COLOR1], "³    ³" );
    _MemWriteText( LigFB+5, ColFB+3+((i*9)), Coul_Border_Window[COLOR1], "ÀÄÄÄÄÙ" );

    _MemWriteChar( LigFB+5, ColFB+9+((i*9)), Coul_Border_Window[COLOR1], 49+i, 1 );

    AfficheBouton( LigFB+7,ColFB+3+((i*9)),Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1]," Push ",NORMAL_BUTTON );
    AfficheEtatSortie( (char)i );
 }


 // Fenetre presel.

 _BoxTitle( " Preselections " , LigPr,ColPr,76,5,
           Coul_Border_Window[COLOR1],Coul_IconClose_Window[COLOR1],2,FILL_BOX,SHADOW );

 for( i=0; i<9; i++ ){
    sprintf( Tampon, " %d ", i+1 );
    AfficheBouton( LigPr+2,ColPr+3+((i*6)),Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1],Tampon,NORMAL_BUTTON );
 }
 _MemWriteText( LigPr+1, ColPr+58, Coul_NormalText_Window[COLOR1], "    Vitesse    " );
 _MemWriteText( LigPr+2, ColPr+58, Coul_NormalText_Window[COLOR1], "ÃÄÄÄÄÄÄþÄÄÄÄÄÄ´" );

 // Bas de page

 _BoxTitle( " Informations " , 23,3,76,3,
           Coul_Border_Window[COLOR1],Coul_IconClose_Window[COLOR1],2,FILL_BOX,SHADOW );
 //_MemWriteChar( 25,1,Coul_NormalText_Window[COLOR1], ' ', 80 );// ligne infos

 return FALSE;
}






 void AfficheBoutonFinPreset( void )
{

 _MemWriteChar( LigFB+7,ColFB+1,Coul_NormalText_Window[COLOR1], ' ', 72 );
 _MemWriteChar( LigFB+8,ColFB+1,Coul_NormalText_Window[COLOR1], ' ', 72 );

 AfficheBouton( LigFB+7,ColFB+27,Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1]," Retour mode manuel ",NORMAL_BUTTON );

}





 void PresetSpeed( void )
{
 char pos,newPos,ecart;

 RegardeSouris();

 ColSouris++;
 LigSouris++;

 if ( BoutonSouris == 1 && LigSouris == LigPr+2 && ColSouris == ColPr+58+PosDelay ){

    pos = PosDelay;
    do{
       RegardeSouris();

       ColSouris++;
       LigSouris++;

      if ( ColSouris >= ColPr+59 && ColSouris <= ColPr+71 ){
         newPos = ColSouris - (ColPr+58);
         if ( newPos != pos ){
            _MemWriteChar( LigPr+2, ColPr+58+pos, Coul_NormalText_Window[COLOR1], 'Ä', 1 );
            _MemWriteChar( LigPr+2, ColPr+58+newPos, Coul_NormalText_Window[COLOR1], 'þ', 1 );
         }
         pos = newPos;
      }

    }while( BoutonSouris == 1 );

    if ( PosDelay < newPos )
       CurrentDelay -= (newPos - PosDelay) * DelayRate;
    else
       CurrentDelay += (PosDelay - newPos) * DelayRate;
    PosDelay = newPos;

 }
}





 int VerifieFinPreset( void )
{
 int i;

 RegardeSouris();
 LigSouris++;
 ColSouris++;

 if ( BoutonSouris == 1 && LigSouris == LigFB+7 &&
      (ColSouris >= ColFB+27 && ColSouris <= ColFB+46) ){
    AfficheBouton( LigFB+7,ColFB+27,Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1]," Retour mode manuel ",PUSH_BUTTON );
    _MemWriteChar( LigFB+7,ColFB+1,Coul_NormalText_Window[COLOR1], ' ', 72 );
    _MemWriteChar( LigFB+8,ColFB+1,Coul_NormalText_Window[COLOR1], ' ', 72 );
    for( i=0; i< NBVOIE; i++ )
       AfficheBouton( LigFB+7,ColFB+3+((i*9)),Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1]," Push ",NORMAL_BUTTON );
    delay( 500 );
    return TRUE;
 }

 return FALSE;
}





 void Preset1( void )
{
 int i,next;

 next = 0;
 do{
    if ( kbhit() && getch() == 27 )      /* Hein? ESC */
       break;
    if ( VerifieFinPreset() )
       break;

    PresetSpeed();

    ChangeEtatSortie( next );

    delay( CurrentDelay );

    ChangeEtatSortie( next++ );
    if ( next == NBVOIE )
       next = FALSE;

 }while( 1 );

 AfficheBouton( LigPr+2,ColPr+3+((CurrentPreset*6)),Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1],Tampon,NORMAL_BUTTON );
 for( i = 0; i < NBVOIE; i++ ){
    cvState[i] = FALSE;
    AfficheEtatSortie( i );
 }

}





 int main( void )
{
 int i,col;
 int oldState[NBVOIE];
 int puiss2[] = { 1,2,4,8,16,32,64,128 };

 if ( InitialisationPrg() )
    return TRUE;

 _setcursortype( _NOCURSOR );
 AfficheEcran();

 do{

   if ( kbhit() && getch() == 27 )      /* Hein? ESC */
      break;

   RegardeSouris();

   LigSouris++;
   ColSouris++;

   if ( BoutonSouris == 1 ){

      // Bouton de M/A
      if ( LigSouris == LigFB+7 ){
         for( i = 0; i < NBVOIE; i++ ){
            col = 6+((i*9));
            if ( ColSouris >= col && ColSouris < col+6 ){
               AfficheBouton( LigFB+7,6+((i*9)),Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1]," Push ",PUSH_BUTTON );
               ChangeEtatSortie( i );
               break;
            }
         } // Fin For
      } // Fin if FB+7

      // Preselections
      if ( LigSouris == LigPr+2 ){
         for( i=0; i<9; i++ ){
            col = ColPr+3+((i*6));
            if ( ColSouris >= col && ColSouris < col+3 ){

               if ( CurrentPreset != -1 ){
                  sprintf( Tampon, " %d ", CurrentPreset+1 );
                  AfficheBouton( LigPr+2,ColPr+3+((CurrentPreset*6)),Coul_NormalButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1],Tampon,NORMAL_BUTTON );
               }

               CurrentPreset = i;
               sprintf( Tampon, " %d ", i+1 );
               AfficheBouton( LigPr+2,ColPr+3+((i*6)),Coul_SelectButton_Window[COLOR1],Coul_ShadowButton_Window[COLOR1],Tampon,PUSH_BUTTON );

               // Sauvegarde de l'etat des sorties avant preset.
               for( i = 0; i < NBVOIE; i++ ){
                  oldState[i] = cvState[i];
                  cvState[i] = FALSE;
                  AfficheEtatSortie( i );
               }
               _bios_printer( _PRINTER_WRITE, LPT1, PrinterState = 0 );

               switch( CurrentPreset ){
                  case 0 : AfficheBoutonFinPreset();   Preset1();   break;
               }
               // Restauration etat initial des sorties
               for( i=0; i<NBVOIE; i++ ){
                  cvState[i] = oldState[i];
                  AfficheEtatSortie( i );
                  if ( cvState[i] )
                     PrinterState += puiss2[i];
               }
               _bios_printer( _PRINTER_WRITE, LPT1, PrinterState );
               break;
            }
         }
      }

   } // If boutonSouris

 }while( 1 );

 _setcursortype( _NORMALCURSOR );
 clrscr();

 return FALSE;
}






