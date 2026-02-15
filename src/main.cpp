#include "service.h"
#include "hauptprogramm.h"

void read_encoder()
{
	// Encoder interrupt routine for both pins. Updates counter
	// if they are valid and have rotated a full indent
	// Erklärung Youtube: How to use a Rotary Encoder with an Arduino - CODE EXPLAINED!
	// https://www.youtube.com/watch?v=fgOfSHTYeio

	static uint8_t old_AB = 3;																 // Lookup table index
	static int8_t encval = 0;																 // Encoder value
	static const int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Lookup table

	// nur wenn Encoder freigeschalten (true), soll interrupt Routine arbeiten
	if (on_off_encoder)
	{
		old_AB <<= 2; // Remember previous state

		if (digitalRead(ENCODER_A))
			old_AB |= 0x02; // Add current state of pin A
		if (digitalRead(ENCODER_B))
			old_AB |= 0x01; // Add current state of pin B

		encval += enc_states[(old_AB & 0x0f)]; // Abschneiden der vorderen 8 Bits

		// Update counter if encoder has rotated a full indent, that is at least 4 steps
		if (encval > 3)
		{						 // Four steps forward, zählt AUFwärts
			int changevalue = 1; // Incrementeinheit ist 1

			// Incrementeinheit wird auf 10 erhöht wenn schneller gedreht wird
			if ((micros() - _lastIncReadTime) < _pauseLength)
				changevalue = FAST_INCREMENT;

			Encoder_count_neu += changevalue; // incremet counter um 1 oder 10;

			//  Korrektur, wenn "überzählt", dann auf MAX setzen
			if (Encoder_count_neu > max_counter)
				Encoder_count_neu = min_counter;

			_lastIncReadTime = micros();
			encval = 0;
		} // end if (encval > 3)
		else if (encval < -3)
		{						  // Four steps backward, zählt ABwärts
			int changevalue = -1; // Decrementeinheit ist -1

			// Decrementeinheit wird auf -10 erhöht  wenn schneller gedreht wird
			if ((micros() - _lastDecReadTime) < _pauseLength)
				changevalue = FAST_INCREMENT * changevalue;

			Encoder_count_neu += changevalue; // Decremet counter um 1 oder 10;

			//  Korrektur, wenn "unterzählt", dann auf MIN setzen
			if (Encoder_count_neu < min_counter)
				Encoder_count_neu = max_counter;

			_lastDecReadTime = micros();
			encval = 0;
		} // end else if(encval < -3)
	} // end if (on_off_encoder)
} // end read_encoder() **********************************************************************

void setup()
{
	Serial.begin(115200); // Kommunikation zum Laptop Bildschirm

	Serial.println("jetzt gehts los"); // Text auf dem Laptop Bildschirm schreiben

	// Eingänge zur Armposition Erkennung:
	pinMode(AL, INPUT_PULLUP); // Eingang Armposition LINKS, pullup
	pinMode(AM, INPUT_PULLUP); // Eingang Armposition MITTE, pullup
	pinMode(AR, INPUT_PULLUP); // Eingang Armposition RECHTS, pullup

	// Eingänge zur Gipsbecher Erkennung:
	pinMode(BL, INPUT_PULLUP); // Eingang Gips Becher LINKS, pullup
	pinMode(BM, INPUT_PULLUP); // Eingang Gips Becher MITTE, pullup
	pinMode(BR, INPUT_PULLUP); // Eingang Gips Becher RECHTS, pullup

	// Ausgang zur Gipsbecher Erkennung:
	pinMode(TONE_PIN, OUTPUT); // Ausgang zur Tone Erzeugung

	// Ausgang zur Front LED:
	pinMode(LL, OUTPUT); // Ausgang zur LED LINKS
	pinMode(LM, OUTPUT); // Ausgang zur LED MITTE
	pinMode(LR, OUTPUT); // Ausgang zur LED RECHTS

	// Eingänge zur ENCODER Erkennung:
	pinMode(ENCODER_B, INPUT_PULLUP); // Eingang pullup Interrupt 4, Anzeigeprint FlBaKa Pin 18, Pin 2)
	pinMode(ENCODER_A, INPUT_PULLUP); // Eingang pullup Interrupt 3, Anzeigeprint FlBaKa Pin 17, Pin 18)

	pinMode(INTERRUPT_RESERVE1, INPUT_PULLUP); // Eingang pullup Interrupt 5, Pin 3)
	pinMode(INTERRUPT_RESERVE2, INPUT_PULLUP); // Eingang pullup Interrupt 2, Pin 19)

	pinMode(LCD_BASE_30, INPUT_PULLUP); // Eingang pullup, 5Vcc, Anzeigeprint FlBaKa Pin 1, Pin 30)
	pinMode(WAAGE_PIN, INPUT_PULLUP);	// Eingang pullup, Anzeigeprint FlBaKa Pin 2, Pin 31)

	// Ausgänge für LCD bei LiquidCrysta
	// OUTPUT wird durch Library LiquidCrystal.h gesetzt

	pinMode(LCD_BASE_43, INPUT_PULLUP); // Eingang pullup Poti, LCD Helligkeit (Anzeigeprint FlBaKa Pin 14)
	pinMode(I_O_PIN, INPUT_PULLUP);		// Eingang pullup (Anzeigeprint FlBaKa Pin 15)
	pinMode(ENTER_PIN, INPUT_PULLUP);	// Eingang pullup (Anzeigeprint FlBaKa Pin 16)
	pinMode(LCD_BASE_46, INPUT_PULLUP); // Eingang pullup Pin 3 Interrupt 5 (Anzeigeprint FlBaKa Pin 17)
	pinMode(LCD_BASE_47, INPUT_PULLUP); // Eingang pullup Pin 2 Interrupt 4 (Anzeigeprint FlBaKa Pin 18)
	pinMode(LCD_BASE_48, INPUT_PULLUP); // Eingang pullup GND (Anzeigeprint FlBaKa Pin 19)
	pinMode(LCD_BASE_49, INPUT_PULLUP); // Eingang pullup GND (Anzeigeprint FlBaKa Pin 20)

	// Ausgänge werden als  Eingänge geschaltzen und dienen als Stützpunkte
	pinMode(AUSGANG_9, INPUT_PULLUP); // A9 wird Eingang pullup für Vcc (Relaisprint FlBaKa Pin 1 und 2)
	pinMode(AUSGANG_0, INPUT_PULLUP); // A0 wird Eingang pullup für GND (Relaisprint FlBaKa Pin 19 und 20)

	// Ausgänge für Relais : Motoren, Rüttler, Ventile und Wasserpumpe definieren
	for (unsigned int i = 0; i < anzahlrelais; i++) // Pointer von 0 bis 9
	{
		pinMode(relais[i], OUTPUT);	  // Ausgänge der Relais als OUTPUT konfigutieren
		digitalWrite(relais[i], AUS); // Relais ausschalten (negative Logik: Ausgang ist also high, wenn Relais AUS ist)
	}

	// Front LED ausschalten. Verkehrte Logik: LOW = EIN, HIGH = AUS
	digitalWrite(LL, EIN);
	digitalWrite(LM, EIN);
	digitalWrite(LR, EIN);

	//  attach interrupt-routin,
	//  Interrupt 5, Pin 3, Stützpunkt Pin 46, FlBaKa Pin 17
	attachInterrupt(digitalPinToInterrupt(ENCODER_A), read_encoder, CHANGE);

	//  attach interrupt-routin,
	//  Interrup 4, Pin 2, Stützpunkt Pin 47, FlBaKa Pin 18
	attachInterrupt(digitalPinToInterrupt(ENCODER_B), read_encoder, CHANGE);

	// Initialisierung LCD
	lcd.begin(LCD_CHARACTERS, LCD_ZEILEN); // 16 Charakters, 2 Zeilen

	// Initialisierung Waage
	// Eingang/Ausgang für I2C Bus bei HX711 (Interruptfähige Eingänge)
	// INPUT oder OUTPUT wird durch Library HX711.h gesetzt
	// pinMode(DATA_PIN, OUTPUT); // Daten PIN
	// pinMode(CLOCK_PIN, OUTPUT); // Clock PIN
	scale.begin(DATA_PIN, CLOCK_PIN); // dataPin =20, clockPin = 21, gain = 128 / Interruptfähige Pins

	// Cal init in setup
	// init_data();
	// daten[0].ueberschrift = "Hello World"; // Beispiel Initialisierung der Überschrift

	lcd.createChar(SMILEY, smiley);							// erstelltes Zeichen Smiley
	lcd.createChar(HERZ, herz);								// erstelltes Zeichen Herz
	lcd.createChar(CURSOR, cursor);							// erstelltes Zeichen Cursor
	lcd.createChar(RECHTECK_UMRAHMUNG, rechteck_umrahmung); // erstelltes Zeichen Rechteck

	// Liest die Datenstruktur aus dem EEPROM
	EEPROM.get(0, daten); // Lesen ab der Adresse 0 aus dem EEPROM
						  // und speichern der Daten in der Variablen Daten (Datenstruktur)

	// WAAGE KALIBRIERUNGSDATEN AUS DEM EEPROM LESEN
	// Liest die EEPROM Adresse und speichert die Daten in der Variable Korrekturfaktor
	// Adresse: anzahl_daten + EEPROM_ADRESSABSTAND
	EEPROM.get(anzahl_daten + EEPROM_ADRESSABSTAND, Korrekturfaktor);

	// WAAGE KALIBRIERUNGSDATEN AUS DEM EEPROM LESEN
	// Liest die EEPROM Adresse und speichert die Daten in der Variable Leergew_einheiten
	// Adresse: anzahl_daten + EEPROM_ADRESSABSTAND + EEPROM_ADRESSABSTAND
	EEPROM.get(anzahl_daten + EEPROM_ADRESSABSTAND + EEPROM_ADRESSABSTAND, Leergew_einheiten);

	// Wasser zu Gips Verhältnis berechnen und abspeichern
	for (int i = 0; i < MAX_DATEN_SATZ; i++) // für jeden Datensatz, 0 bis 2 also 3 Datensätze
		h2o_gips_verhaeltnis[i] =
			(float)daten[i].gewicht[1] //  Wasser Referenzgewicht
			/						   //  dividiert durch
			(float)daten[i].gewicht[0] //  Gips Referenzgewicht
			;						   //  Ergebnis ist Wasser zu Gips Verhältnis


} // end setup **********************************************************************

void loop()
{
	mainprogramm();

} // end void loop() **********************************************************************
