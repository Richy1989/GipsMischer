#include "service.h"

void come_back(bool mit_ton)
{
	lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("Hardw. TEST :I/O"); //  Text in erster Zeile anzeigen
	lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2

	min_counter = 0;				// Minimalwert für Encoder
	max_counter = anzahl_tests - 1; // Maximalwert für Encoder

	on_off_encoder = true; // Encoder Interrupt einschalten

	if (mit_ton) //  Tonfolge abspielen
	{
		tone(TONE_PIN, 587);  // D4 (Re)
		delay(300);			  // 300 ms warten
		tone(TONE_PIN, 1587); // D5 (Mi)
		delay(200);			  // 200 ms warten
		noTone(TONE_PIN);	  // Alle Töne ausschalten
		delay(500);			  // 500 ms warten
	} // end if (mit_ton)

} //  end come_back(bool mit_ton)

//  ************************** Anfang Service **************************************

void service()
{
	// Variablen für Becher auf LCD Schreiberkennung
	bool bl_LCD_NO, bm_LCD_NO, br_LCD_NO, bl_LCD_OK, bm_LCD_OK, br_LCD_OK;
	abbruch = false;

	come_back(false); // Rückkehrerkennung einschalten ohne Ton

	Encoder_count_neu = EEPROM_TEST;
	Encoder_count_alt = RELAIS_TEST; // Erststartbedingung herstellen

	on_off_encoder = true; // Encoder Interrupt einschalten

	do
	{
		if (Encoder_count_neu != Encoder_count_alt) // If count has changed
		{
			Encoder_count_alt = Encoder_count_neu;

			switch (test_routinen[Encoder_count_neu]) //  Menüauswahl Zeile 1
			{
			case EEPROM_TEST:
				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("EEPROM TEST :ENT"); // Text in zweiter Zeile anzeigen

				// Serial.println("im case EEPROM_TEST");
				// delay(400);
				// Serial.println(digitalRead(ENTER_PIN));

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{
					//		Serial.println("Enter Taste gedrueckt");

					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("EEPROM TEST :I/O");
					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("Adr:     Dat:   ");

					// Min- Maxwerte für Encoder
					min_counter = 0;
					max_counter = MAX_EEPROM_ADRESSE; // Mega hat 4 KB (4096 Byts, 0-4095 bytes) EEPROM Zellen

					Encoder_count_neu = min_counter;	 // Routinenwiedereintritt herstellen
					Encoder_count_alt = min_counter - 1; // Routinenwiedereintritt herstellen

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

					come_back(true); // Rückkehrerkennung einschalten, Encoder Grenzen setzen, mit Ton

					Encoder_count_neu = EEPROM_TEST;
				} //  end  if (!digitalRead(ENTER_PIN))

				Encoder_count_alt = OUT_OF_RANGE; // Damit Abfrage nach Enter Taste weiter läuft
				break;							  //  end case EEPROM_TEST

			case LED_TEST:
				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("LED TEST    :I/O"); // Text in erster Zeile anzeigen
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

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = BECHER_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = EEPROM_TEST; // Encoderwert verringern

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case LED_TEST

			case BECHER_TEST:
				bl_LCD_OK = false;
				bm_LCD_OK = false;
				br_LCD_OK = false;
				bl_LCD_NO = false;
				bm_LCD_NO = false;
				br_LCD_NO = false;

				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("BECHER TEST :I/O"); // Text in erster Zeile anzeigen
				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("L:    M:    R:  "); // Text in zweiter Zeile anzeigen OK und NO

				do
				{
					// Becher Links
					lcd.setCursor(2, 1); // Setz Curser auf Charakter 3, Zeile 2

					if (!digitalRead(BL)) // wenn Becher Links erkannt wird
					{
						if (!bl_LCD_OK) // OK Erkennung wurde nicht LCD geschrieben
						{
							bl_LCD_NO = false; // NO Erkennung wurde nicht auf LCD geschrieben
							bl_LCD_OK = true;  // OK Erkennung wurde auf LCD geschrieben
							lcd.print("OK");
						} // end   if (!bl_LCD_OK)
					} // end if (!digitalRead(BL))
					else if (!bl_LCD_NO)
					{
						bl_LCD_NO = true;  // NO Erkennung wurde auf LCD geschrieben
						bl_LCD_OK = false; // OK Erkennung wurde nicht auf LCD geschrieben
						lcd.print("NO");
					} // end  if (!blLCD_NO)

					// Becher Mitte
					lcd.setCursor(8, 1); // Setz Curser auf Charakter 9, Zeile 2

					if (!digitalRead(BM)) // wenn Becher Mitte erkannt wird
					{
						if (!bm_LCD_OK) // OK Erkennung wurde nicht LCD geschrieben
						{
							bm_LCD_NO = false; // NO Erkennung wurde nicht auf LCD geschrieben
							bm_LCD_OK = true;  // OK Erkennung wurde auf LCD geschrieben
							lcd.print("OK");
						} // end   if (!bm_LCD_OK)
					} // end if (!digitalRead(BM))
					else if (!bm_LCD_NO)
					{
						bm_LCD_NO = true;  // NO Erkennung wurde auf LCD geschrieben
						bm_LCD_OK = false; // OK Erkennung wurde nicht auf LCD geschrieben
						lcd.print("NO");
					} // end  if (!bm_LCD_NO)

					// Becher Rechts
					lcd.setCursor(14, 1); // Setz Curser auf Charakter 15, Zeile 2

					if (!digitalRead(BR)) // wenn Becher Rechts erkannt wird
					{
						if (!br_LCD_OK) // OK Erkennung wurde nicht LCD geschrieben
						{
							br_LCD_NO = false; // NO Erkennung wurde nicht auf LCD geschrieben
							br_LCD_OK = true;  // OK Erkennung wurde auf LCD geschrieben
							lcd.print("OK");
						} // end   if (!br_LCD_OK)
					} // end if (!digitalRead(BR))
					else if (!br_LCD_NO)
					{
						br_LCD_NO = true;  // NO Erkennung wurde auf LCD geschrieben
						br_LCD_OK = false; // OK Erkennung wurde nicht auf LCD geschrieben
						lcd.print("NO");
					} // end  if (!br_LCD_NO)

				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = TON_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = LED_TEST; // Encoderwert verringern

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

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

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = ARM_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = BECHER_TEST; // Encoderwert verringern

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case TON_TEST

			case ARM_TEST:
				Encoder_count_store = Encoder_count_neu;

				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("ARM TEST    :I/O"); // Text in erster Zeile anzeigen

				armposition = ARM_NO_POS; // keine Position, Startwert
				armposition_alt = 255;	  // Startwert für Erstanfang

				do
				{
					if (!digitalRead(AL))
						armposition = ARM_LINKS; // Negative Logik
					else
					{
						if (!digitalRead(AM))
							armposition = ARM_MITTE;
						else
						{
							if (!digitalRead(AR))
								armposition = ARM_RECHTS;
							else
								armposition = ARM_NO_POS;
						} // end else if (!digitalRead(AM))

						if (armposition != armposition_alt)
						{
							armposition_alt = armposition;
							lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2

							switch (armposition)
							{
							case ARM_LINKS:
								lcd.print("     LINKS      ");
								break;
							case ARM_MITTE:
								lcd.print("     MITTE      ");
								break;
							case ARM_RECHTS:
								lcd.print("     RECHTS     ");
								break;
							case ARM_NO_POS:
								lcd.print(" KEINE POSITION ");
								break;
							default:
								lcd.print("     ERROR      ");
								break;
							} // end switch (armposition)

						} // end (armposition != armposition_alt) if (!digitalRead(AL))
					} //  end else   if (!digitalRead(AL))
				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = WAAGE_KALIBRIERUNG; // Encoderwert erhöhen
				else
					Encoder_count_neu = TON_TEST; // Encoderwert verringern

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case ARM_TEST

			case WAAGE_KALIBRIERUNG:
				if (abbruch) // es erfolgte ein Bedinerabbruch, daher LCD wieder aktualisieren
				{
					abbruch = false;  // Abbruch zurücksetzen
					come_back(false); // Rückkehrerkennung einschalten ohne Ton
				} //  if (abbruch)

				lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("W. KALIBRI. :ENT");

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{
					on_off_encoder = false; // Encoder Interrupt ausschalten

					Encoder_count_store = Encoder_count_neu;
					abbruch = false;

					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("Tara        :I/O");
					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print(" wird ermittelt  ");

					while (!scale.is_ready()) // scale.is_ready --> Waage ist bereit wenn true
					{
						if (!digitalRead(I_O_PIN)) // Bedienerabbruch der Kalibrierung
						{
							abbruch = true;
							break; // bricht.... while (!scale.is_ready() )
						}
					} // warten bis die Waage bereit ist

					if (abbruch)
					{
						abbruch = false;						 // Abbruch zurücksetzen
						Encoder_count_neu = Encoder_count_store; // Verbleib im Menü WAAGE_KALIBRIERUNG
						Encoder_count_alt = OUT_OF_RANGE;		 // Erststartbedingung herstellen

						on_off_encoder = true; // Encoder Interrupt einschalten
						break;				   // case WAAGE_KALIBRIERUNG
					} //  end if (abbruch)

					Leergew_einheiten = scale.read_average(20); // Leergewicht einlesen, Durchschnitt von 20 Messungen

					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("            :ENT");
					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print(Leergew_einheiten);

					while (digitalRead(ENTER_PIN))
					{
						if (!digitalRead(I_O_PIN)) // Bedienerabbruch der Kalibrierung
						{
							abbruch = true;

							break; // bricht  while (digitalRead(ENTER_PIN) )

						} // end if (!digitalRead(I_O_PIN)
					} // zum ansehen der Leergewichteinheiten, weiter mit ENTER oder I/O Taste (abbruch)

					if (abbruch)
					{
						abbruch = false;						 // Abbruch zurücksetzen
						Encoder_count_neu = Encoder_count_store; // Verbleib im Menü WAAGE_KALIBRIERUNG
						Encoder_count_alt = OUT_OF_RANGE;		 // Erststartbedingung herstellen

						on_off_encoder = true; // Encoder Interrupt einschalten
						break;				   // case WAAGE_KALIBRIERUNG
					} //  end if (abbruch)

					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("G. auflegen :I/O");
					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("fertig      :ENT");

					while (!digitalRead(ENTER_PIN)) // warten bis Enter taste losgelasen wird
					{
					};
					delay(5); // Entprellzeit

					while (digitalRead(ENTER_PIN))
					{
						if (!digitalRead(I_O_PIN)) // Bedienerabbruch der Kalibrierung
						{
							abbruch = true;
							break; // while (digitalRead(I_O_PIN) || digitalRead(ENTER_PIN))
						}

					} // auf Bestätigung des Gewichtauflegen warten, weiter mit ENTER oder I/O Taste (abbruch)

					if (abbruch)
					{
						abbruch = false;						 // Abbruch zurücksetzen
						Encoder_count_neu = Encoder_count_store; // Verbleib im Menü WAAGE_KALIBRIERUNG
						Encoder_count_alt = OUT_OF_RANGE;		 // Erststartbedingung herstellen

						on_off_encoder = true; // Encoder Interrupt einschalten
						break;				   // case WAAGE_KALIBRIERUNG
					} //  end if (abbruch)

					lcd.setCursor(0, 0); // Set cursor to column 1, row 1
					lcd.print("G. eingeben :I/O");
					lcd.setCursor(0, 1); // Set cursor to column 1, row 2
					lcd.print("Netto      g:ENT");

					min_counter = 0;		   // Encounter Min. Wert
					max_counter = MAX_GEWICHT; // Encounter Max. Wert, Maximales messbares Gewicht in Gramm
					Encoder_count_neu = min_counter;
					Encoder_count_alt = min_counter - 1;

					while (!digitalRead(ENTER_PIN)) // warten bis Enter taste losgelasen wird
					{
					};

					delay(5); // Entprellzeit

					on_off_encoder = true; // Encoder Interrupt einschalten

					while (digitalRead(ENTER_PIN))
					{
						if (!digitalRead(I_O_PIN)) // Bedienerabbruch der Kalibrierung
						{
							abbruch = true;
							break; // end do... while (digitalRead(ENTER_PIN))
						}

						if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value
						{
							Encoder_count_alt = Encoder_count_neu;

							lcd.setCursor(6, 1);		  // Set cursor to column 7, row 2
							lcd.print("    ");			  // Clear previous value
							lcd.setCursor(6, 1);		  // Set cursor to column 7, row 2
							lcd.print(Encoder_count_neu); // im Encoder_count_neu steht das Gewicht in Gramm
						} // end if (Encoder_count_neu != Encoder_count_alt)

					} // Bestätigung des eingegebenen Gewichtwertes, weiter mit ENTER oder I/O Taste (abbruch)

					on_off_encoder = false; // Encoder Interrupt ausschalten

					if (abbruch)
					{
						abbruch = false;						 // Abbruch zurücksetzen
						Encoder_count_neu = Encoder_count_store; // Verbleib im Menü WAAGE_KALIBRIERUNG
						Encoder_count_alt = OUT_OF_RANGE;		 // Erststartbedingung herstellen

						on_off_encoder = true; // Encoder Interrupt einschalten
						break;				   // case WAAGE_KALIBRIERUNG
					} // end if (abbruch)

					while (!scale.is_ready())
					{
						if (!digitalRead(I_O_PIN)) // Bedienerabbruch der Kalibrierung
						{
							abbruch = true;
							break; //  bricht  while (!scale.is_ready())
						}
					} // warten bis die Waage bereit ist oder I/O Taste (abbruch) gedrückt wird

					if (abbruch)
					{
						abbruch = false;						 // Abbruch zurücksetzen
						Encoder_count_neu = Encoder_count_store; // Verbleib im Menü WAAGE_KALIBRIERUNG
						Encoder_count_alt = OUT_OF_RANGE;		 // Erststartbedingung herstellen

						on_off_encoder = true; // Encoder Interrupt einschalten

						break; // case WAAGE_KALIBRIERUNG
					} //  end if (abbruch)

					Eichgew_einheiten = scale.read_average(3); // Waagenwert einlesen, Durchschnitt von 3 Messungen

					// Differenz der Gewichtseinheit minus der Leereinheitdurch,
					// Dividiert durch das eingegeben Gewicht in Gramm (Encoder_count_neu)
					Korrekturfaktor = (Eichgew_einheiten - Leergew_einheiten) / Encoder_count_neu;
					/////////////////////////////// Korekturwert im EEPROM abspeichern ***************
					/////////////////////////////// Leergew_einheiten im EEPROM abspeichern ***************
					Encoder_count_neu = Encoder_count_store; // Verbleib im Menü WAAGE_KALIBRIERUNG
					come_back(true);						 // Rückkehrerkennung einschalten mit Ton
				} // end if (!digitalRead(ENTER_PIN))

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				come_back(false); // Rückkehrerkennung einschalten ohne Ton
				break;			  //  end case WAAGE_KALIBRIERUNG

			case WAAGE_TEST:
				lcd.print("WAAGE:         g");

				Gewicht_alt = 0; // Gewicht_alt auf 0 setzen, damit Gewicht angezeigt wird

				Encoder_count_store = Encoder_count_neu;
				on_off_encoder = true; // Encoder Interrupt einschalten
				do
				{
					// Differenz der Gewichtseinheit minus der Leereinheitdurch,
					// Dividiert Korrekturfaktor
					Gewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;

					if (Gewicht != Gewicht_alt) // Anzeigen des Gewichtes nur wenn es sich geändert hat
					{
						Gewicht_alt = Gewicht;			  // Gewicht_alt aktualisieren
						lcd.setCursor(7, 1);			  // Setze Cursor auf die 8. Stelle der 2. Zeile
						lcd.print("    ");				  // Clear previous value
						lcd.setCursor(7, 1);			  // Setze Cursor auf die 8. Stelle der 2. Zeile
						lcd.print((unsigned int)Gewicht); // Gewicht anzeigen, casten auf unsigned int
					} // end if (Gewicht != Gewicht_alt)

				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				if (Encoder_count_neu > Encoder_count_store)
					Encoder_count_neu = RELAIS_TEST; // Encoderwert erhöhen
				else
					Encoder_count_neu = WAAGE_KALIBRIERUNG; // Encoderwert verringern

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				come_back(false); // Rückkehrerkennung einschalten ohne Ton

				break; //  end case WAAGE_TEST

			case RELAIS_TEST:
				lcd.print("RELAIS TEST :ENT");

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{
					Encoder_count_store = Encoder_count_neu;

					on_off_encoder = true; // Encoder Interrupt einschalten

					min_counter = 0;
					max_counter = anzahlrelais - 1;		 // zur Adressierug des relais Arryas
					Encoder_count_neu = min_counter;	 // Startwert für Encoder; unterschiedlich, damit beim ersten Durchgang sofort ausgeführt wird
					Encoder_count_alt = min_counter - 1; // Erststartbedingung für Encoder herstellen

					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("RELAIS TEST :I/O");

					on_off_encoder = true; // Encoder Interrupt einschalten

					do
					{
						if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value to serial
						{
							Encoder_count_alt = Encoder_count_neu;

							lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2

							switch (relais[Encoder_count_neu])
							{
							case RELAIS_ML:
								lcd.print("G.Mot.Links :ENT");
								break;
							case RELAIS_MM:
								lcd.print("G.Mot.Mitte :ENT");
								break;
							case RELAIS_MR:
								lcd.print("G.Mot.Recht :ENT");
								break;
							case RELAIS_RL:
								lcd.print("Rüttl.Links :ENT");
								break;
							case RELAIS_RM:
								lcd.print("Rüttl.Mitte :ENT");
								break;
							case RELAIS_RR:
								lcd.print("Rüttl.Rechts:ENT");
								break;
							case RELAIS_VL:
								lcd.print("W.Vent.Links:ENT");
								break;
							case RELAIS_VM:
								lcd.print("W.Vent.Mitte:ENT");
								break;
							case RELAIS_VR:
								lcd.print("W.Vent.Recht:ENT");
								break;
							case RELAIS_WP:
								lcd.print("H2O Pumpe   :ENT");
								break;
							default:
								lcd.print(Encoder_count_neu);
								lcd.print("  ?xx??xx??xx");
								break;
							} // end switch (Encoder_count_neu)
						} // end if (Encoder_count_neu != Encoder_count_alt)

						if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
						{
							on_off_encoder = false; // Encoder Interrupt ausschalten
							lcd.setCursor(0, 1);	// Setz Curser auf Charakter 1, Zeile 2
							lcd.print(relais[Encoder_count_neu]);

							digitalWrite(relais[Encoder_count_neu], EIN); // Relais einschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

							while (!digitalRead(ENTER_PIN))
							{
							} // Relais halten bis Enter Taste losgelassen

							digitalWrite(relais[Encoder_count_neu], AUS); // Relais ausschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

							on_off_encoder = true; // Encoder Interrupt einschalten
						} // if(!digitalRead(ENTER_PIN))

					} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

					/*	lcd.setCursor(0, 1); // Setze Cursor auf die 1. Stelle der 2. Zeile
						lcd.print(Encoder_count_neu);
						lcd.print("  //  "); // Clear previous value
						delay(1000);
					*/

					if (Encoder_count_neu != RELAIS_TEST)
					{
						if (Encoder_count_neu >= EEPROM_TEST && Encoder_count_neu < ARM_TEST)
							Encoder_count_neu = EEPROM_TEST; // Encoderwert erhöhen, (ACHTUNG Zählerübertrag)
						else
							Encoder_count_neu = WAAGE_TEST; // Encoderwert verringern
					}
				} // end if (!digitalRead(ENTER_PIN))

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				come_back(true); // Rückkehrerkennung einschalten mit Ton
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