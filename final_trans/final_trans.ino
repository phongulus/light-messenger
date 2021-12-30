/*
Code pour transmetteur Li-Fi
Par DO Nhat Minh et LE Nguyen Phong
Projet TPE 2018 - 2019
*/

#include <elapsedMillis.h>

#define INTERVAL 5000 // Temps T (intervalle entre chaque bit) en µs

const int led = 7; // Port de la DEL

const int attente = INTERVAL / 2;
elapsedMicros chrono; // Définition du chronomètre

String message = "";
int val[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Stockage temporaire de la série binaire à envoyer

const int startLen = 4;
int start[startLen] = {0, 1, 0, 1}; // Commande start

void setup() {
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    Serial.begin(9600);
}

void loop() {
    if(Serial.available() > 0){
        message = Serial.readString();
        int longueur = message.length();
        longueur--;
        if(longueur > 255){ // Ne pas envoyer si le message excède 255 caractères
            Serial.println("Message trop long");
        }
    else{
        int msgBin[longueur][8];
        for(int i = 0; i < longueur; i++){ // Conversion du message en code binaire
            deccon(message.charAt(i) + 0);
            for(int a = 0; a < 8; a++){
                msgBin[i][a] = val[a];
            }
        }
        deccon(longueur);
        /* Code pour débogage, affichage des séries binaires sur ordinateur
        for(int i = 0; i < startLen; i++){Serial.print(start[i]);};
        Serial.println(" ");
        for(int i = 0; i < 8; i++){Serial.print(val[i]);};
        Serial.println(" ");
        for(int i = 0; i < longueur; i++){
            for(int a = 0; a < 8; a++){Serial.print(msgBin[i][a]);};
            Serial.println(" ");
        }
        */
        chrono = 0; // Remettre le chronomètre à zéro, pour commencer la transmission
        sendChr(start, startLen); // Envoyer la commande start
        sendChr(val, 8); // Envoyer la longueur du message
        for(int i = 0; i < longueur; i++){
            sendChr(msgBin[i], 8); // Envoyer chaque caractère
        }
        Serial.println("Envoyé."); // Notifier l'utilisateur une fois le message envoyé
        while(chrono < attente){}
        chrono = 0;
        digitalWrite(led, HIGH);
        }
    }
}

void deccon(int x){ // Conversion en code binaire
  for(int i = 7; i > - 1; i--){
    val[i] = x%2;
    x = x/2;
  }
}

void sendChr(int msg[], int len){ // Envoi de la série binaire val
  for(int i = 0; i < len; i++){
    
    while(chrono < attente){};
    chrono = 0;
    
    if(msg[i] == 0){
      digitalWrite(led, LOW);
      while(chrono < attente){};
      chrono = 0;
      digitalWrite(led, HIGH);
    }
    else if(msg[i] == 1){
      digitalWrite(led, HIGH);
      while(chrono < attente){};
      chrono = 0;
      digitalWrite(led, LOW);
    }
  }
  while(chrono < attente){};
  chrono = 0;
  digitalWrite(led, HIGH);
  delayMicroseconds(attente);
  digitalWrite(led, LOW);
  chrono = 0;
}
