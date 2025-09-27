#pragma once

/******************************************
 *      INCLUDE DEFINITIONEN  Anfang        *
 *******************************************/
#include <HX711.h>
#include <Arduino.h>
//  Waage mit AD Wandler HX711
//  Library von Bogdan Necula

#include "LiquidCrystal.h"
//  LCD Anzeige
//  Library von arduino-libraries

#include <EEPROM.h>
// #include "IO.h"
// #include "service.h"
/******************************************
 *      INCLUDE DEFINITIONEN  Anfang        *
 *******************************************/

/******************************************
 *      HARDWARE DEFINITIONEN  Anfang        *
 *******************************************/

// #define ??? nur Ausgang (LED)      13

//  Pin Definition für Eingang Arduino:3 x Hall Sensor
#define AM 12 // Eingang Armposition MITTE, pullup
#define AR 11 // Eingang Armposition RECHTS, pullup
#define AL 1  // Eingang Armposition LINKS, pullup

// #define ???                        10 // NU

//  Pin Definition für Eingang Arduino:3 x GIPS_BECHER
#define BL 9 // GIPS_BECHER_LINKS_PIN
#define BM 8 // GIPS_BECHER_MITTE_PIN
#define BR 7 // GIPS_BECHER_RECHTS_PIN

//  Pin Definition für NU
// #define ???                        6 // NU
// #define ???                        5 // NU

//  Pin Definition für Ausgang Piepser Arduino
#define TONE_PIN 4 // Ausgang für Buzzer, Piezo Lautsprecher

//  Pin Definition für NU
// #define ???                        3  // NU
// #define ???                        0  // NU

//  Pin Definition für Ausgang LED Frontanzeige Arduino:3 x LED
#define LL 14 // Ausgang Front LED LINKS
#define LM 15 // Ausgang Front LED MITTE
#define LR 16 // Ausgang Front LED RECHTS

//  Pin Definition für NU
// #define ???                        17 // NU

// Define rotary encoder pins (Arduino Mega 2560: Pins 2, 3, 18, 19, 20, 21 möglich)
// Pin Definition für Encoder mit Stützpunkten vom Flachbandkabel
#define ENCODER_A 3	   // Eingang pullup, Interrupt 5 (Anzeigeprint FlBaKa Pin 17)
#define LCD_BASE_46 46 // Stützbunkt bei 20 poligem Stecker (Pin 46), Brücke zu Arduino Pin 2

#define ENCODER_B 2	   // Eingang pullup, Interrupt 4 (Anzeigeprint FlBaKa Pin 18)
#define LCD_BASE_47 47 // Stützbunkt bei 20 poligem Stecker (Pin 47), Brücke zu Arduino Pin 3

//  Pin Definition für Interrupt Reserve
#define INTERRUPT_RESERVE1 18 // Interrupt 3
#define INTERRUPT_RESERVE2 19 // Interrupt 2

//  Waage: Pin Definition für den I2C Bus bei HX711 (Interruptfähige Eingänge)
#define DATA_PIN 20
#define CLOCK_PIN 21

//  Pin Definition für NU
// #define ???                        22 // NU
// #define ???                        23 // NU
// #define ???                        24 // NU
// #define ???                        25 // NU
// #define ???                        26 // NU
// #define ???                        27 // NU
// #define ???                        28 // NU
// #define ???                        29 // NU

// Anfang Flachbandkabel 20 Pin zur Bedieneinheit
#define LCD_BASE_30 30 // Eingang pullup Vcc (Anzeigeprint FlBaKa Pin 1)
					   // Stützbunkt bei 20 poligem Stecker (Pin 30), Brücke zu Vcc
#define WAAGE_PIN 31   // Eingang pullup, Pin Definition für die Taste: Waage (Anzeigeprint FlBaKa Pin 2)
#define RS 32		   // Ausgang (Anzeigeprint FlBaKa Pin 3)
#define RW 33		   // Ausgang (Anzeigeprint FlBaKa Pin 4)
#define EN 34		   // Ausgang (Anzeigeprint FlBaKa Pin 5)
#define D0 35		   // Ausgang (Anzeigeprint FlBaKa Pin 6)
#define D1 36		   // Ausgang (Anzeigeprint FlBaKa Pin 7)
#define D2 37		   // Ausgang (Anzeigeprint FlBaKa Pin 8)
#define D3 38		   // Ausgang (Anzeigeprint FlBaKa Pin 9)
#define D4 39		   // Ausgang (Anzeigeprint FlBaKa Pin 10)
#define D5 40		   // Ausgang (Anzeigeprint FlBaKa Pin 11)
#define D6 41		   // Ausgang (Anzeigeprint FlBaKa Pin 12)
#define D7 42		   // Ausgang (Anzeigeprint FlBaKa Pin 13)
#define LCD_BASE_43 43 // Eingang pullup Poti, LCD Helligkeit (Anzeigeprint FlBaKa Pin 14)
					   // Stützbunkt bei 20 poligem Stecker (Pin 43), Brücke zu Pot für LCD Kontrasteinstellung
#define I_O_PIN 44	   // Eingang pullup, Pin Definition für die Taste: I/O (Anzeigeprint FlBaKa Pin 15)
#define ENTER_PIN 45   // // Eingang pullup, Pin Definition für die Taste: Enter (Anzeigeprint FlBaKa Pin 16)
// #define LCD_BASE_46                46 // Stützbunkt bei 20 poligem Stecker (Pin 46), Brücke zu Arduino Pin 19
// #define LCD_BASE_47                47 // Stützbunkt bei 20 poligem Stecker (Pin 47), Brücke zu Arduino Pin 18
#define LCD_BASE_48 48 // Eingang pullup GND (Anzeigeprint FlBaKa Pin 19)
					   // Stützbunkt bei 20 poligem Stecker (Pin 48), Brücke zu GND
#define LCD_BASE_49 49 // Eingang pullup GND (Anzeigeprint FlBaKa Pin 20)
					   // Stützbunkt bei 20 poligem Stecker (Pin 20), Brücke zu GND
					   // Ende Flachbandkabel 20 Pin zur Bedieneinheit

//  Pin Definition für NU
// #define ???                        50 // NU
// #define ???                        51 // NU
// #define ???                        52 // NU
// #define ???                        53 // NU

// #define ???                        A15 // NU
// #define ???                        A14 // NU
// #define ???                        A13 // NU
// #define ???                        A12 // NU

// Anfang Flachbandkabel 20 Pin zur Relaiskarte
//  Ausgänge Pin Definition für Relais Ausgangskarte (24 VDC): 3 x GIPS_MOTOR, 3 x WASSER_VENTIL, 1 x Wasserpumpe
//  Ausgänge Pin Definition für Relais Ausgangskarte (24 VAC): 3 x GIPS_RUETTLER
#define RELAIS_VL A15			 // WASSER_VENTIL_LINKS_PIN; FlBaKa Pin 17 / Relais 15
#define RELAIS_RM A14			 // GIPS_RUETTLER_MITTE_PIN; FlBaKa Pin 15 / Relais 13
#define RELAIS_VR A13			 // WASSER_VENTIL_RECHTS_PIN; FlBaKa Pin 13 / Relais 11
#define RELAIS_WP /*defekt*/ A12 // WASSER_PUMPEN_PIN; FlBaKa Pin 3 / Relais 1
// #define defekt                     A11// WASSER_VENTIL_RECHTS_PIN; FlBaKa Pin 11 / Relais 9
#define RELAIS_VM A10 // WASSER_VENTIL_MITTE_PIN; FlBaKa Pin 9 / Relais 7
#define RELAIS_ML A8  // GIPS_MOTOR_LINKS_PIN; FlBaKa Pin 4 / Relais 2
#define RELAIS_MM A7  // GIPS_MOTOR_MITTE_PIN; FlBaKa Pin 5 / Relais 3
#define RELAIS_MR A6  // GIPS_MOTOR_RECHTS_PIN; FlBaKa Pin 7 / Relais 5
// #define defekt                    A5 // WASSER_PUMPE_PIN; FlBaKa Pin 10 / Relais 8
#define RELAIS_RL A4 // GIPS_RUETTLER_LINKS_PIN; FlBaKa Pin 12 / Relais 10
// #define defekt                    A3 // GIPS_RUETTLER_MITTE_PIN; FlBaKa Pin 14 / Relais 12
#define RELAIS_RR A2 // GIPS_RUETTLER_RECHTS_PIN; FlBaKa Pin 16 / Relais 14
// #define defekt                    A1 // WASSER_VENTIL_LINKS_PIN; FlBaKa Pin 18 / Relais 16

#define AUSGANG_9 A9 // Eingang A9 pullup VCC (Anzeigeprint FlBaKa Pin 1 und 2)
					 // Stützbunkt bei 20 poligem Stecker (Pin 1 und 2), Brücke zu Vcc
#define AUSGANG_0 A0 // Eingang A0 pullup GND (Anzeigeprint FlBaKa Pin 19 und 20)
					 // Stützbunkt bei 20 poligem Stecker (Pin 19 und 20), Brücke zu GND
// Ende Flachbandkabel 20 Pin zur Relaiskarte

/******************************************
 *      HARDWARE DEFINITIONEN  Ende        *
 *******************************************/

/******************************************
 *      KONSTANTEN DEFINITIONEN  Anfang    *
 *******************************************/

#define LCD_CHARACTERS 16
#define LCD_ZEILEN 2

#define EIN 0
#define AUS 1

#define ARM_LINKS 0
#define ARM_MITTE 1
#define ARM_RECHTS 2
#define ARM_NO_POS 3

#define ARM_ANZAHL 3  //  0 bis 2, also 3 Arm-Positionen

#define BECHER_LINKS 0
#define BECHER_MITTE 1
#define BECHER_RECHTS 2
#define NO_BECHER 3

// für die Variable daten vom Typ Datensatz
#define MAX_DATEN_SATZ 3 // 0 bis 2, also 3 Datensätze, die in den EEPROM gespeichert werden
// pro Datensatz jeweils einer Armposition zugeordnet
#define MAX_STRING 13 // 0 bis 12, also 13 Charakters in der Überschrift 
#define MAX_GEWICHTANZAHL 11 // 0 bis 10, also 11 --> 10 Gewichte und 1 Mischungsverhältnis

#define MAX_POSITIONEN 11 // 0 bis 11, also 12 Positionen (int), zum Positionen Anzeigen und Scrollen
// Pos 1: in der ersten Zeile  -->  Überschrift (String) - 13 Charakters (bleibt in der 1 Zeile fixiert)
// Pos 2: einmalig in der zweiten Zeile (scrollen)  --> LCD Anzeige: "-- xxx g  yyy ml"
//                                       Gipsgewicht (unsigned int), Wassergewicht (unsigned int) - Mischungsverhältnis
// Pos 3 bis 5: (3x) in der zweiten Zeile (scrollen)  --> LCD Anzeige: "-- xxx g fixiert"
//                                                        das Gesamtgewicht jeweils um 160 g des Gundgewichtes erhöht
// Pos 6 bis 10: (5x) in der zweiten Zeile (scrollen)  --> LCD Anzeige: "-- xxx g eingabe"
//                                                        freie Gesamtgewicht Eingabe
// Position 11 - LCD Anzeige: " Gipsentleerung "
// Position 12 - LCD Anzeige: " Wasserentnahme "
// Die Anzeige der Positionen am LCD erfolgt numerisch zweistellig über die ersten beiden links stehenden Charakters
// gefolgt von einem Leerzeichen, Beispiel: 01 , 02 , ..., "10 ", "11 ", "12 "

#define FAST_INCREMENT 10

// Arduino Mega hat 4 KB (4096 bytes) EEPROM Zellen, also 0 bis 4095
#define MAX_EEPROM_ADRESSE 4095 

// Sicherheitsabstand der einzellnen Adressen beim Schreiben auf den EEPROM 
#define EEPROM_ADRESSABSTAND 100 //  Abstand in Byts der Adressen der zu speichernden Variablen im EEPROM

// Maximal messbares Gewicht in Gramm
#define MAX_GEWICHT 3000

// Wartezeit in ms
#define WAIT_TIME_LED  750  // LED Hell- und Dunkelzeit beim service blinken 
#define WAIT_TIME_2   3000  // ersten 3 Sekunden anzeige
#define WAIT_TIME_3   5000  // zweiten 2 Sekunden anzeige
#define WAIT_TIME_4   8000  // dritten 3 Sekunden anzeige

// Benennung der Service Test Routinen
#define EEPROM_TEST 		0
#define LED_TEST 			1
#define BECHER_TEST			2
#define TON_TEST 			3
#define ARM_TEST		 	4
#define WAAGE_KALIBRIERUNG 	5
#define WAAGE_TEST 			6
#define RELAIS_TEST			7
#define DATA_RESET		    8
#define OUT_OF_RANGE	    255 // Wert außerhalb des Bereichs der Test Routinen

// Benennung der Melodien
#define MELODIE_ANFANG		0
#define MELODIE_ENDE		1
#define MELODIE_OK			2
#define MELODIE_ENTER		3
#define MELODIE_FEHLER		4

// Sonderzeichen definierem
#define SMILEY				0



/******************************************
 *      KONSTANTEN DEFINITIONEN  Ende      *
 *******************************************/

/******************************************
 *      VARIABLEN DEFINITIONEN  Anfang     *
 *******************************************/
// Create HX711 instance
extern HX711 scale;

// Create LCD instance
extern LiquidCrystal lcd;

extern bool leer;
extern bool voll;

extern float Leergew_einheiten;
extern float Eichgew_einheiten;
extern float Gewicht;
extern float Gewicht_alt;
extern float Korrekturfaktor;

extern unsigned int armposition;
extern unsigned int armposition_alt;

extern unsigned int Anzeige;
extern unsigned int Anzeige_alt;

// steuerung des Encoder Interrupts. Nur wenn der Encoder in Funktion sein soll
extern bool on_off_encoder;

// zum Abbruch einet Funktion
extern bool abbruch;

// Zahlenwert im Encoder
extern  int Encoder_count_neu;
extern  int Encoder_count_alt;

// definition des Encoder ausgabewertes Minimum und Maximum in der Variablen counter
extern  int max_counter;
extern  int min_counter;

// definition der Encoder Zeitabständen in Mikrosekunden
extern unsigned long _lastIncReadTime;
extern unsigned long _lastDecReadTime;
extern unsigned long _pauseLength;

//
extern const unsigned int relais[];
//  Array für die Relais Pins, die in der Reihenfolge der Relais angeordnet sind
//  Relais 1 bis 9 sind Motoren, Rüttler und Ventile, Relais 10 ist die Wasserpumpe
//  Relais 11 bis 16 sind defekt, da sie nicht mehr auf der Relaiskarte vorhanden sind
extern const unsigned int anzahlrelais;

extern unsigned long start_time;
extern	unsigned long wait_time; //  Wartezeit in Millisekunden

	// Array für die Test Routinen
//extern const int test_routinen[]; 
	// berechnet die Anzahl der test_routinen
//extern const int anzahl_tests;

extern const unsigned int positiveTones[]; // Positive Töne
extern const unsigned int negativeTones[]; // Negative Töne

extern unsigned int start;

//  Array für die Test Routinen
extern const unsigned int test_routinen[];
// berechnet die Anzahl der test_routinen
extern const unsigned int anzahl_tests;

//  Array mit Zeichen für die Texteingabe (Großbuchstaben, Zahlen, Sonderzeichen)
extern const char texteingabe[]; 
// berechnet die Anzahl der Characters der Texteingabe
extern const unsigned int anzahl_texteingabe;

// Structure Definition
struct datensatz    
{						 
//	String ueberschrift; // 13 Charakters (0 bis 12) pro Armposition
	char ueberschrift[MAX_STRING]; // 13 Charakters (0 bis 12) pro Armposition
	unsigned int gewicht[MAX_GEWICHTANZAHL]; // Array von 11 Gewichten (0 bis 10), für die Gewichte pro Armposition
}; // Structure variable

// Array of structures (0 bis 2) also 3 für die Armpositionen
extern datensatz daten[MAX_DATEN_SATZ]; 
// berechnet die Anzahl der Byts der Variablen daten
extern const unsigned int anzahl_daten;

extern byte smiley[8];  //erstellt Zeichen Smiley


void init_data();

/******************************************
 *      VARIABLEN DEFINITIONEN  Ende        *
 *******************************************/
