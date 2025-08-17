#include "service.h"

//  ************************** Anfang EEPROM TEST **************************************
void EEPROM_test()
{
	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("EEPROM TEST  I/O");
	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("Adr xxxx Dat yyy");

	min_counter = 0;					 // Minimalwert für Encoder
	max_counter = MAX_EEPROM_ADRESSE;	 // Arduino Mega hat 4 KB (4096 bytes) EEPROM Zellen, also 0 bis 4095
	Encoder_count_neu = min_counter;	 // Startwert für Encoder
	Encoder_count_alt = min_counter - 1; // Startwert für Encoder

	do
	{
		if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value to serial
		{
			Encoder_count_alt = Encoder_count_neu;

			lcd.setCursor(4, 1);		  // Setz Curser auf Charakter 5, Zeile 2
			lcd.print("    ");			  // Lösche die Zeile
			lcd.setCursor(4, 1);		  // Setz Curser auf Charakter 5, Zeile 2
			lcd.print(Encoder_count_neu); // Speicherzelle anzeigen

			lcd.setCursor(12, 1);					   // Setz Curser auf Charakter 13, Zeile 2
			lcd.print("   ");						   // Lösche die Zeile
			lcd.setCursor(12, 1);					   // Setz Curser auf Charakter 13, Zeile 2
			lcd.print(EEPROM.read(Encoder_count_neu)); // Speicherzellen-Inhalt anzeigen
		} // end if (Encoder_count_neu != Encoder_count_alt)
	} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

} // end EEPROM_test

//  ************************** Ende EEPROM TEST **************************************

//  ************************** Anfang Service **************************************

void service()
{
	unsigned int Encoder_count_store;
	bool i_am_back = true; // Variable zur Rückkehrerkennung von den Test Routinen
	bool mit_ton = false;  // Variable zum Ton abspielen
	unsigned long start_time;
	unsigned long wait_time = 750; //  Wartezeit in Millisekunden

	// Array für die Test Routinen
	const int test_routinen[] = {EEPROM_TEST, LED_TEST, BECHER_TEST, TON_TEST,
								 WAAGE_KALIBRIERUNG, WAAGE_TEST, RELAIS_TEST};
	// berechnet die Anzahl der test_routinen
	const int anzahl_tests = sizeof(test_routinen) / sizeof(test_routinen[0]);

	Encoder_count_neu = min_counter; // Erststartbedingung für Encoder herstellen
	Encoder_count_alt = min_counter - 1;

	on_off_encoder = true; // Encoder Interrupt einschalten

	do
	{
		if (i_am_back) // Wenn  zurück von den Test Routinen
		{
			min_counter = 0;
			max_counter = anzahl_tests - 1;

			Encoder_count_neu = Encoder_count_store;	 // recall neuen Encoderwert
			Encoder_count_alt = Encoder_count_store - 1; // Startwert herstellen

			lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
			lcd.print("HW TEST  end:I/O"); //  Text in erster Zeile anzeigen
			lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2

			if (mit_ton) //  Tonfolge abspielen
			{
				tone(TONE_PIN, 587);  // D4 (Re)
				delay(300);			  // 300 ms warten
				tone(TONE_PIN, 1587); // D5 (Mi)
				delay(200);			  // 200 ms warten
				noTone(TONE_PIN);	  // Alle Töne ausschalten
				delay(500);			  // 500 ms warten

				mit_ton = false; //  kein Ton ist default
			} // end if (mit_ton)

			i_am_back = false; // Rückkehrerkennung ausschalten

		} //  end if (i_am_back)

		if (Encoder_count_neu != Encoder_count_alt) // If count has changed
		{
			Encoder_count_alt = Encoder_count_neu;

			switch (test_routinen[Encoder_count_neu]) //  Menüauswahl Zeile 1
			{
			case EEPROM_TEST:
				lcd.print("EEPROM TEST  ENT"); // Text in zweiter Zeile anzeigen
				break;						   //  end case EEPROM_TEST

			case LED_TEST:
				//  Direkt anzeigen, alle LED Blinken gleichzeitig, Takt: 750 ms
				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("   LED   TEST   "); // Text in zweiter Zeile anzeigen
				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("LED blinken: ");	   // Text in zweiter Zeile anzeigen

				do
				{
					lcd.setCursor(13, 1); // Setz Curser auf Charakter 14, Zeile 2
					lcd.print("AUS");

					digitalWrite(LL, AUS);
					digitalWrite(LM, AUS);
					digitalWrite(LR, AUS);

					start_time = millis();

					while (Encoder_count_neu == Encoder_count_alt &&
						   (millis() - start_time < wait_time)) // 750 ms Sekunden warten
					{
					} // warten bis Encoder den Wert geändert hat oder die Zeit vergangen ist

					lcd.setCursor(13, 1); // Setz Curser auf Charakter 14, Zeile 2
					lcd.print("EIN");
					digitalWrite(LL, EIN);
					digitalWrite(LM, EIN);
					digitalWrite(LR, EIN);

					start_time = millis();

					while (Encoder_count_neu == Encoder_count_alt &&
						   (millis() - start_time < wait_time)) // 750 ms Sekunden warten
					{
					} // warten bis Encoder den Wert geändert hat oder die Zeit vergangen ist
				} while (Encoder_count_neu == Encoder_count_alt); //  solange bis encoder den Wert geändert hat

				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("                "); // Text in zweiter Zeile löschen

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_store++; // Encoderwert erhöhen
				else
				{
					if (Encoder_count_neu < Encoder_count_store)
						Encoder_count_store--; // Encoderwert verringern
											   // Encoderwert verringern
				}

				mit_ton = false;  // Tonfolge abspielen: false ist default
				i_am_back = true; // Rückkehrerkennung einschalten
				break;			  //  end case LED_TEST

			case BECHER_TEST:
				lcd.print("BECHER TEST     ");
				// Direkt anzeigen, da Becher Test keine Scrollen benötigt
				break; //  end case BECHER_TEST

			case TON_TEST:
				lcd.print("TON TEST     ENT");
				break; //  end case TON_TEST

			case WAAGE_KALIBRIERUNG:
				lcd.print("W KALIBRIER. ENT");
				break; //  end case WAAGE_KALIBRIERUNG

			case WAAGE_TEST:
				lcd.print("  WAAGE   TEST  ");
				// Direkt anzeigen, da Waage Test keine Scrollen benötigt
				break; //  end case WAAGE_TEST

			case RELAIS_TEST:
				lcd.print("  RELAIS  TEST  ");
				// Direkt anzei
				break; //  end case RELAIS_TEST
			default:
			lcd.print(Encoder_count_neu);	
			lcd.print("  ?xx??xx??xx");
				break; //  end default
			} // end switch (Encoder_count_neu)
		} // end if (Encoder_count_neu != Encoder_count_alt)

		//--------------------- Wenn Entertaste gedrückt ist, also low --------------------
		if (!digitalRead(ENTER_PIN))
		{
			lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2

			switch (test_routinen[Encoder_count_neu]) //  Menüauswahl Zeile 2
			{
			case EEPROM_TEST:
				lcd.print("EEPROM TEST  ENT");

				Encoder_count_store = Encoder_count_neu; // Encoderwert Sicherung
				EEPROM_test();
				i_am_back = true; // Rückkehrerkennung einschalten
				mit_ton = true;	  // Ton abspielen
				break;			  // end case EEPROM_TEST

			case LED_TEST:
				//  wird im ersten Menue bearbeitet
				//  alle LED Blinken gleichzeitig
				break; //  end case LED_TEST

			case BECHER_TEST:
				lcd.print("BECHER TEST     ");
				// Direkt anzeigen, da Becher Test keine Scrollen benötigt
				break; //  end case BECHER_TEST

			case TON_TEST:
				lcd.print("TON TEST     ENT");
				break; //  end case TON_TEST

			case WAAGE_KALIBRIERUNG:
				lcd.print("W KALIBRIER. ENT");
				break; //  end case WAAGE_KALIBRIERUNG

			case WAAGE_TEST:
				lcd.print("  WAAGE   TEST  ");
				// Direkt anzeigen, da Waage Test keine Scrollen benötigt
				break; //  end case WAAGE_TEST

			case RELAIS_TEST:
				//on_off_encoder = false; // Encoder Interrupt ausschalten
				lcd.setCursor(0, 1);	// Setz Curser auf Charakter 1, Zeile 2
				lcd.print(relais[Encoder_count_neu]);

				digitalWrite(relais[Encoder_count_neu], EIN); // Relais einschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

				while (!digitalRead(ENTER_PIN))
				{
				}; // Relais halten bis Enter Taste losgelassen

				digitalWrite(relais[Encoder_count_neu], AUS); // Relais ausschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

				on_off_encoder = true; // Encoder Interrupt einschalten
				break;

			default:
				lcd.print("??xx??xx??xx ");
				lcd.print(Encoder_count_neu);
				break; //  end default
			} // end switch (Encoder_count_neu)

		} // if(!digitalRead(ENTER_PIN))
	} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist
} // end service

//  ************************** Service Ende **************************************