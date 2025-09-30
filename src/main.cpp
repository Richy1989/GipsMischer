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
	Serial.begin(115200);

	Serial.println("jetzt gehts los"); // Erkennung auf dem Laptop Bildschirm schreiben

	// Eingänge zur Armposition Erkennung:
	pinMode(AL, INPUT_PULLUP); // Eingang Armposition LINKS
	pinMode(AM, INPUT_PULLUP); // Eingang Armposition MITTE
	pinMode(AR, INPUT_PULLUP); // Eingang Armposition RECHTS

	// Eingänge zur Gipsbecher Erkennung:
	pinMode(BL, INPUT_PULLUP); // Eingang Armposition LINKS, pullup
	pinMode(BM, INPUT_PULLUP); // Eingang Armposition MITTE, pullup
	pinMode(BR, INPUT_PULLUP); // Eingang Armposition RECHTS, pullup

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

	lcd.createChar(SMILEY, smiley); // erstelltes Zeichen smiley
	lcd.createChar(HERZ, herz);		// erstelltes Zeichen herz
	lcd.createChar(CURSOR, cursor); // erstelltes Zeichen cursor

	// Liest die Datenstruktur aus dem EEPROM
	EEPROM.get(0, daten); // Lesen ab der Adresse 0 aus dem EEPROM
						  // und speichern der Daten in der Variablen Daten (Datenstruktur)

	// Liest die EEPROM Adresse und speichert die Daten in der Variable Korrekturfaktor
	// Adresse: anzahl_daten + EEPROM_ADRESSABSTAND
	EEPROM.get(anzahl_daten + EEPROM_ADRESSABSTAND, Korrekturfaktor);

	// Liest die EEPROM Adresse und speichert die Daten in der Variable Leergew_einheiten
	// Adresse: anzahl_daten + EEPROM_ADRESSABSTAND + EEPROM_ADRESSABSTAND
	EEPROM.get(anzahl_daten + EEPROM_ADRESSABSTAND + EEPROM_ADRESSABSTAND, Leergew_einheiten);

} // end setup **********************************************************************

void loop()
{

	int LCD_cursor_position; // Curserposition auf dem LCD (3 bis 16)
	bool write_to_LCD[MAX_CURSOR_POSITIONEN + 1];	 // damit immer nur einmalig auf den LCD geschrieben wird

	greeting(); // Programmsart und Entscheidung: Serviceroutine oder Hauptprogramm

	on_off_encoder = true; // Encoder Interrupt einschalten

	lcd.clear();	// LCD löschen
	lcd.noBlink();	// Cursor ausschalten
	lcd.noCursor(); // Cursor ausschalten

	////////////////////////////////// Normales Arbeits Programm ///////////////////////////////////////////

	min_counter = 0;					 // Minimalwert für Encoder
	max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

	Encoder_count_neu = min_counter;  // Start mit Zeile 0 (Überschrift)
	Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

	on_off_encoder = true;			// Encoder Interrupt einschalten
	armposition_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
									//////////////////////////// Beginn Arbeitsprogramm //////////////////////////////

	for (int i = 0; i <= MAX_CURSOR_POSITIONEN; i++)
	{
		write_to_LCD[i] = true; // damit immer nur einmalig auf den LCD geschrieben wird
	}

	do
	{
		check_armposition();

		/*
		setzen der LED Anzeige
		*/

		if (Encoder_count_neu != Encoder_count_alt)
		{
			Encoder_count_alt = Encoder_count_neu;

			switch (Encoder_count_neu)
			{
			case 0: /*  Überschrift */
				if (write_to_LCD[0])
				{
					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("01");
					//		lcd.print(byte(SMILEY)); //  Zeichen "smiley" anzeigen
					lcd.write(byte(SMILEY)); //  Zeichen "smiley" anzeigen
					lcd.print(daten[armposition].ueberschrift);

					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("02 xxx g  yyy ml");

					lcd.setCursor(3, 1);					  // Setz Curser auf Charakter 1, Zeile 2
					lcd.print(daten[armposition].gewicht[0]); //  Referenzmenge Gipsgewicht in g

					lcd.setCursor(10, 1);					  // Setz Curser auf Charakter 1, Zeile 2
					lcd.print(daten[armposition].gewicht[1]); // Referenzmenge Waser in g

					write_to_LCD[0] = false; // damit immer nur einmalig auf den LCD geschrieben wird
				} // end if (erstmalig)

				if (!digitalRead(ENTER_PIN))
				{
					while (digitalRead(ENTER_PIN))
					{ // warten bis Taste ENTER losgelasen wird
					} //  end while (digitalRead(ENTER_PIN))
					delay(5); // Entprellzeit

					min_counter = EDIT_CHAR_CURSOR_SART; // Minimalwert für Encoder 0
					max_counter = anzahl_texteingabe;	 // Maximalwert für Encoder (0 bis 39, also 40 Positionen)

					LCD_cursor_position = EDIT_LCD_CURSOR_SART; // Startposition des Cursors am LCD für die Charactereingabe
					// Pointer auf den Character im Character Array, welcher zur Anzeige gebracht wird

					Encoder_count_neu = edit_cursor_start(daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET]);
					Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

					do
					{
						if (Encoder_count_neu != Encoder_count_alt)
						{
							Encoder_count_alt = Encoder_count_neu;

							lcd.setCursor(LCD_cursor_position, 0);	   // Setz Curser auf Charakter 4, Zeile 1
							lcd.print(texteingabe[Encoder_count_neu]); // Character des Character Array auf den LCD schreiben
							lcd.blink();
						} // end if (Encoder_count_neu != Encoder_count_alt)

						if (!digitalRead(ENTER_PIN))
						{
							while (digitalRead(ENTER_PIN))
							{ // warten bis Taste ENTER losgelasen wird
							} //  end while (digitalRead(ENTER_PIN))
							delay(5); // Entprellzeit

							//  abspeichern des Character in der Überschrift
							daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET] = texteingabe[Encoder_count_neu]; // Character in das Überschrift Array schreiben);
							lcd.noBlink();																								  // Bestende LCD Curserposition blinken ausschalten																							  // Bestende LCD Curserposition blinken ausschalten

							LCD_cursor_position++; // Nächste LCD Curserposition

							if (LCD_cursor_position <= EDIT_LCD_CURSOR_MAX)
							{
								Encoder_count_neu = edit_cursor_start(daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET]);
								Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

							} // end if (LCD_cursor_position <= EDIT_LCD_CURSOR_MAX)

						} // end if (!digitalRead(ENTER_PIN))
					} while (LCD_cursor_position <= EDIT_LCD_CURSOR_MAX);

					min_counter = 0;					 // Minimalwert für Encoder
					max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

					Encoder_count_neu = min_counter; // Start mit Zeile 0 (Überschrift)

					write_to_LCD[0] = true; // damit wieder einmalig auf den LCD geschrieben wird

				} // end if (!digitalRead(ENTER_PIN))

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;							  //  end case 0

			case 1:								  /*  Referenz Gipsgewicht / Wassergewicht */
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;

			case 2: /*  Gipsgewicht fix immer um 160 g erhöht */
			case 3:
			case 4:
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;

			case 5: /*  Gipsgewicht in g eingeben default ist 0  */
			case 6:
			case 7:
			case 8:
			case 9:
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;

			case 10:							  /*  Gipsentleerung  */
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;

			case 11:							  /*  Wasserentnahme  */
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;

			default:

				break;
			} // end switch (curser_position)

		} // end if (Encoder_count_neu != Encoder_count_alt
	} while (true);

} // end void loop() **********************************************************************
