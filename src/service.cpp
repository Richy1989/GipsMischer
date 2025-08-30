#include "service.h"

void come_back(bool mit_ton)
{
	lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("Hardw. TEST :I/O"); //  Text in erster Zeile anzeigen
	lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2

	min_counter = 0;
	max_counter = anzahl_tests - 1;

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

} //  end come_back(bool mit_ton)

//  ************************** Anfang Service **************************************

void service()
{
	int bl, bm, br; // Variablen für Becher auf LCD Schreiberkennung

	come_back(false);				 // Rückkehrerkennung einschalten ohne Ton
	Encoder_count_neu = min_counter; // Erststartbedingung für Encoder herstellen
	Encoder_count_alt = max_counter; // Erststartbedingung für Encoder herstellen
	on_off_encoder = true;			 // Encoder Interrupt einschalten

	do
	{
		if (Encoder_count_neu != Encoder_count_alt) // If count has changed
		{
			Encoder_count_alt = Encoder_count_neu;

			switch (test_routinen[Encoder_count_neu]) //  Menüauswahl Zeile 1
			{
			case EEPROM_TEST:
				lcd.print("EEPROM TEST :ENT"); // Text in zweiter Zeile anzeigen

				//Serial.println("im case EEPROM_TEST");
				//delay(400);
				//Serial.println(digitalRead(ENTER_PIN));

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{
					Serial.println("Enter Taste gedrueckt");

					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("EEPROM TEST :I/O");
					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("Adr:xxxx Dat:yyy");

					min_counter = 0;					 // Minimalwert für Encoder
					max_counter = MAX_EEPROM_ADRESSE;	 // Arduino Mega hat 4 KB (4096 Byts, 0-4095 bytes) EEPROM Zellen
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
							lcd.print(Encoder_count_neu); // Speicheradresse (0 - 4096) anzeigen

							lcd.setCursor(13, 1);					   // Setz Curser auf Charakter 13, Zeile 2
							lcd.print("   ");						   // Lösche die Zeile
							lcd.setCursor(13, 1);					   // Setz Curser auf Charakter 13, Zeile 2
							lcd.print(EEPROM.read(Encoder_count_neu)); // Daten der Adresse (FF = leer) anzeigen
																	   /*
																	   Beispiel der Datenspeicherung:
																								  // EEPROM.put(0, daten);
																								  // EEPROM.get(0, daten); // Lies die Datenstruktur aus der EEPROM-Adresse 1)
																	   */
						} // end if (Encoder_count_neu != Encoder_count_alt)
					} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

					Encoder_count_neu = EEPROM_TEST; // Verbleib im Menü EEPROM Test
			//		Encoder_count_alt = LED_TEST;	 // Startwert herstellen
					come_back(true);				 // Rückkehrerkennung einschalten mit Ton
				} //  if (!digitalRead(ENTER_PIN))
				Encoder_count_alt = min_counter - 1;  // Damit Abfrage nach Enter Taste weiter läuft
				break; //  end case EEPROM_TEST

			case LED_TEST:
				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("LED TEST end:I/O"); // Text in erster Zeile anzeigen
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

					while (Encoder_count_neu == Encoder_count_alt && // logisches UND
						   (millis() - start_time < wait_time))		 // 750 ms Sekunden warten
					{
					} // warten bis Encoder den Wert geändert hat oder die Zeit vergangen ist

					lcd.setCursor(13, 1); // Setz Curser auf Charakter 14, Zeile 2
					lcd.print("EIN");
					digitalWrite(LL, EIN);
					digitalWrite(LM, EIN);
					digitalWrite(LR, EIN);

					start_time = millis();

					while (Encoder_count_neu == Encoder_count_alt && // logisches UND
						   (millis() - start_time < wait_time))		 // 750 ms Sekunden warten
					{
					} // warten bis Encoder den Wert geändert hat oder die Zeit vergangen ist
				} while (Encoder_count_neu == Encoder_count_alt); //  solange bis encoder den Wert geändert hat

				Encoder_count_alt = LED_TEST;

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = BECHER_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = EEPROM_TEST; // Encoderwert verringern

				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case LED_TEST

			case BECHER_TEST:
				bl = 2;
				bm = 2;
				br = 2;

				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("BECHER T end:I/O"); // Text in erster Zeile anzeigen
				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("L:    M:    R:  "); // Text in zweiter Zeile anzeigen OK und NO

				do
				{
					// Becher Links
					lcd.setCursor(2, 1);			 // Setz Curser auf Charakter 3, Zeile 2
					if (!digitalRead(BL) && bl != 1) // Becher Links erkannt UND noch nicht auf LCD geschrieben
					{
						bl = 1; // Erkennung wurde bereits auf LCD geschrieben
						lcd.print("OK");
					}
					else if (bl != 0)
					{
						bl = 0; // Erkennung wurde bereits auf LCD geschrieben
						lcd.print("NO");
					}

					// Becher Mitte
					lcd.setCursor(8, 1);			 // Setz Curser auf Charakter 9, Zeile 2
					if (!digitalRead(BM) && bm != 1) // Becher Mitte erkannt UND noch nicht auf LCD geschrieben
					{
						bm = 1; // Erkennung wurde bereits auf LCD geschrieben
						lcd.print("OK");
					}
					else if (bm != 0)
					{
						bm = 0; // Erkennung wurde bereits auf LCD geschrieben
						lcd.print("NO");
					}

					// Becher Rechts
					lcd.setCursor(14, 1);			 // Setz Curser auf Charakter 15, Zeile 2
					if (!digitalRead(BR) && br != 1) // Becher Rechts erkannt UND noch nicht auf LCD geschrieben
					{
						br = 1; // Erkennung wurde bereits auf LCD geschrieben
						lcd.print("OK");
					}
					else if (br != 0)
					{
						bm = 0; // Erkennung wurde bereits auf LCD geschrieben
						lcd.print("NO");
					}
				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				Encoder_count_alt = BECHER_TEST;

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = TON_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = LED_TEST; // Encoderwert verringern

				come_back(false); // Rückkehrerkennung einschalten ohne Ton

				break; //  end case BECHER_TEST

			case TON_TEST:
				Encoder_count_store = Encoder_count_neu;

				lcd.print("     Sirene     "); // Text in zweiter Zeile anzeigen

				do
				{
					for (int i = 200; i <= 4000; i = i + 2) // Sirene aufwärts
					{
						tone(TONE_PIN, i);
						delay(2); // 2 ms warten
						if (Encoder_count_neu != Encoder_count_alt)
							break;
					}
					if (Encoder_count_neu == Encoder_count_alt)
						for (int i = 4000; i >= 200; i = i - 2) // Sirene abwärts
						{

							tone(TONE_PIN, i);
							delay(2); // 2 ms warten
							if (Encoder_count_neu != Encoder_count_alt)
								break;
						}

				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				noTone(TONE_PIN); // Alle Töne ausschalten

				Encoder_count_alt = TON_TEST;

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = ARM_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = BECHER_TEST; // Encoderwert verringern

				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case TON_TEST

			case ARM_TEST:
				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("    ARM TEST    ");
				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case WAAGE_KALIBRIERUNG

			case WAAGE_KALIBRIERUNG:
				lcd.print("W KALIBRIER. ENT");
				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case WAAGE_KALIBRIERUNG

			case WAAGE_TEST:
				lcd.print("  WAAGE   TEST  ");
				// Direkt anzeigen, da Waage Test keine Scrollen benötigt
				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case WAAGE_TEST

			case RELAIS_TEST:
				lcd.print("RELAIS TEST  ENT");
				// Direkt anzei
				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case RELAIS_TEST
			default:
				lcd.print(Encoder_count_neu);
				lcd.print("  ?xx??xx??xx");
				break; //  end default
			} // end switch (Encoder_count_neu)
		} // end if (Encoder_count_neu != Encoder_count_alt)
	} while (digitalRead(I_O_PIN)); // solange I/O Taste nicht gedrückt ist, also high ist
} // end service

//  ************************** Service Ende **************************************