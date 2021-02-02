
/*
 * Programme Arduino Nano pour le kit KAR-Dents.
 * NIVEAU MOYEN : LE CODE COMPORTE DES TROUS ! REMPLIS LES EN SUIVANT LES INSTRUCTIONS ET TRANSFÈRE TOI MEME TON PROGRAMME VERS LA CARTE
 * 
 * Pour récupérer les librairies utilisées par ce programme (par la commande #include), téléchargez-les en allant dans Croquis -> Inclure une bibliothèque -> Gérer les bibliothèques, écrivez le nom de la librairie souhaitée, cliquez dessus puis sur "Installer"
 * 
 * Pour téléverser le programme sur l'Arduino fournie, installez d'abord le pilote en suivant ce lien : https://sparks.gogo.co.nz/assets/_site_/downloads/CH34x_Install_Windows_v3_4.zip. Décompressez l'archive et lancez le fichier exécutable.
 * Ensuite, dans "Outils", sélectionnez Arduino Nano comme type de carte, ATMega328p(old bootloader) comme processeur et le port COM correspondant à votre carte. Vous pourrez alors téléverser le programme sur votre carte.
 * 
 * Des commentaires sont disponible au fil du programme afin d'expliquer le but des différentes fonctions.
 * 
 * Code rédigé par Kardeur.io
 * @author : Hugo Leurent, hugo@kardeur.io
 * 
 */

#include <TM1637Display.h> //TM1637Display sert à ajouter les fonctions de contrôle de l'afficheur 4*7 segments
#include <avr/sleep.h> //Cette bibliothèque gère le mode veille pour consommer le moins possible lorsque l'Arduino n'est pas utilisé
#include <avr/power.h> //Celle-ci gère quelles parties de l'Arduino sont alimentées
#include <TimerOne.h> //TimerOne ajoute des fonctions de contrôle de l'un des minuteurs de l'Arduino
#include <U8g2lib.h> //Librairie de gestion de l'écran OLED. Documentation disponible sur https://github.com/olikraus/u8g2/wiki/u8g2reference


/* ########## REMPLIS TOI MEME LES TEMPS DE DECOMPTE ET LES PINS CORRESPONDANT A CHAQUE COMPOSANT. REFERE TOI AU TUTORIEL POUR CONNAITRE LE SCHEMA DE BRANCHEMENT, RENSEIGNE TOI SUR LA FONCTION #DEFINE ET COMPARE AVEC LE TUTO FACILE SI CA NE FONCTIONNE PAS ######### */

#define TEMPS_DE_BROSSAGE ?? // Temps (en secondes) de brossage de dents. Vous pouvez modifier sa valeur ici en fonction de votre propre niveau d'hygiène ;)
#define TEMPS_VEILLE ?? //Temps (en secondes) avant la mise en veille de l'appareil lorsqu'aucune action n'est détectée. Vous pouvez modifier sa valeur ici

//Broches correspondant à chaque composant branché à l'Arduino
#define HORLOGE_AFFICHEUR ??
#define DONNEES_AFFICHEUR ??

#define BUZZER ??

#define BOUTON ??
#define CAPTEUR_MOUVEMENT ??

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //Initialise l'écran OLED avec le driver SH1106 (il en existe beaucoup d'autres, qui peuvent tous être géré par la librairie u8g2)

uint32_t decalage_OLED = 0; //Variable qui gère le déplacement des dessins affichés à l'écran

// Pixels permettant l'affichage de l'image de brosse à dents sur l'écran
uint8_t brosse_a_dents[370] = {
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111110,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11111011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111110,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01101111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000
 };

// Pixels permettant l'affichage de l'image de dentifrice sur l'écran
uint8_t dentifrice[40] = {
0b00000000,0b00000000,0b00001111,0b11000000,0b00000000,0b00000000,0b00111111,0b11110000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000001,0b11111100,0b00000000,0b00000000,0b00000011,0b11110000,0b00000000,0b00000000,0b00000111,0b11100000,0b00000000,0b00000000,0b00001111,0b11000000,0b00000000,0b00000000,0b01111111,0b10000000,0b00000000,0b00000011,0b11111111,0b00000000,0b00000000,0b00000001,0b11111100,0b00000000,0b00000000
 };
 
TM1637Display afficheur(HORLOGE_AFFICHEUR, DONNEES_AFFICHEUR);
unsigned int minuteur = TEMPS_DE_BROSSAGE; //Variable représentant le minuteur, qui se décrémentera à chaque seconde une fois lancé
unsigned int mise_en_veille = TEMPS_VEILLE; //Variable qui se décrémentera à chaque seconde si aucune action n'est détectée
bool decompte_minuteur = false; //Indique si le decompte doit être lancé ou non

const uint8_t afficheur_eteint[] = {0x00, 0x00, 0x00, 0x00}; 

//Variables utilisées pour empêcher le rebond lors de l'appui du bouton (sans cela, il se peut que le bouton envoie plusieurs fois l'information qu'il a été pressé alors qu'un seul appui a été effectué)
unsigned long dernierAppuiBouton = 0;
unsigned long DelaiEntreDeuxAppuis = 200; //Correspond à 200ms


/*---------------------------- FONCTIONS D'AFFICHAGE OLED -----------------------------------------*/

/* Initialise les paramètres d'affichage sur l'écran OLED (police, taille, ...) */
void initialisation_OLED()
{
  u8g2.begin();
  u8g2.setFont( ?? ); /* ########## CHOISIS TOI MEME LA POLICE ET LA TAILLE PARMI TOUT CE QUI EST DISPONIBLE : https://github.com/olikraus/u8g2/wiki/fntlistall ########## */
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosCenter();
  u8g2.setFontDirection(0);
}

/* Fonction d'animation de la brosse à dents pendant le brossage */
void animation_brosse_a_dents(int decalage)
{
  u8g2.drawBitmap(20,20,10,37,brosse_a_dents);
  u8g2.drawBitmap(64,-9+decalage,4,10,dentifrice);
  /* ########## UNE IMAGE FIXE C'EST SYMPA, MAIS ESSAIE PLUTOT DE FAIRE TA PROPRE ANIMATION EN JOUANT SUR LE DECALAGE ET LES DIFFERENTES FONCTIONS PROPOSEES PAR U8G2 ########## */
}

/* Fonction d'affichage sur l'écran. Appelle des fonctions de la librairie u8g2 ou animation_brosse_a_dents. Le paramètre decalage est optionnel et vaut 0 par défaut */
void afficheur_OLED(uint8_t selecteur, uint32_t decalage=0)
{
  
  /* ########## 
  PERSONNALISE TES PHRASES A AFFICHER EN REMPLACANT OU EN AJOUTANT DES ELEMENTS. ATTENTION, SI IL Y A PLUSIEURS ELEMENTS, TU DOIS CREER UN TABLEAU ET UNE VARIABLE NB_PHRASE_... POUR CONNAITRE SA TAILLE########## */
  const char* phrase_intro[] = ?? 
  const char* phrase_brosse = ?? ;
  const char* phrase_outro[] = ?? ;
  
  const int nb_phrases_intro = sizeof(phrase_intro) / sizeof(phrase_intro[0]); //Récupère la taille des tableaux pour connaître le nombre de phrases
  const int nb_phrases_outro = sizeof(phrase_outro) / sizeof(phrase_outro[0]);
  
  int largeur;
  u8g2.firstPage(); // u8g2 fonctionne avec un système de pages. Chaque partie de l'écran correspond à une page, il faut donc défiler entre les pages pour afficher tout l'écran. Cette méthode permet d'utiliser moins de mémoire dynamique.
  do {
    switch(selecteur){ //Sélecteur = 0 si un mouvement est détecté, 1 si le décompte est en cours, 2 si le décompte est terminé
      case 0:
        /* ########## AFFICHE TES PROPRES PHRASES EN UTILISANT PHRASE_INTRO, UNE BOUCLE 'FOR' ET LA VARIABLE 'NB_PHRASES_INTRO' POUR PARCOURIR LES DIFFERENTES PHRASES DE TON TABLEAU. NE LES FAIS PAS TROP LONGUES ET N'EN MET PAS TROP, L'ECRAN NE FAIT QUE 128X64 PIXELS ########## */
        break;
      case 1:
        animation_brosse_a_dents(decalage); 
        /* ########## AFFICHE TES PROPRES PHRASES EN UTILISANT PHRASE_BROSSE, UNE BOUCLE 'FOR' ET LA TAILLE DE PHRASE_BROSSE POUR PARCOURIR LES DIFFERENTES PHRASES DE TON TABLEAU. ########## */
        break;
      case 2:
        /* ########## AFFICHE TES PROPRES PHRASES EN UTILISANT PHRASE_OUTRO, UNE BOUCLE 'FOR' ET LA VARIABLE 'NB_PHRASES_OUTRO' POUR PARCOURIR LES DIFFERENTES PHRASES DE TON TABLEAU ########## */
        break; 
    }
  } while (u8g2.nextPage());
}

/*-------------------------------------------------------------------------------------------------*/

/* Fonction d'initialisation des différents composants et capteurs, puis lancement du mode Veille. C'est la fonction qui est appelée en premier lors du démarrage de l'Arduino */
void setup() {

  /* ########## ICI ESSAYE DE RAJOUTER UNE LIGNE POUR GERER LA LUMINOSITE DE L'ECRAN 7 SEGMENTS EN TE RENSEIGNANT SUR LA LIBRAIRIE TM1637Display : https://github.com/avishorp/TM1637 ########## */

  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  
  attachInterrupt(digitalPinToInterrupt(BOUTON),btn_ISR, FALLING); //La fonction btn_ISR sera appelée lorsque le bouton sera pressé. C'est ce qu'on appelle une interruption. Cela permet d'éviter de surveiller constamment l'état du bouton dans la fonction loop()

  Timer1.initialize(1000000); //1 000 000 de µs = 1 s
  Timer1.attachInterrupt(timer1_interruption);//Appellera cette fonction toutes les secondes. C'est encore un système d'interruption
  
  Serial.begin(9600); //Initialise la connection série pour d'éventuels Debugs

  initialisation_OLED(); //initialise le fonctionnement de l'écran OLED

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Initialiser le mode de sommeil qui consomme le moins lors de la mise en veille de l'appareil
  sleep_fct(); //Lance directement la fonction de sommeil de l'appareil

}


/* Fonction appelée pour lancer le mode veille. Elle réinitialise certaines variables et assigne une interruption au capteur de mouvement, pour permettre à l'Arduino de se réveiller si un mouvement (ou un appui de bouton) est détecté */
void sleep_fct(){
  
  /* ########## ICI ECRIT UNE LIGNE POUR AJOUTER UNE INTERRUPTION LIÉE AU CAPTEUR DE MOUVEMENT. PRENDS EXEMPLE SUR UNE COMMANDE SIMILAIRE DE LA FONCTION SETUP ##########*/
  
  Serial.println("Appareil en veille ...");

  u8g2.clear(); //Eteint l'OLED
  afficheur.setSegments(afficheur_eteint); //Eteint l'afficheur
  //Réinitialisation des variables de décompte
  minuteur = TEMPS_DE_BROSSAGE;
  mise_en_veille = TEMPS_VEILLE;
  
  sleep_enable();
  
  // Passe l'arduino en mode veille
  sleep_mode();
  
  //Le programme se bloque ici tant qu'une interruption n'est pas détectée depuis le capteur de mouvement ou un appui de bouton

  //L'interruption réveille le programme
  sleep_disable();
  power_all_enable();
  
  Serial.println("Mouvement détecté. Réveil");
  
  afficheur_OLED(0); //Affiche le message de bienvenue à l'écran
  
  detachInterrupt(digitalPinToInterrupt(CAPTEUR_MOUVEMENT)); //Désactive l'interruption par le capteur de mouvement car inutile tant que l'appareil ne dort pas
}


/* Convertit une valeur au format secondes en une valeur en format minutes : secondes pour l'afficheur */
int conversion_secondes_minutes(unsigned int number){
  /* ########## REMPLIS TOI-MÊME CETTE FONCTION, L'IDÉE EST DE CONVERTIR UNE VALEUR EN SECONDES A UN AFFICHAGE EN MINUTES:SECONDES.
   * LES ':' SONT AUTOMATIQUEMENT AFFICHÉS SUR L'ECRAN, CE QUI SIGNIFIE QUE POUR CONVERTIR DEUX MINUTES PAR EXEMPLE, ON VEUT QUE 120 DEVIENNE 200 (CE QUI AFFICHERA 02:00 À L'ÉCRAN). POUR TROIS MINUTES, ON VEUT QUE 180 DEVIENNE 300.
   * ESSAIE DE TRANSFORMER LA VARIABLE DE FACON A CE QUE LES MINUTES DEVIENNENT LE CHIFFRE DES CENTAINES, ET LES SECONDES DEVIENNENT LE CHIFFRE DES DIZAINES ET DES UNITES
   * TU AURAS SUREMENT BESOIN DE L'OPERATEUR MODULO : '%'
   * N'OUBLIE PAS DE FAIRE UN 'return number;' POUR QUE TA VARIABLE MODIFIÉE SOIT BIEN RENVOYÉE VERS LA SORTIE DE LA FONCTION
    */
}


/*Fonction appelée par le timer chaque seconde par interruption */
void timer1_interruption() {
  if (decompte_minuteur && minuteur > 0) minuteur--; //Si la variable decompte_minuteur est vraie, on décrémente la valeur du minuteur
  else if (!decompte_minuteur) mise_en_veille--; //Si le minuteur ne décompte pas, cela signifie qu'aucune action n'est en cours et donc on décrémente le décompte avant la mise en veille
}


/* Fonction appelée lorsque le bouton est pressé */
void btn_ISR() {
/* ########## AJOUTE UNE CONDITION 'IF' POUR GERER LE REBOND DU BOUTON. RENSIGNE TOI SUR LA COMMANDE 'millis()' ET UTILISE CERTAINES VARIABLES DEFINIES AU DEBUT. IL FAUDRA AUSSI CHANGER LA VALEUR D'UNE VARIABLE APRÈS AVOIR FERMÉ LA CONDITION ########## */
  {
    decompte_minuteur = !decompte_minuteur; //Si le décompte était lancé, il se met en pause. S'il était en pause, il se lance
    if (decompte_minuteur) minuteur = TEMPS_DE_BROSSAGE; //Si cet appui lance le décompte, le décompte repart de la valeur de brossage initiale 
    
    Serial.print("Button pushed, decompte_minuteur = ");
    Serial.println(decompte_minuteur);
    mise_en_veille = TEMPS_VEILLE; //Réinitialisation du compteur de mise en veille car une action a été effectuée
  }
  
}


/*Fonction appelée lorsqu'un mouvement est détecté. Elle est vide car le seul but du détecteur de mouvement est de réveiller l'appareil */
void pir_ISR(){
}

/* Fonction lancée en boucle lorsque l'appareil n'est pas en mode sommeil */
void loop() {

  afficheur.showNumberDecEx(conversion_secondes_minutes(minuteur), 0b01000000, true); //Affiche le temps restant sur le minuteur, après sa conversion en format "minutes:secondes". Le deuxième argument affiche les deux points du format minute:seconde. Le true permet d'afficher les 0 devant les minutes
  
  if ((minuteur == TEMPS_DE_BROSSAGE) && (decompte_minuteur == true)){ //Lorsque le décompte démarre
    
   /* ########## AJOUTE UNE MUSIQUE DANS LE FICHIER TUNES.INO ET APPELLE-LA ICI POUR QU'ELLE SE LANCE LORSQUE LE DECOMPTE DEMARRE ##########
    * ton_jingle_intro() */
    
    minuteur--; //Il est nécessaire de retirer une seconde ici sinon la musique se répète plusieurs fois avant que le timer effectue le décompte
    
  }

  if (decompte_minuteur == true)
  {
    /* ########## APPELLE LA FONCTION D'ANIMATION DE L'ECRAN OLED ICI. N'OUBLIE PAS D'AJOUTER LE PARAMETRE DE DECALAGE ET DE L'INCRÉMENTER ########## */
  }
  
  if (minuteur == 0 && (decompte_minuteur == true)){ //Lorsqu'on arrive à la fin du décompte
    
    /* ########## AJOUTE UNE MUSIQUE DANS LE FICHIER TUNES.INO ET APPELLE-LA ICI POUR QU'ELLE SE LANCE LORSQUE LE DECOMPTE EST FINI ##########
    * ton_jingle_outro() */
    
    decompte_minuteur = false; //Stoppe le décompte lorsqu'on atteint 0
    afficheur_OLED(2); //Affiche le message d'au revoir à l'écran
  }
  if (mise_en_veille == 0) sleep_fct(); //Lorsque le décompte de mise en veille est terminé, l'Arduino se rendort

}
