#include "setup.h"

HX711 scale;
LiquidCrystal lcd(RS, RW, EN, D0, D1, D2, D3, D4, D5, D6, D7); // LCD Initialisierung

unsigned int Anzeige_alt = 255;
unsigned int Anzeige;

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
int Encoder_count_store = 0; // Sicherung des Encoderwertes

// definition des Encoder ausgabewertes Minimum und Maximum in der Variablen counter
int max_counter = 500;
int min_counter = 0;

// Arm - Positionsspeicherung
unsigned int armposition;
unsigned int armposition_alt;

// steuerung des Encoder Interrupts. Nur wenn der Encoder in Funktion sein soll
bool on_off_encoder = false;

// zum Abbruch einet Funktion
 bool abbruch = false;

// definition der Encoder Zeitabständen in Mikrosekunden
unsigned long _lastIncReadTime = micros();
unsigned long _lastDecReadTime = micros();
unsigned long _pauseLength = 25000;

const unsigned int relais[] = {
	RELAIS_ML, RELAIS_MM, RELAIS_MR, // Motoren Relais Pins
	RELAIS_RL, RELAIS_RM, RELAIS_RR, // Rüttler Relais Pins
	RELAIS_VL, RELAIS_VM, RELAIS_VR, // Ventile Relais Pins
	RELAIS_WP						 // Wasserpumpe Relais Pin
};

//  Array für die Relais Pins, die in der Reihenfolge der Relais angeordnet sind
//  Relais 1 bis 9 sind Motoren, Rüttler und Ventile, Relais 10 ist die Wasserpumpe
//  Relais 11 bis 16 sind defekt, da sie nicht mehr auf der Relaiskarte vorhanden sind
const unsigned int anzahlrelais = sizeof(relais) / sizeof(relais[0]); // Anzahl der Relais berechnet

const unsigned int positiveTones[] = {1000, 1200, 1400, 1600}; // Positive Töne
const unsigned int negativeTones[] = {500, 300, 200};		   // Negative Töne

unsigned long start_time;
unsigned long wait_time = WAIT_TIME_1; // Wartezeit in Millisekunden

// Array für die Test Routinen
const unsigned int test_routinen[] = {EEPROM_TEST, LED_TEST, BECHER_TEST, TON_TEST,
							          ARM_TEST, WAAGE_KALIBRIERUNG, WAAGE_TEST, RELAIS_TEST};
// berechnet die Anzahl der test_routinen
const unsigned anzahl_tests = sizeof(test_routinen) / sizeof(test_routinen[0]);

datensatz daten[MAX_ARM_POS] = {}; // Array of structures (0 bis 2) für die Armpositionen

const unsigned int anzahldaten = sizeof(daten) / sizeof(daten[0]); // Anzahl der Relais berechnet

void init_data()
{
 daten[0].ueberschrift = "Gips A"; // Beispiel Initialisierung der Überschrift
 daten[0].gesamt_gewicht = 0;  	// Beispiel Initialisierung des Gesamtgewichts
 daten[0].mischverhaeltnis = 0; 	// Beispiel Initialisierung der Mischverhältnis
 daten[0].gewicht[0] = 0;  		// Beispiel Initialisierung der Gewichte
 daten[0].gewicht[1] = 0;
 daten[0].gewicht[2] = 0;
 daten[0].gewicht[3] = 0;
 daten[0].gewicht[4] = 0;	
}
