
/*
 * Arduino Nano program for the KAR-Dents DIY kit.
 * EXPERT LEVEL : APART FROM THE GLOBAL VARIABLES AND THE FUNCTION DEFINITION, NOTHING EXCEPT COMMENTS ARE GIVEN
 * 
 * To get every additional library used by this program (the ones with the #include command), download them by clicking on Sketch -> Include library -> Manage Libraries. Write the name of the desired library, click on it then "Install".
 * 
 * To upload the program on the provided Arduino, you have to install the following driver first : https://sparks.gogo.co.nz/assets/_site_/downloads/CH34x_Install_Windows_v3_4.zip. Uncompress the archive and launch the executable file.
 * Then, in the "Tools" section, select Arduino Nano as the Board, ATMega328p (Old Bootloader) as Processor, and select the corresponding COM port for your board. Then you can upload the program on your board.
 *
 *Comments are available along the program to help you understanding it.
 * 
 * Code written by Kardeur.io
 * @author : Hugo Leurent, hugo@kardeur.io
 * 
 */

#include <TM1637Display.h> //TM1637Display is used to add the necessary functions for the 4*7 segments display
#include <avr/sleep.h> //This library handles the sleep mode of the Arduino, to save as much energy as possible
#include <avr/power.h> //This one handles which parts of the Arduino are powered
#include <TimerOne.h> //TimerOne add control functions for one of the Arduino timers
#include <U8g2lib.h> //OLED screen library. Documentation available at https://github.com/olikraus/u8g2/wiki/u8g2reference


/* ########## FILL THE COUNTDOWN TIMES AND THE PIN NUMBER BY YOURSELF. REFER TO THE TUTORIAL FOR THE PIN SCHEMATIC  ######### */

#define BRUSH_TIME ?? //Time (in seconds) for brushing your teeth. You can change its value here depending on your own hygiene level ;)
#define TIMEOUT_VALUE ?? //Time (in secondes) before the device goes to standby mode when no action is detected. You can change its value here

//Pins attributed to each component plugged to the board.
#define CLK_DISPLAY ??
#define DATA_DISPLAY ??

#define BUZZER ??

#define BUTTON ??
#define PIR_SENSOR ??


U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //Initialization of the OLED screen with the SH1106 driver (u8g2 handles many different drivers)
uint32_t offset_OLED = 0; //Variable handling the motion of the picture on the screen

// Pixels displaying the toothbrush on screen
uint8_t toothbrush_img[370] = {
0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111110,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11111011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b11111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111111,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111110,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01101111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000001,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111100,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000111,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11110000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000011,0b11111110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001111,0b11111000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00111111,0b11100000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01111111,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000
 };

// Pixels displaying the toothpaste on screen
uint8_t toothpaste_img[40] = {
0b00000000,0b00000000,0b00001111,0b11000000,0b00000000,0b00000000,0b00111111,0b11110000,0b00000000,0b00000000,0b11111111,0b10000000,0b00000000,0b00000001,0b11111100,0b00000000,0b00000000,0b00000011,0b11110000,0b00000000,0b00000000,0b00000111,0b11100000,0b00000000,0b00000000,0b00001111,0b11000000,0b00000000,0b00000000,0b01111111,0b10000000,0b00000000,0b00000011,0b11111111,0b00000000,0b00000000,0b00000001,0b11111100,0b00000000,0b00000000
 };
 
TM1637Display seven_seg_display(CLK_DISPLAY,DATA_DISPLAY);
unsigned int brush_timer = BRUSH_TIME;  //Variable counting the time left in the brush timer
unsigned int timeout = TIMEOUT_VALUE; //Counting the time left before timeout and thus going to standby mode
bool play_brush_timer = false; //Indicates if the brush timer should be decrementing or stopped

const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};

//Variables used to prevend the bounce of the button (without those, there's a possibility that by pressing the button only once, the information will be sent multiple times, which we want to avoid)
unsigned long lastInterruptTime = 0;
unsigned long BtnInterruptDelay = 200; //Corresponds to 200ms

/*---------------------------- OLED DISPLAY FUNCTIONS -----------------------------------------*/

/* Initialization of the OLED screen parameters (size, font, ...) */
void initialization_OLED()
{
  /* Visit the u8g2 documentation to initialize size, font, rotation, and begin the OLED displaying : https://github.com/olikraus/u8g2/wiki/u8g2reference  */
}

/* Toothbrush animation on screen during the brushing */
void animation_toothbrush(int offset)
{
 /* Use this function to create your own animation for the OLED screen and run it while the countdown runs */
}

/* Handler of the OLED display. Regroups the different calls to the u8g2 lib or the animation_toothbrush function. The offset parameter is optional and is 0 by default  */
void display_OLED(uint8_t index_OLED, uint32_t offset=0)
{
  /* This function handles the OLED screen and displays sentences, images or animations depending the value of index_OLED
   *  In the base program, index_OLED = 0 means displaying sentences when the sleeping mode stops, index_OLED = 1 shows the previously written animation, and index_OLED = 2 shows a message at the end of the timer.
   */
}

/*-------------------------------------------------------------------------------------------------*/

/* Function for setting up the components and sensors then launching standby mode. It's the first function called when Arduino starts */
void setup() {
  
  /* Initializes multiple parameters :
   - 7-segments screen brightness
   - Buzzer
   - Interruption caused by the pushbutton, calling the btn_ISR function
   - Timer1 initialisation and the interruption calling timer1_interruption()
   - OLED screen
   - Initialisation and calling of the sleeping mode
   */
   
}


/* Function called when going into standby mode. Resets the value of some variables and assigns an interrupt to the PIR sensor, to allow it to wake up the device */
void sleep_fct(){

  /* In this function, it is advised to :
   - Attach an interrupt to the PIR sensor and to the btn_PIR() function
   - Reinitializing of some timers and variables
   - Starting sleep mode
   - Waking up the device
   - Deactivating the interrupt from the PIR once the device is awake, since the PIR has no use anymore until next sleep
   - Optionnaly, displaying some text on the OLED screen
   */
}


/*Conversion of the seconds format to minutes:seconds format */
int conv_seconds_minutes(unsigned int number){

  /* Using this function to convert the secondes format into 'minutes : seconds'
   *  The 7 segment display can already display the ":". What we want to do is converting 120 into 200 for example, in order to go from 120 seconds to displaying 02:00.
   *  120 becomes 200 (02:00), 119 becomes 159 (01:59), etc ...
   */
}


/*Function called by the timer each second when an interrupt is triggered by Timer1 */
void timer1_interrupt() {
 /* Function called each second by Timer1. In charge of the decrementation either of the countdown or the timeout. This will depend on the state of the play_brush_timer variable */


/* Function called when the button is pressed */
void btn_ISR() {

  /* Function called whenever the button is pressed. Depending on the actual state of the program (and the state of play_brush_timer), we change the variable value and restart the timeout timer.
   *  We can also surrond the code by an 'if' statement, which prevents artificial bounce of the button. When a button is pressed, it is possible that multiple presses are detected in a very short delay.
   *  You can prevent this by using the variables lastInterruptTime and BtnInterruptDelay, and the function millis().
   */
}


/* Function called when PIR causes an interrupt. Empty because there's no use for the PIR apart from waking up the device */
void pir_ISR(){

  /* This function can stay empty. We don't need the PIR to trigger anything else than waking up the device */
}


/* Loop function when the device isn't in standby mode */
void loop() {

  /* 
   *  The function which will loop once the device has woken up. We advise you to :
   - Display the countdown on the 7-segments. Functions and parameters can be found at https://github.com/avishorp/TM1637/blob/master/TM1637Display.h
   - if/else statements depending on the value of the brush timer and play_brush_timer.
   - Incorporate the display_OLED function and the buzzer by creating tracks in the tunes.ino file
   - Go back to sleeping mode if the timeout reaches 0
   */

}
