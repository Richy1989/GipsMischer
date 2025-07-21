#include "HX711.h"
//  Waage mit AD Wandler HX711
//  Library von Bogdan Necula

#include "LiquidCrystal.h"
//  LCD Anzeige
//  Library von arduino-libraries

#include "setup.h"

//Eine neue Test Zeile

/******************************************
 *      VARIABLEN DEFINITIONEN  Anfang     *
 *******************************************/
// Create HX711 instance
HX711 scale;

// Create LCD instance
LiquidCrystal lcd(RS, RW, EN, D0, D1, D2, D3, D4, D5, D6, D7); // LCD Initialisierung

bool leer = false;
bool voll = false;

float Leergew_einheiten;
float Eichgew_einheiten;
float Gewicht;
float Korrekturfaktor;

unsigned int Armposition;

/*
unsigned int Datensatz[MAX_ARM_POS] [LCD_CHARACTERS] [MAX_DATEN_SATZ] [MAX_DATEN_SATZ];
          // Variablenname maximale
          //           Anzahl der Armpstellungen (3)
          //                         Überschrift freuer Text, Gipssorte (16)
          //                                          Gesamtgewicht (1)
          //                                              Mischungsverhältnis (1)
*/

unsigned int Ueberschriften[MAX_ARM_POS][LCD_CHARACTERS];
unsigned int Gesamt_Gewichte[MAX_ARM_POS][MAX_DATEN_SATZ];
unsigned int Mischverhaeltnise[MAX_ARM_POS][MAX_DATEN_SATZ];

unsigned int Motoren[MAX_MOTOREN] = {ML, MM, MR}; // über Armpositoion adressieren
unsigned int Ruettler[MAX_ARM_POS] = {RL, RM, RR};
unsigned int Ventile[MAX_ARM_POS] = {VL, VM, VR};

int Encoder_count_neu = 0;
int Encoder_count_alt = -1;
// steuerung des Encoder Interrupts. Nur wenn der Encoder in Funktion sein soll
bool on_off_read_encoder = true;
// definition des Encoder ausgabewertes Minimum und Maximum in der Variablen counter
int max_counter = 500;
int min_counter = 0;

unsigned long _lastIncReadTime = micros();
unsigned long _lastDecReadTime = micros();
unsigned long _pauseLength = 25000;

int start = 1;

/******************************************
 *      VARIABLEN DEFINITIONEN  Ende        *
 *******************************************/

void read_encoder()
{
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent
  // Erklärung Youtube: How to use a Rotary Encoder with an Arduino - CODE EXPLAINED!
  // https://www.youtube.com/watch?v=fgOfSHTYeio

  static uint8_t old_AB = 3;                                                               // Lookup table index
  static int8_t encval = 0;                                                                // Encoder value
  static const int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Lookup table
  
  // nur wenn Encoder freigeschalten (true), soll interrupt Routine arbeiten
  if (on_off_read_encoder)
  {               
    old_AB <<= 2; // Remember previous state

    if (digitalRead(ENCODER_A))
      old_AB |= 0x02; // Add current state of pin A
    if (digitalRead(ENCODER_B))
      old_AB |= 0x01; // Add current state of pin B

    encval += enc_states[(old_AB & 0x0f)]; // Abschneiden der vorderen 8 Bits

    // Update counter if encoder has rotated a full indent, that is at least 4 steps
    if (encval > 3)
    {                      // Four steps forward, zählt AUFwärts
      int changevalue = 1; // Incrementeinheit ist 1

      if ((micros() - _lastIncReadTime) < _pauseLength)
        changevalue = FAST_INCREMENT; // Incrementeinheit wird auf 10 erhöht

      Encoder_count_neu += changevalue; // incremet counter um 1 oder 10; Encoder_count_neu = Encoder_count_neu + changevalue
      if (Encoder_count_neu > max_counter)
        Encoder_count_neu = max_counter; //  Korrektur, decremet counter um 1 oder 10

      _lastIncReadTime = micros();
      encval = 0;
    } // end if (encval > 3)
    else if (encval < -3)
    {                       // Four steps backward, zählt ABwärts
      int changevalue = -1; // Decrementeinheit ist -1

      if ((micros() - _lastDecReadTime) < _pauseLength)
        changevalue = FAST_INCREMENT * changevalue; // Decrementeinheit wird auf -10 erhöht

      Encoder_count_neu += changevalue; // Decremet counter um 1 oder 10; Encoder_count_neu = Encoder_count_neu + changevalue
      if (Encoder_count_neu < min_counter)
        Encoder_count_neu = min_counter; // Korrektur, increment counter

      _lastDecReadTime = micros();
      encval = 0;
    } // end else if(encval < -3)
  } // end if (on_off_read_encoder)
} // end read_encoder() **********************************************************************

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("jetzt gehts los");

  // Eingänge zur Armposition Erkennung:
  pinMode(AL, INPUT); // Eingang Armposition LINKS
  pinMode(AM, INPUT); // Eingang Armposition MITTE
  pinMode(AR, INPUT); // Eingang Armposition RECHTS

  // Eingänge zur Gipsbecher Erkennung:
  pinMode(BL, INPUT_PULLUP); // Eingang Armposition LINKS, pullup
  pinMode(BM, INPUT_PULLUP); // Eingang Armposition MITTE, pullup
  pinMode(BR, INPUT_PULLUP); // Eingang Armposition RECHTS, pullup

  // Ausgang zur Gipsbecher Erkennung:
  pinMode(TONE, OUTPUT); // Ausgang zur Tone Erzeugung

  // Eingänge zur ENCODER Erkennung:
  pinMode(ENCODER_B, INPUT_PULLUP); // Eingang pullup Interrupt 4 (Anzeigeprint FlBaKa Pin 18)
  pinMode(ENCODER_A, INPUT_PULLUP); // Eingang pullup Interrupt 5 (Anzeigeprint FlBaKa Pin 17)

  // Interruptroutinen zuweisen
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), read_encoder, CHANGE);

  // Ausgang zur Front LED:
  pinMode(LL, OUTPUT); // Ausgang zur LED LINKS
  pinMode(LM, OUTPUT); // Ausgang zur LED MITTE
  pinMode(LR, OUTPUT); // Ausgang zur LED RECHTS

  // Eingang Ausgang für I2C Bus bei HX711 (Interruptfähige Eingänge)
  // INPUT oder OUTPUT wird durch Library HX711.h gesetzt
  // pinMode(DATA_PIN, OUTPUT); // Daten PIN
  // pinMode(CLOCK_PIN, OUTPUT); // Clock PIN

  pinMode(LCD_BASE_30, INPUT_PULLUP); // Eingang pullup Vcc (Anzeigeprint FlBaKa Pin 1)
  pinMode(WAAGE_PIN, INPUT_PULLUP);   // Eingang pullup (Anzeigeprint FlBaKa Pin 2)

  // Ausgänge für LCD bei LiquidCrysta
  // OUTPUT wird durch Library LiquidCrystal.h gesetzt

  pinMode(LCD_BASE_43, INPUT_PULLUP); // Eingang pullup Poti, LCD Helligkeit (Anzeigeprint FlBaKa Pin 14)
  pinMode(I_O_PIN, INPUT_PULLUP);     // Eingang pullup (Anzeigeprint FlBaKa Pin 15)
  pinMode(ENTER_PIN, INPUT_PULLUP);   // Eingang pullup (Anzeigeprint FlBaKa Pin 16)
  pinMode(LCD_BASE_46, INPUT_PULLUP); // Eingang pullup Pin 3 Interrupt 5 (Anzeigeprint FlBaKa Pin 17)
  pinMode(LCD_BASE_47, INPUT_PULLUP); // Eingang pullup Pin 2 Interrupt 4 (Anzeigeprint FlBaKa Pin 18)
  pinMode(LCD_BASE_48, INPUT_PULLUP); // Eingang pullup GND (Anzeigeprint FlBaKa Pin 19)
  pinMode(LCD_BASE_49, INPUT_PULLUP); // Eingang pullup GND (Anzeigeprint FlBaKa Pin 20)

  // Ausgänge für Relais für: Motoren, Rüttler, Ventike und Wasserpumpe:
  pinMode(VR, OUTPUT);              // Ausgang 11
  pinMode(VM, OUTPUT);              // Ausgang 10
  pinMode(AUSGANG_9, INPUT_PULLUP); // A9 wird Eingang pullup Vcc (Relaisprint FlBaKa Pin 1 und 2)
  pinMode(ML, OUTPUT);              // Ausgang A8
  pinMode(MM, OUTPUT);              // Ausgang A7
  pinMode(MR, OUTPUT);              // Ausgang A6
  pinMode(WP, OUTPUT);              // Ausgang A5
  pinMode(RL, OUTPUT);              // Ausgang A4
  pinMode(RM, OUTPUT);              // Ausgang A3
  pinMode(RR, OUTPUT);              // Ausgang A2
  pinMode(VL, OUTPUT);              // Ausgang A1
  pinMode(AUSGANG_0, INPUT_PULLUP); // A0 wird Eingang pullup GND (Relaisprint FlBaKa Pin 19 und 20)

  // Relais ausschalten:
  digitalWrite(VR, AUS);
  digitalWrite(VM, AUS);
  digitalWrite(ML, AUS);
  digitalWrite(MM, AUS);
  digitalWrite(MR, AUS);
  digitalWrite(WP, AUS);
  digitalWrite(RL, AUS);
  digitalWrite(RM, AUS);
  digitalWrite(RR, AUS);
  digitalWrite(VL, AUS);


  // Initialisierung LCD
  lcd.begin(LCD_CHARACTERS, LCD_ZEILEN); // 16 Charakters, 2 Zeilen

  // Initialisierung Waage
  scale.begin(DATA_PIN, CLOCK_PIN); // dataPin =20, clockPin = 21, gain = 128 / Interruptfähige Pins

  // attach interrupts
  // attachInterrupt(digitalPinToInterrupt(ENCODER_A), read_encoder, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(ENCODER_B), read_encoder, CHANGE);

  // WeightArm weightArms[3];

  // void showArm(WeightArm arm)
  //{
  //  lcd.println(arm.getDisplayTextLine1());
  //  lcd.println(arm.getDisplayTextLine2());
  // }

} // end setup **********************************************************************

void loop()
{
  // int armIndex = 0; // Index for the current arm being processed
  // showArm(weightArms[armIndex]); // Display the first arm's information

  // int localGewicht = 50000;
  // int zeilenIndex = 1;

  // weightArms[armIndex].setWeight(zeilenIndex, localGewicht); // Set the weight for the first arm

  // int sum = calulator(3, 4);

  /*
  if (start == 1)
  {
  start = 0;
    //  ***************************** LCD Testroutine **************************************

    Serial.println("geradern Pin Test  ");

     do {
      //  LCD Testroutine für geraden LCD Pins: 4, 6, 8, 10, 12, 14
      digitalWrite(RS, EIN); // LCD Pin 4
      digitalWrite(EN, EIN); // LCD Pin 6
      digitalWrite(D1, EIN); // LCD Pin 8
      digitalWrite(D3, EIN); // LCD Pin 10
      digitalWrite(D5, EIN); // LCD Pin 12
      digitalWrite(D7, EIN); // LCD Pin 14
      delay(500);
      digitalWrite(RS, AUS);
      digitalWrite(EN, AUS);
      digitalWrite(D1, AUS);
      digitalWrite(D3, AUS);
      digitalWrite(D5, AUS);
      digitalWrite(D7, AUS);
      delay(500);
    } while (digitalRead(ENTER_PIN));

     Serial.println("UNgeradern Pin Test  ");

  do {
    //  LCD Testroutine für UNgeraden LCD Pins: 5, 7, 9, 11, 13, 15
      digitalWrite(RW, EIN); // LCD Pin 5
      digitalWrite(D0, EIN); // LCD Pin 7
      digitalWrite(D2, EIN); // LCD Pin 9
      digitalWrite(D4, EIN); // LCD Pin 11
      digitalWrite(D6, EIN); // LCD Pin 13
      delay(500);
      digitalWrite(RW, AUS);
      digitalWrite(D0, AUS);
      digitalWrite(D2, AUS);
      digitalWrite(D4, AUS);
      digitalWrite(D6, AUS);
      delay(500);
    } while (digitalRead(ENTER_PIN));

   Serial.println("übergabe an LCD");
    lcd.begin(LCD_CHARACTERS, LCD_ZEILEN);                //16 Charakters, 2 Zeilen
  }  //  end if (start == 1)


  //Serial.println("läuft normal");
  */

  do
  {
    Serial.println(Encoder_count_neu);
    delay(500);
    if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value to serial
    {
      Encoder_count_alt = Encoder_count_neu;

      Serial.println(Encoder_count_neu);
      lcd.setCursor(0, 0); // top left
      lcd.print("Encoder:     ");
      lcd.setCursor(10, 0);
      lcd.print(Encoder_count_neu);
    } // end if (Encoder_count_neu != Encoder_count_alt)

    if (!digitalRead(WAAGE_PIN))
    {
      lcd.setCursor(0, 1); // top left
      lcd.print("Taste Waage");
      Serial.println("Taste Waage getrückt");
      delay(500);
    }

    if (!digitalRead(I_O_PIN))
    {
      lcd.setCursor(0, 1); // top left
      lcd.print("Taste I/O  ");
      Serial.println("Taste I/O getrückt");
      delay(500);
    }

    if (!digitalRead(ENTER_PIN))
    {
      lcd.setCursor(0, 1); // top left
      lcd.print("Enter      ");
      Serial.println("Taste Enter getrückt");
      delay(500);
    }

    /*
  if (digitalRead(AL))
    {
      lcd.setCursor(14, 0);
      Serial.println("LINKS");
      lcd.print("LINKS ");
    }
    else
      if (digitalRead(AM))
      {
        lcd.setCursor(14, 0);
        Serial.println("MITTE");
        lcd.print("MITTE ");
      }
      else
        if (digitalRead(AR))
          {
            lcd.setCursor(14, 0);
            Serial.println("RECHTS");
            lcd.print("RECHTS");
          }
        else
          {
            lcd.setCursor(14, 0);
            Serial.println("NO ARM");
            lcd.print("NO ARM");
         }
  */
  } while (digitalRead(ENTER_PIN));
  /*
  digitalWrite(ML, EIN);
  delay(1500);

  digitalWrite(MM, EIN);
  delay(1500);

  digitalWrite(MR, EIN);
  delay(1500);

  digitalWrite(RL, EIN);
  delay(1500);

  digitalWrite(RM, EIN);
  delay(1500);

  digitalWrite(RR, EIN);
  delay(1500);

  digitalWrite(VL, EIN);
  delay(1500);

  digitalWrite(VM, EIN);
  delay(1500);

  digitalWrite(VR, EIN);
  delay(1500);

  digitalWrite(WP, EIN);
  delay(1500);

  digitalWrite(ML, AUS);
  digitalWrite(MM, AUS);
  digitalWrite(MR, AUS);
  digitalWrite(RL, AUS);
  digitalWrite(RM, AUS);
  digitalWrite(RR, AUS);
  digitalWrite(VL, AUS);
  digitalWrite(VM, AUS);
  digitalWrite(VR, AUS);
  digitalWrite(WP, AUS);

  delay(1500);
  */

  // Armposition in Armpointer einlesen. z. B.: Armpointer[Armposition] = ARM_LINKS;

  /*
 Armposition = ARM_LINKS; //read_Armposition;

 digitalWrite(Motoren [Armposition], EIN);
 digitalWrite(Ruettler[Armposition], EIN);

 tone(8, 340, 750);     // Ton F5
 delay(2000);
 // Bis Waagen Zwischenwert erreicht ist
 digitalWrite(Motoren [Armposition], AUS);
 digitalWrite(Ruettler[Armposition], AUS);

 tone(8, 466, 750);     //  Ton Bb5
 delay(2000);

 digitalWrite(WP, EIN);
 digitalWrite(Ventile[Armposition], EIN);

 tone(8, 262, 750);     //  Ton C5
 delay(2000);
 // Bis Waagen Endwert erreicht ist
 digitalWrite(Ventile[Armposition], AUS);
 digitalWrite(WP, AUS);

 tone(8, 392, 750);     //   Ton G5
 delay(2000);



  if (digitalRead(ENTER_PIN) == 1)
 {
   while (!scale.is_ready()) {}      // warten bis die Waage bereit ist

   Leergew_einheiten = scale.read_average(20);
   Serial.println("Eichen OHNE Gewicht   ");
   Serial.println(Leergew_einheiten);

   delay (5000);
   leer = true;
 }  // end if (digitalRead(ENTER_PIN) == 1)
  else
 {
   while (!scale.is_ready()) {}  // warten bis die Waage bereit ist

   Eichgew_einheiten = scale.read_average(20); // Wagenwert mit 150 Gramm
   Serial.println("Eichen MIT Gewicht   ");
   Serial.println(Eichgew_einheiten);
   voll = true;
 }   // end else   if (digitalRead(ENTER_PIN) == 1)

 if (leer & voll)
 {
   Korrekturfaktor = (Eichgew_einheiten - Leergew_einheiten) / 150;    // Differenz der Gewichtseinheit minus der Leereinheitdurch, Dividiert durch das Gewicht in Gramm

   while (true)
   {
     Serial.print("gemessenes Gewicht: ");
     Gewicht = (scale.read() - Leergew_einheiten ) / Korrekturfaktor;
     Serial.println((unsigned int) Gewicht);

     delay (4000);
   }  //  end while (true)
 }  // end if (leer & voll)

 */
} // end void loop() **********************************************************************
