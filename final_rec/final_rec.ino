/*
Code pour récepteur Li-Fi
Par DO Nhat Minh et LE Nguyen Phong
Projet TPE 2018 - 2019
*/

#include <elapsedMillis.h>
#include <LiquidCrystal.h>

#define INTERVAL 5000 // Temps T (intervalle entre chaque bit) en µs
#define TIME_OUT 2500 // La durée time-out pour vérification

const int attente = INTERVAL / 4 * 3;

LiquidCrystal lcd(2, 4, 5, 6, 7, 8); // Définir l'écran LCD

const int sensor = A1; // Port du panneau solaire
const int pot = A5; // Port du potentiomètre VR2

// Variables pour le mode calibration
const int cal = 9;
int calButton = 2;
int potVal;
int calPotVal;
int sensorVal;
int calSensorVal;

const int startLen = 4;
int start[startLen] = {0, 1, 0, 1}; // Commande "start"
int val[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Tableau pour stockage temporaire des bits lors d'une lecture

elapsedMicros chrono; // Définir le chronomètre

bool error = false; // Variable permettant de détecter les erreurs de lecture

void setup() {
  lcd.begin(16, 2);
  pinMode(sensor, INPUT);
  pinMode(pot, INPUT);
  pinMode(cal, INPUT);
}

void loop() {
  if(digitalRead(cal) == LOW){  // mode attente du message
    if(calButton != 0){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pret.");
      calButton = 0;
    }
    bool startRec = false;
    sensorVal = analogRead(sensor);
    if(sensorVal <= potVal){
      chrono = 0;
      delayMicroseconds(TIME_OUT);
      chrono = 0;
      val[0] = 0;
      if(readChar(1, 4) == true){  // Vérification du commande start
        for(int i = 0; i < 4; i++){
          if(val[i] != start[i]){
            break;
          }
          if(i == 3){
            startRec = true;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Lecture...");
          }
        }
      }

      if(startRec == true){ // Si commande start valide, commencer la lecture
        
        if(readChar(0, 8) == true){ // Lire la longueur du message
          int msgLen = deccon(8);

          lcd.setCursor(0, 1);
          lcd.print(msgLen);

          int msgBin[msgLen][8];

          // Lecture du message
          for(int i = 0; i < msgLen; i++){
            if(readChar(0, 8) == true){
              for(int a = 0; a < 8; a++){
                msgBin[i][a] = val[a];
              }
            }
            else{
              error = true;
              break;
            }
          }

          if(error == false){

            // Affichage du message sur l'écran LCD

            char msg[msgLen];
            for(int i = 0; i < msgLen; i++){
              for(int a = 0; a < 8; a++){
                val[a] = msgBin[i][a];
              }
              msg[i] = deccon(8);
            }
            lcd.clear();
            for(int i = 0; i < msgLen; i++){
              lcd.write(msg[i]);
              if(i == 15){
                lcd.setCursor(0, 1);
              }
            }
            startRec = false;
          }
        }
        else{
          error = true;
        }
      }

      // En cas d'erreur, afficher "Erreur !" sur l'écran LCD

      if(error == true){
        lcd.clear();
        lcd.print("Erreur !");
        error == false;
      }
    }
  }

  else{ // Mode calibration qui permet le réglage du seuil de tension du panneau solaire
    if(calButton != 1){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pot: ");
      lcd.print(analogRead(pot));
      lcd.setCursor(0, 1);
      lcd.print("Sensor: ");
      lcd.print(analogRead(sensor));
      calButton = 1;
    }
    potVal = analogRead(pot);
    sensorVal = analogRead(sensor);
    if(chrono > 500000){
      if(potVal != calPotVal){
        calPotVal = potVal;
        lcd.setCursor(5, 0);
        lcd.print("         ");
        lcd.setCursor(5, 0);
        lcd.print(potVal);
      }
      if(sensorVal != calSensorVal){
        calSensorVal = sensorVal;
        lcd.setCursor(8, 1);
        lcd.print("       ");
        lcd.setCursor(8, 1);
        lcd.print(sensorVal);
      }
      chrono = 0;
    }
  }
}

bool readChar(int deb, int len){ // Lecture d'un caractère (série de 8 bits)

  bool notError = true;

  if(deb == 0){ // Attendre la fin de la pause si au début de la série
    while(chrono < INTERVAL){}
    chrono = 0;
  }

  for(int i = deb; i < len; i++){

    while(chrono < attente){}; // Attendre le moment de lecture
    chrono = 0;

    if(analogRead(sensor) <= potVal){
      
      val[i] = 0;
      
      while(analogRead(sensor) <= potVal && chrono < TIME_OUT){}
      if(chrono < TIME_OUT){
        chrono = 0; // Remise du chronomètre à zéro; correction des décalages temporels
      }
      else{
        notError = false;
        break;
      }
    }
    else{

      val[i] = 1;

      while(analogRead(sensor) > potVal && chrono < TIME_OUT){}
      if(chrono < TIME_OUT){
        chrono = 0; // Même chose
      }
      else{
        notError = false;
        break;
      }
    }
  }
  return notError; // Retourner l'état d'erreur
}

int deccon(int len){  // Conversion de la série binaire val en nombre décimal
  int num = 0;
  for(int i = 0; i < 8; i++){
    num += val[7 - i] * bit(i); // La fonction bit(n) retourne 2 puissance n
  }
  return num;
}
