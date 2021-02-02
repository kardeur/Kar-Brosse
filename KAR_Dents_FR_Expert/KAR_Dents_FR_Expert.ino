
/*
 * Programme Arduino Nano pour le kit KAR-Dents.
 * NIVEAU EXPERT : A PART LES VARIABLES GLOBALES ET LES DEFINITIONS DE FONCTIONS, RIEN D'AUTRE QUE DES COMMENTAIRES DE CODE NE SONT FOURNIS
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


/* ########## REMPLIS TOI MEME LES TEMPS DE DECOMPTE ET LES PINS CORRESPONDANT A CHAQUE COMPOSANT. REFERE TOI AU TUTORIEL POUR CONNAITRE LE SCHEMA DE BRANCHEMENT ######### */

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
  /* Consulte la documentation pour initialiser l'écran OLED, la police, la taille, la rotation de l'écran, ... à l'adresse  https://github.com/olikraus/u8g2/wiki/u8g2reference ########## */
}

/* Fonction d'animation de la brosse à dents pendant le brossage */
void animation_brosse_a_dents(int decalage)
{
  /* Utilise cette fonction pour créer ton animation sur l'écran OLED, et la diffuser pendant le décompte */
}

/* Fonction d'affichage sur l'écran. Appelle des fonctions de la librairie u8g2 ou animation_brosse_a_dents. Le paramètre decalage est optionnel et vaut 0 par défaut */
void afficheur_OLED(uint8_t selecteur, uint32_t decalage=0)
{
  /* Cette fonction gère l'afficheur OLED et affiche des phrases ou des images différentes en fonction de la valeur du sélecteur.
   * Dans le programme de base, selecteur = 0 signifie un affichage du texte à l'allumage, selecteur = 1 diffuse une animation pendant le décompte, et sélecteur = 2 affiche du texte à la fin du décompte
   */
}

/*-------------------------------------------------------------------------------------------------*/

/* Fonction d'initialisation des différents composants et capteurs, puis lancement du mode Veille. C'est la fonction qui est appelée en premier lors du démarrage de l'Arduino */
void setup() {
  
  /* Initialise plusieurs paramèteres :
  - la luminosité de l'écran 7 segments
  - le buzzer
  - l'interruption causée par le bouton et la fonction btn_ISR
  - le Timer1 et l'interruption qui appelle timer1_interruption()
  - l'écran OLED 
  - l'initialisation et l'appel au mode sommeil
  */ 

}


/* Fonction appelée pour lancer le mode veille. Elle réinitialise certaines variables et assigne une interruption au capteur de mouvement, pour permettre à l'Arduino de se réveiller si un mouvement (ou un appui de bouton) est détecté */
void sleep_fct(){

  /* Dans cette fonction :
   - on attache une interruption au capteur de mouvement et à la fonction btn_pir()
   - on réinitialise certaines variables
   - on passe en mode sommeil
   - on réveille l'appareil
   - on désactive l'interruption du capteur de mouvement car il ne sert plus une fois que le module est réveillé
   - éventuellement, on affiche un texte sur l'écran OLED
 */
 
}


/* Convertit une valeur au format secondes en une valeur en format minutes : secondes pour l'afficheur */
int conversion_secondes_minutes(unsigned int number){
  

  /* Utiliser cette fonction pour convertir du format secondes au format 'minutes : secondes'.
   * L'afficheur 7 segments peut afficher le ":". Ce qu'on veut donc faire c'est par exemple convertir deux minutes de la valeur 120 à 200, pour qu'il soit noté 02:00 sur l'afficheur
   * Il faut également que la valeur calculée passe de 200 à 159, par exemple, pour correspondre avec le format 'minutes : secondes' (de 02:00 à 01:59)
   */
   
}


/*Fonction appelée par le timer chaque seconde par interruption */
void timer1_interruption() {
  
  /* Fonction reliée au Timer1, appelée toutes les secondes, en charge de la décrémentation soit du décompte, soit du compteur de mise en veille. Cela dépendra de l'état de la variable decompte_minuteur */
  
}


/* Fonction appelée lorsque le bouton est pressé */
void btn_ISR() {
  
  /* Cette fonction liée à l'interruption du bouton est appelée quand le bouton est pressé. En fonction de l'état actuel du programme (et donc de decompte_minuteur), on change l'état de decompte_minuteur et on stoppe le décompte de mise en veille
   * On peut également veiller à entourer ces commandes d'une condition 'if' qui gère le rebond du bouton. Cela empêchera le programme de considérer plusieurs appuis dans un délai très court s'il n'y en a eu qu'un seul. On utilisera les variables delaiAppuiBouton et DelaiEntreDeuxAppuis, ainsi que la fonction millis()
   */
}


/*Fonction appelée lorsqu'un mouvement est détecté. Elle est vide car le seul but du détecteur de mouvement est de réveiller l'appareil */
void pir_ISR(){

  /* A laisser vide, l'interruption liée au capteur de mouvement est uniquement utile pour réveiller du mode sommeil */ 
}

/* Fonction lancée en boucle lorsque l'appareil n'est pas en mode sommeil */
void loop() {
  
}
  /*
   * Fonction qui tournera une fois l'appareil réveillé. On te conseille de faire :
   - Afficher le compteur sur le 7 segments. Utilise https://github.com/avishorp/TM1637/blob/master/TM1637Display.h pour connaître les fonctions utiles et chaque paramètre.
   - Des conditions if / else en fonction de la valeur de minuteur et decompte_minuteur.
   - Incorporer la fonction affichage_OLED et utiliser le buzzer en créant des musiques dans tunes.ino
   - Relancer le mode sommeil si mise_en_veille atteint 0
   *
   */

}
