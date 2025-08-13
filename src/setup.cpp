#include "setup.h"

HX711 scale;
LiquidCrystal lcd(RS, RW, EN, D0, D1, D2, D3, D4, D5, D6, D7); // LCD Initialisierung
int Anzeige_alt = 255;
int Anzeige;

bool leer = false;
bool voll = false;

float Leergew_einheiten;
float Eichgew_einheiten;
float Gewicht;
float Gewicht_alt;
float Korrekturfaktor;

// Zahlenwert im Encoder
int Encoder_count_neu = 0;
int Encoder_count_alt = -1;

// definition des Encoder ausgabewertes Minimum und Maximum in der Variablen counter
int max_counter = 500;
int min_counter = 0;

// steuerung des Encoder Interrupts. Nur wenn der Encoder in Funktion sein soll
bool on_off_encoder = false;

// definition der Encoder Zeitabständen in Mikrosekunden
unsigned long _lastIncReadTime = micros();
unsigned long _lastDecReadTime = micros();
unsigned long _pauseLength = 25000;

const int relais[] = {RELAIS_ML, RELAIS_MM, RELAIS_MR,  // Motoren Relais Pins
                      RELAIS_RL, RELAIS_RM, RELAIS_RR,  // Rüttler Relais Pins
                      RELAIS_VL, RELAIS_VM, RELAIS_VR,  // Ventile Relais Pins   
                      RELAIS_WP                         // Wasserpumpe Relais Pin 
                    };

//  Array für die Relais Pins, die in der Reihenfolge der Relais angeordnet sind
//  Relais 1 bis 9 sind Motoren, Rüttler und Ventile, Relais 10 ist die Wasserpumpe
//  Relais 11 bis 16 sind defekt, da sie nicht mehr auf der Relaiskarte vorhanden sind
const int anzahlrelais = sizeof(relais) / sizeof(relais[0]); // Anzahl der Relais berechnet 

const int positiveTones[] = {1000, 1200, 1400, 1600}; // Positive Töne
const int negativeTones[] = {500, 300, 200};          // Negative Töne

int start = 1;