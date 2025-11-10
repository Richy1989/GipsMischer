#include "setup.h"

HX711 scale;
LiquidCrystal lcd(RS, RW, EN, D0, D1, D2, D3, D4, D5, D6, D7); // LCD Initialisierung

unsigned int Anzeige_alt = 255;
unsigned int Anzeige;

float Leergew_einheiten;
float Eichgew_einheiten;
float Gewicht;
float Gewicht_alt;
float teilgewicht_gips;
float teilgewicht_h2o;
float gesamtgewicht;
float gewicht_waagschale;
float teilgewicht_h2o_waagschale;
float Korrekturfaktor;

// Zahlenwert im Encoder
int Encoder_count_neu = 0;
int Encoder_count_alt = -1;
int Encoder_count_store;

// definition des Encoder ausgabewertes Minimum und Maximum in der Variablen counter
int max_counter = 500;
int min_counter = 0;

// Arm - Positionsspeicherung
unsigned int armposition;
unsigned int armposition_alt;

// steuerung des Encoder Interrupts. Nur wenn der Encoder in Funktion sein soll
bool on_off_encoder = false;

// zum Abbruch einer Funktion
bool abbruch = false;

// definition der Encoder Zeitabständen in Mikrosekunden
unsigned long _lastIncReadTime = micros();
unsigned long _lastDecReadTime = micros();
unsigned long _pauseLength = 25000;  // 25 ms Pause zwischen den Encoder Impulsen

//  Array für die Adressierung der RelaisPins auf dem Arduino Mega2560
//  Motor Links (A8), Motor Mitte (A7), Motor Rechts (A6)
//  Rüttler Links (A4), Rüttler Mitte (A14), Rüttler Rechts (A2)
//  Ventil Links (A15), Ventil Mitte A10), Ventil Rechts (A13)
//  Wasserpumpe (A12)
const unsigned int relais[] = {
	RELAIS_ML, RELAIS_MM, RELAIS_MR, // Motoren Relais     00, 01, 02
	RELAIS_RL, RELAIS_RM, RELAIS_RR, // Rüttler Relais     03, 04, 05
	RELAIS_VL, RELAIS_VM, RELAIS_VR, // Ventile Relais     06, 07, 08
	RELAIS_WP						 // Wasserpumpe Relais 09
};

//  Berechnet die Anzahl der Relais;
//  Relais = Anzahl Byts der Variablen relais geteilt durch die Bytegröße eines Arrayelements
const unsigned int anzahlrelais = sizeof(relais) / sizeof(relais[0]); // Anzahl der Relais berechnet

unsigned long start_time;
unsigned long start_time_LED;		//  LED Blinkzeit
unsigned long start_time_armpos;    //  Zeit zu Alarm für falsche Armposition


// Array für die Test Routinen
const unsigned int test_routinen[] = {EEPROM_TEST, LED_TEST, BECHER_TEST,
									  TON_TEST, ARM_TEST, WAAGE_KALIBRIERUNG,
									  WAAGE_TEST, RELAIS_TEST, DATA_RESET};
// berechnet die Anzahl der test_routinen in der Variablen test_routinen
// test_routinen = Anzahl Byts der Variablen test_routinen geteilt durch die Bytegröße eines Arrayelements
const unsigned int anzahl_tests = sizeof(test_routinen) / sizeof(test_routinen[0]);

//  Array mit Zeichen für die Texteingabe
const char texteingabe[] = {
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'-',
	'+',
	'*',
	' ',
}; // Ende mit Nullzeichen
// berechnet Anzahl der Characters der Texteingabe, -1 wegen dem Nullzeichen
const unsigned int anzahl_texteingabe = sizeof(texteingabe) - 1;

// POS 01: Überschrift Eingabe

// POS 02: Gewicht[0]:Referenzgewicht Gips   (eingeben)
// POS 02: Gewicht[1]:Referenzgewicht Wasser (eingeben)

// POS 03: Gewicht[2]:Gipsgewicht fixiert (160g) /  (130g)
// POS 04: Gewicht[3]:Gipsgewicht fixiert (160g + 160g = 320g) / (130g + 130g = 260g)
// POS 05: Gewicht[4]:Gipsgewicht fixiert (160g + 160g + 160g = 480g) / (130g + 130g + 130g = 390g)
// POS 06: Gewicht[5]:Gipsgewicht fixiert (160g + 160g + 160g + 160g = 640g) / (130g + 130g + 130g + 130g = 520g)

// POS 07: Gewicht[6]:Gipsgewicht variabel  (eingeben)
// POS 08: Gewicht[7]:Gipsgewicht variabel  (eingeben)
// POS 09: Gewicht[8]:Gipsgewicht variabel  (eingeben)
// POS 10: Gewicht[9]:Gipsgewicht variabel  (eingeben)

// POS 11: Gipsentleerung (keine Gewicht, keine Eingabe)
// POS 12: Wasserentnahme (keine Gewicht, keine Eingabe)

// Array of structures (0 bis 2), also 3 für die Armpositionen
datensatz daten[MAX_DATEN_SATZ] = {};
// berechnet die Anzahl der Byts der Variablen daten, zur Berechnung der EEPROM Speicheradressen
const unsigned int anzahl_daten = sizeof(daten); //  Bytegröße der Variablen daten 13+22 = 35*3 = 105 Byts)

//  Mischungsverhältnisse (in Prozent als 0,xxx Zahl) zur Berechnung der Teilmengen aus einer Gesamtmenge
//  unsigned long gipsverhaeltnis[MAX_DATEN_SATZ];	 // Mischungsverhältnis Gips zu Gesamtmenge in Prozent (0,xxx Zahl)
unsigned long gips_verhaeltnis[MAX_DATEN_SATZ]; // Mischungsverhältnis Wasser zu Gesamtmenge in Prozent (0,xxx Zahl)
// unsigned long gesamtgewicht[MAX_DATEN_SATZ];	 // Gesamtgewicht Gips und Wasser der Referenzmenge

// Array of Becher (0 bis 2) also 3 für 
unsigned int becher[BECHER_ANZAHL];

byte smiley[8] = {
	// erstellt Zeichen Smiley --> Nr.: 0
	B00000,
	B10001,
	B00000,
	B00000,
	B10001,
	B01110,
	B00000};

byte herz[8] = { // erstellt Zeichen Herz --> Nr.: 1
	B00000,
	B01010,
	B11111,
	B11111,
	B01110,
	B00100,
	B00000,
	B00000};

byte cursor[8] = { // erstellt Zeichen Cursor --> Nr.: 2
	B10000,
	B11000,
	B11100,
	B11111,
	B11111,
	B11100,
	B11000,
	B10000};

byte rechteck[8] = { // erstellt Zeichen Rechteck --> Nr.: 3
	B11111,
	B10001,
	B10001,
	B10001,
	B10001,
	B10001,
	B10001,
	B11111};
