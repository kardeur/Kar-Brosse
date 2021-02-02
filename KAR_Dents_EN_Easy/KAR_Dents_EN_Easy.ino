
/*
 * Arduino Nano program for the KAR-Dents DIY kit.
 * BEGINNER LEVEL : THE WHOLE PROGRAM IS GIVEN AND DOCUMENTED
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


#define BRUSH_TIME 120 //Time (in seconds) for brushing your teeth. You can change its value here depending on your own hygiene level ;)

#define TIMEOUT_VALUE 30 //Time (in secondes) before the device goes to standby mode when no action is detected. You can change its value here

//Pins attributed to each component plugged to the board.
#define CLK_DISPLAY 5
#define DATA_DISPLAY 4

#define BUZZER 6

#define BUTTON 2
#define PIR_SENSOR 3

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
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf); //Handles font and size
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosCenter();
  u8g2.setFontDirection(0);
}

/* Toothbrush animation on screen during the brushing */
void animation_toothbrush(int offset)
{
  if (offset < 20) {
  u8g2.drawBitmap(20,20,10,37,toothbrush_img);
  u8g2.drawBitmap(64,-11+offset,4,10,toothpaste_img);
  }
  
  else if (offset < 40) {
  u8g2.drawBitmap(20 - (offset-20),20,10,37,toothbrush_img);
  u8g2.drawBitmap(64 - (offset-20), 9,4,10,toothpaste_img);
  }
  
  else if (((offset-40)/46)%2==0){
  u8g2.drawBitmap(20-20 + (offset-40)%46,20,10,37,toothbrush_img);
  u8g2.drawBitmap(64-20 + (offset-40)%46, 9,4,10,toothpaste_img);
  }
  
  else {
  u8g2.drawBitmap(20+26 - (offset-40)%46,20,10,37,toothbrush_img);
  u8g2.drawBitmap(64+26 - (offset-40)%46, 9,4,10,toothpaste_img);
  }
}

/* Handler of the OLED display. Regroups the different calls to the u8g2 lib or the animation_toothbrush function. The offset parameter is optional and is 0 by default  */
void display_OLED(uint8_t index_OLED, uint32_t offset=0)
{
  const char* sentences_intro[] = {"Brush your teeth !", "Press the button"}; //Customize your own sentances to display here by replacing or adding elements
  const char* sentences_brush = "BRUSH ! BRUSH !";
  const char* sentences_outro[] = {"Great job !", "See you soon !", "kardeur.io"};
  
  const int nb_sentences_intro = sizeof(sentences_intro) / sizeof(sentences_intro[0]); //Computes the size of the sentances arrays to be used later in the loops
  const int nb_sentences_outro = sizeof(sentences_outro) / sizeof(sentences_outro[0]);
  
  int width;
  u8g2.firstPage(); // u8g2 uses a page system. Each part of the screen is a page. It means the program has to go through multiple pages to display the whole image. This method allows the program to use less dynamic memory
  do {
    switch(index_OLED){ //index = 0 when a motion is detected, 1 if the counter is running, 2 when the brush timer is over
      case 0:
        for (int i=0; i < nb_sentences_intro; i++) //This loop displays the sentences from the sentences_intro array on the screen
        {
          width=u8g2.getStrWidth(sentences_intro[i]);
          u8g2.drawUTF8(64-width/2,(64 * (i + 1)) / (nb_sentences_intro + 1),sentences_intro[i]); //The placement of each sentence on the screen is automatically computed, depending on the number of sentences
        }
        break;
      case 1:
        animation_toothbrush(offset); 
        width=u8g2.getStrWidth(sentences_brush);
        u8g2.drawStr(64-width/2, 54, sentences_brush);
        break;
      case 2:
         for (int i=0; i < nb_sentences_outro; i++) //This loop displays the sentences from the sentences_outro array on the screen
        {
          width=u8g2.getStrWidth(sentences_outro[i]);
          u8g2.drawUTF8(64-width/2,(64 * (i + 1)) / (nb_sentences_outro + 1),sentences_outro[i]);
        }
        break; 
    }
  } while (u8g2.nextPage());
}

/*-------------------------------------------------------------------------------------------------*/

/* Function for setting up the components and sensors then launching standby mode. It's the first function called when Arduino starts */
void setup() {

  seven_seg_display.setBrightness(1); //Brightness can be set between 0-7

  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON),btn_ISR, FALLING); //Function btn_ISR will be called when the button is pressed. That's what we call an interrupt. It allows the microcontroller to avoid constantly checking the state of the button in the loop() function.

  Timer1.initialize(1000000); //1 000 000 s = 1cs
  Timer1.attachInterrupt(timer1_interrupt); //Calls this function each second through an interrupt.
  
  Serial.begin(9600); //Initialization if the serial communication for possible debugs

  initialization_OLED(); //Initialize the OLED screen functions
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Initialization if the standby mode which uses the least current when sleeping
  sleep_fct(); //Directly puts the device in standby mode
   
}


/* Function called when going into standby mode. Resets the value of some variables and assigns an interrupt to the PIR sensor, to allow it to wake up the device */
void sleep_fct(){
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR), pir_ISR, RISING); //Assigns an interrupt to the PIR sensor. The interrupt will trigger the waking up of the device
  Serial.println("Sleeping...");

  u8g2.clear(); //Turns the OLED screen off
  seven_seg_display.setSegments(blank); //Turns the display off
  //Reset the decrementation variables
  brush_timer = BRUSH_TIME;
  timeout = TIMEOUT_VALUE;
  
  sleep_enable();
  
  // Now enter sleep mode.
  sleep_mode();

  //The function blocks here while the Arduino is asleep. It can only be woke up by a motion detected by the PIR sensor or a button pushed
  
  //interrupt disables sleep.
  sleep_disable();
  // Re-enable the peripherals.
  power_all_enable();
  
  Serial.println("Motion detected. Waking up");

  display_OLED(0); //Displays the welcome message on the OLED screen
  
  detachInterrupt(digitalPinToInterrupt(PIR_SENSOR)); //Deactivates the motion sensor interrupt since we don't need it except for waking up the device
}


/*Conversion of the seconds format to minutes:seconds format */
int conv_seconds_minutes(unsigned int number){
  int minutes = number / 60;
  int secondes = number % 60;
  return ((100*minutes)+secondes);
}


/*Function called by the timer each second when an interrupt is triggered by Timer1 */
void timer1_interrupt() {
  if (play_brush_timer && brush_timer >0) brush_timer--; //Decrementation of the brush counter when the play_brush_timer is true
  else if (!play_brush_timer)timeout--; //If the brush timer doesn't decrement, there's no action and the timeout timer gets in use
}


/* Function called when the button is pressed */
void btn_ISR() {
  if ((millis() - lastInterruptTime) > BtnInterruptDelay) //This condition will determine if the button is really pressed of if it's only the result of a bounce. We compare the time spend with the last time the button has been pressed and if it's lower than BtnInterruptDelay, it's not considered a real push
  {
    play_brush_timer = !play_brush_timer; //If the timer has been started, it is paused. In the other case, it starts
    if (play_brush_timer) brush_timer = BRUSH_TIME; //if pushing the button starts the timer, the brush timer resets from the start
    
    Serial.print("Button pushed, play_brush_timer = ");
    Serial.println(play_brush_timer);
    timeout = TIMEOUT_VALUE; //Resets the timeout timer if an action has been performed
  }
  lastInterruptTime = millis(); //Takes the actual time to store it in lastInterruptTime for comparison with the next press of the button
}


/* Function called when PIR causes an interrupt. Empty because there's no use for the PIR apart from waking up the device */
void pir_ISR(){
}


/* Loop function when the device isn't in standby mode */
void loop() {

  seven_seg_display.showNumberDecEx(conv_seconds_minutes(brush_timer), 0b01000000, true); //Displays the brush timer after being converted to "minutes:seconds". The second argument displays the two dots for the minutes:seconds format. The "true" enables display of leading zeros 
  if ((brush_timer == BRUSH_TIME) && (play_brush_timer == true)){ //When the brush timer starts
    zelda_song(); //Calls a function declared in Tunes.ino files, plays a little starting music
    brush_timer--; //We need to take a second back from the brush timer, or else the music plays multiple time before the timer1 can take over the countdown
      
  }

  if (play_brush_timer == true) display_OLED(1, offset_OLED+=2); //Calls the toothbrush animation when the timer is running. Here we call the offset and increment it at the same time
  
  if (brush_timer == 0 && (play_brush_timer == true)){ // Called when the brush countdown ends
    mario_song(); //Calls a function declared in Tunes.ino files, plays a little ending music
    play_brush_timer = false; //Stops the countdown when reaching 0
    display_OLED(2); //Displays the goodbye message on the OLED screen
    
  }
  if (timeout == 0) sleep_fct(); //Device goes back to standby mode when the timeout reaches 0

}
