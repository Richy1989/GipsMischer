/******************************************
*      HARDWARE DEFINITIONEN  Anfang        *  
*******************************************/
//  Pin Definition für Eingang Ardui3no:3 x Hall Sensor
#define AL                         13 // Eingang Armposition LINKS, pullup
#define AM                         12 // Eingang Armposition MITTE, pullup
#define AR                         11 // Eingang Armposition RECHTS, pullup

//  Pin Definition für Eingang Arduino:3 x GIPS_BECHER
#define BL                         9 // GIPS_BECHER_LINKS_PIN
#define BM                         8 // GIPS_BECHER_MITTE_PIN
#define BR                         7 // GIPS_BECHER_RECHTS_PIN

//  Pin Definition für NU
// #define ???                        6 // NU
// #define ???                        5 // NU

//  Pin Definition für Ausgang Piepser Arduino
#define TONE                       4  // Ausgang für Piepser


// Define rotary encoder pins (Arduino Mega 2560: Pins 2, 3, 18, 19, 20, 21 möglich)
// Pin Definition für Encoder mit Stützpunkten vom Flachbandkabel
#define ENCODER_B                  3  // Eingang pullup Interrupt 4 (Anzeigeprint FlBaKa Pin 18)
#define LCD_BASE_47                47 // Stützbunkt bei 20 poligem Stecker (Pin 47), Brücke zu Arduino Pin 3

#define ENCODER_A                  2  // Eingang pullup Interrupt 5 (Anzeigeprint FlBaKa Pin 17)
#define LCD_BASE_46                46 // Stützbunkt bei 20 poligem Stecker (Pin 46), Brücke zu Arduino Pin 2

//  Pin Definition für Ausgang LED Frontanzeige Arduino:3 x LED
#define LL                         0  // Ausgang Front LED LINKS
#define LM                         14 // Ausgang Front LED MITTE
#define LR                         15 // Ausgang Front LED RECHTS

//  Pin Definition für NU
// #define ???                        16 // NU
// #define ???                        17 // NU
// #define ???                        18 // NU
// #define ???                        19 // NU

//  Waage: Pin Definition für den I2C Bus bei HX711 (Interruptfähige Eingänge)
#define DATA_PIN                   20
#define CLOCK_PIN                  21 

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
#define LCD_BASE_30                30 // Eingang pullup Vcc (Anzeigeprint FlBaKa Pin 1)
                                      // Stützbunkt bei 20 poligem Stecker (Pin 30), Brücke zu Vcc
#define WAAGE_PIN                  31 // Eingang pullup, Pin Definition für die Taste: Waage (Anzeigeprint FlBaKa Pin 2)                                  
#define RS                         32 // Ausgang (Anzeigeprint FlBaKa Pin 3)
#define RW                         33 // Ausgang (Anzeigeprint FlBaKa Pin 4)
#define EN                         34 // Ausgang (Anzeigeprint FlBaKa Pin 5)
#define D0                         35 // Ausgang (Anzeigeprint FlBaKa Pin 6)
#define D1                         36 // Ausgang (Anzeigeprint FlBaKa Pin 7)
#define D2                         37 // Ausgang (Anzeigeprint FlBaKa Pin 8)
#define D3                         38 // Ausgang (Anzeigeprint FlBaKa Pin 9)
#define D4                         39 // Ausgang (Anzeigeprint FlBaKa Pin 10)
#define D5                         40 // Ausgang (Anzeigeprint FlBaKa Pin 11)
#define D6                         41 // Ausgang (Anzeigeprint FlBaKa Pin 12)
#define D7                         42 // Ausgang (Anzeigeprint FlBaKa Pin 13)
#define LCD_BASE_43                43 // Eingang pullup Poti, LCD Helligkeit (Anzeigeprint FlBaKa Pin 14)
                                      // Stützbunkt bei 20 poligem Stecker (Pin 43), Brücke zu Pot für LCD Kontrasteinstellung
#define I_O_PIN                    44 // Eingang pullup, Pin Definition für die Taste: I/O (Anzeigeprint FlBaKa Pin 15) 
#define ENTER_PIN                  45 // // Eingang pullup, Pin Definition für die Taste: Enter (Anzeigeprint FlBaKa Pin 16) 
#define LCD_BASE_48                48 // Eingang pullup GND (Anzeigeprint FlBaKa Pin 19)
                                      // Stützbunkt bei 20 poligem Stecker (Pin 48), Brücke zu GND
#define LCD_BASE_49                49 // Eingang pullup GND (Anzeigeprint FlBaKa Pin 20)
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
#define VR                         A11// WASSER_VENTIL_RECHTS_PIN; FlBaKa Pin 11 / Relais 9
#define VM                         A10// WASSER_VENTIL_MITTE_PIN; FlBaKa Pin 9 / Relais 7
#define AUSGANG_9                  A9 // Eingang A9 pullup VCC (Anzeigeprint FlBaKa Pin 1 und 2)
                                      // Stützbunkt bei 20 poligem Stecker (Pin 1 und 2), Brücke zu Vcc
#define ML                         A8 // GIPS_MOTOR_LINKS_PIN; FlBaKa Pin 4 / Relais 2
#define MM                         A7 // GIPS_MOTOR_MITTE_PIN; FlBaKa Pin 5 / Relais 3
#define MR                         A6 // GIPS_MOTOR_RECHTS_PIN; FlBaKa Pin 7 / Relais 5
#define WP                         A5 // WASSER_PUMPE_PIN; FlBaKa Pin 10 / Relais 8
#define RL                         A4 // GIPS_RUETTLER_LINKS_PIN; FlBaKa Pin 12 / Relais 10
#define RM                         A3 // GIPS_RUETTLER_MITTE_PIN; FlBaKa Pin 14 / Relais 12
#define RR                         A2 // GIPS_RUETTLER_RECHTS_PIN; FlBaKa Pin 16 / Relais 14
#define VL                         A1 // WASSER_VENTIL_LINKS_PIN; FlBaKa Pin 18 / Relais 16
#define AUSGANG_0                  A0 // Eingang A0 pullup GND (Anzeigeprint FlBaKa Pin 19 und 20)
                                      // Stützbunkt bei 20 poligem Stecker (Pin 19 und 20), Brücke zu GND
 // Ende Flachbandkabel 20 Pin zur Relaiskarte 

/******************************************
*      HARDWARE DEFINITIONEN  Ende        *  
*******************************************/


/******************************************
*      KONSTANTEN DEFINITIONEN  Anfang    *  
*******************************************/

#define LCD_CHARACTERS             16
#define LCD_ZEILEN                 2

#define EIN                        0
#define AUS                        1

#define ARM_LINKS                  0    // Pointer auf Array Motoren, Rüttler, Ventile
#define ARM_MITTE                  1
#define ARM_RECHTS                 2
#define ARM_NO_POS                 3
#define MAX_ARM_POS                3

#define MAX_DATEN_SATZ             3
#define MAX_MOTOREN                3
#define MAX_RUETTLER               3
#define MAX_VENTILE                3

#define FAST_INCREMENT             10


/******************************************
*      KONSTANTEN DEFINITIONEN  Ende      *  
*******************************************/