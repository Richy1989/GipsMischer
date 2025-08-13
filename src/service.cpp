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
	on_off_encoder = true;			 // Encoder Interrupt einschalten

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
	}
	while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

	on_off_encoder = false; // Encoder Interrupt ausschalten

	// clear_tast_buf(); // Tastaturpuffer löschen

} // end EEPROM_test

//  ************************** Ende EEPROM TEST **************************************

//  ************************** Anfang Service **************************************

void service()
{
	int Encoder_count_store;
    // Array für die Test Routinen
	const int test_routinen[] = {EEPROM_TEST, LED_TEST, BECHER_TEST,
								 TON_TEST, WAAGE_KALIBRIERUNG, WAAGE_TEST}; 
	// berechnet die Anzahl der test_routinen
	const int anzahl_tests = sizeof(test_routinen) / sizeof(test_routinen[0]);

	min_counter = 0;
	max_counter = anzahl_tests - 1;
	Encoder_count_neu = min_counter; // Erststartbedingung für Encoder herstellen
	Encoder_count_alt = min_counter - 1;

	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("HW TESTE end:I/O");

	on_off_encoder = true; // Encoder Interrupt einschalten

	do
	{
		if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value to serial
		{
			Encoder_count_alt = Encoder_count_neu;

			lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2

			switch (test_routinen[Encoder_count_neu])
			{
			case EEPROM_TEST:
				lcd.print("EEPROM TEST  ENT");

				break; //  end case EEPROM_TEST

			case LED_TEST:
				lcd.print("   LED   TEST   ");

				//  Direkt anzeigen, da Becher Test keine Scrollen benötigt
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
			default:
				lcd.print("??xx??xx??xx??xx");
				break; //  end default
			} // end switch (Encoder_count_neu)
		} // end if (Encoder_count_neu != Encoder_count_alt)

		if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
		{
			lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
			switch (test_routinen[Encoder_count_neu])
			{
			case EEPROM_TEST:
				lcd.print("EEPROM TEST  ENT");

				Encoder_count_store = Encoder_count_neu; // Encoderwert Sicherung
				EEPROM_test();
				Encoder_count_neu = Encoder_count_store; // recall alten Encoderwert

				break; //  end case EEPROM_TEST

			case LED_TEST:
				lcd.print("   LED   TEST   ");

				//  Direkt anzeigen, da Becher Test keine Scrollen benötigt
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
			default:
				lcd.print("??xx??xx??xx??xx");
				break; //  end default
			} // end switch (Encoder_count_neu)

			on_off_encoder = false; // Encoder Interrupt ausschalten
			lcd.setCursor(0, 1);		 // Setz Curser auf Charakter 1, Zeile 2
			lcd.print(relais[Encoder_count_neu]);

			digitalWrite(relais[Encoder_count_neu], EIN); // Relais einschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

			while (!digitalRead(ENTER_PIN))
			{
			}; // Relais halten bis Enter Taste losgelassen

			digitalWrite(relais[Encoder_count_neu], AUS); // Relais ausschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

			on_off_encoder = true; // Encoder Interrupt einschalten
		} // if(!digitalRead(ENTER_PIN))

	} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist
} // end service

//  ************************** Service Ende **************************************