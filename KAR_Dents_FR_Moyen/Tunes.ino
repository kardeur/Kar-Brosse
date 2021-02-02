#include <Arduino.h>

void mario_song(){
  tone(BUZZER, 659);
  delay(120);
  noTone(BUZZER);
  delay(30);
  tone(BUZZER, 659);
  delay(270);
  noTone(BUZZER);
  delay(30);
  tone(BUZZER, 659);
  delay(150);
  noTone(BUZZER);
  delay(150);
  tone(BUZZER, 523);
  delay(150);
  tone(BUZZER, 659);
  delay(300);
  tone(BUZZER, 784);
  delay(200);
  noTone(BUZZER);
  delay(400);
  tone(BUZZER, 392);
  delay(120);
  noTone(BUZZER);
}

void zelda_song()
{
  tone(BUZZER, 880);
  delay(150);
  tone(BUZZER, 930);
  delay(150);
  tone(BUZZER, 987);
  delay(150);
  tone(BUZZER, 1046);
  delay(300);
  noTone(BUZZER);
}

void your_jingle_intro ()
{
  /* ########## CREATE YOUR OWN JINGLE BY FOLLOWING THE PREVIOUS EXAMPLES ########## */
}

void your_jingle_outro ()
{
  /* ########## CREATE YOUR OWN JINGLE BY FOLLOWING THE PREVIOUS EXAMPLES ########## */
}
