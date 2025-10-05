#include "service.h"

// *********   Armpositionen einlesen und in Variable armposition speichern, negative Logik 	***************
void read_armposition()
{
	if (!digitalRead(AL)) // Negative Logik (LOW = aktiv)
		armposition = ARM_LINKS;
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
		} //  end else if (!digitalRead(AM))
	} //  end else if (!digitalRead(AL))
} //  end read_armposition()
/****************************    ende read_armposition() 	********************************/

/*************************   Anfang  Spielt eine Melodie 	****************************/
void Musik(int Melodie)
{
	switch (Melodie)
	{
	case MELODIE_ANFANG:
		tone(TONE_PIN, 523); // C5 (Do)
		delay(300);
		tone(TONE_PIN, 784); // G5 (Sol)
		delay(200);
		noTone(TONE_PIN);
		delay(400);
		break;

	case MELODIE_ENDE:
		tone(TONE_PIN, 392); // G4 (Sol)
		delay(300);
		tone(TONE_PIN, 587); // D5 (Re)
		delay(200);
		noTone(TONE_PIN);
		delay(400);
		break;

	case MELODIE_OK:
		tone(TONE_PIN, 440); // A4 (La)
		delay(300);
		tone(TONE_PIN, 660); // E5 (Höhe)
		delay(200);
		noTone(TONE_PIN);
		delay(400);
		break;

	case MELODIE_ENTER:
		tone(TONE_PIN, 659); // E5 (Mi)
		delay(300);
		tone(TONE_PIN, 880); // A5 (La)
		delay(200);
		noTone(TONE_PIN);
		delay(400);
		break;

	case MELODIE_FEHLER:
		tone(TONE_PIN, 330); // E4 (Mi)
		delay(300);
		tone(TONE_PIN, 440); // A4 (La)
		delay(200);
		noTone(TONE_PIN);
		delay(400);
		break;

	default:
		break;

	} // end switch (Melodie)
} //  end Musik(int Melodie)
/********************   Ende  Spielt eine Melodie 	******************************/

/**********   für die Standardwertherstellung in der Procedure serrvice() 	***************/
void come_back()
{
	lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("Hardw. TEST :I/O"); //  Text in erster Zeile anzeigen
	lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2

	min_counter = 0;				// Minimalwert für Encoder
	max_counter = anzahl_tests - 1; // Maximalwert für Encoder

	on_off_encoder = true; // Encoder Interrupt einschalten
} //  end come_back()
/**********   für die Standardwertherstellung in der Procedure serrvice() 	***************/

//  ************************** Anfang Service **************************************
void service()
{
	// Variablen für "Bechererkennung" auf LCD Schreiberkennung
	bool bl_LCD_NO, bm_LCD_NO, br_LCD_NO, bl_LCD_OK, bm_LCD_OK, br_LCD_OK;

	lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("    SERVICE     "); // Text in zweiter Zeile anzeigen
	lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("    Routine     "); // Text in zweiter Zeile anzeigen

	Musik(MELODIE_ANFANG);

	while (!digitalRead(ENTER_PIN) && !digitalRead(I_O_PIN)) // warten bis Taste ENTER und I/O losgelasen wird
	{
	};
	delay(5); // Entprellzeit

	come_back(); // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften

	Encoder_count_neu = EEPROM_TEST;
	Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

	do
	{
		if (Encoder_count_neu != Encoder_count_alt) // If count has changed
		{
			Encoder_count_alt = Encoder_count_neu;

			switch (test_routinen[Encoder_count_neu]) //  Menüauswahl Zeile 1
			{
			case EEPROM_TEST: /*  DAUERDURCHLAUF  */
				if (!digitalRead(I_O_PIN))
					return; // Bedinerabbruch, Servicemenü verlassen

				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
				lcd.print("EEPROM TEST :ENT"); // Text in zweiter Zeile anzeigen

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{
					lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("EEPROM TEST :I/O"); // Text in erster Zeile anzeigen
					lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("Adr:     Dat    "); // Text in zweiter Zeile anzeigen");

					// Min- Maxwerte für Encoder setzen
					min_counter = 0;
					max_counter = MAX_EEPROM_ADRESSE; // Mega hat 4 KB (4096 Byts, 0-4095 bytes) EEPROM Zellen

					Encoder_count_neu = min_counter;
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

							lcd.setCursor(12, 1);					   // Setz Curser auf Charakter 13, Zeile 2
							lcd.print("    ");						   // Lösche die Zeile
							lcd.setCursor(12, 1);					   // Setz Curser auf Charakter 13, Zeile 2
							lcd.print(EEPROM.read(Encoder_count_neu)); // Daten der Adresse (FF = leer) anzeigen
							lcd.setCursor(15, 1);
							lcd.print(char(EEPROM.read(Encoder_count_neu))); // Daten der Adresse als charakter anzeigen
						} // end if (Encoder_count_neu != Encoder_count_alt)
					} while (digitalRead(I_O_PIN)); // solange I/O Taste nicht gedrückt ist, also high ist

					while (!digitalRead(I_O_PIN)) // warten bis I/O Taste losgelasen
					{
					} // end while (!digitalRead(I_O_PIN))

					Musik(MELODIE_ENDE);
					Encoder_count_neu = EEPROM_TEST; // Bedienerabbruch, Verbleib im Menü EEPROM_TEST

				} //  end  if (!digitalRead(ENTER_PIN))

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end case EEPROM_TEST

			case LED_TEST:					   /*  IMMER NUR EIN Programm DURCHLAUF  */
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

					while (Encoder_count_neu == Encoder_count_alt &&  // logische UND
						   (millis() - start_time < WAIT_TIME_LED) && // 750 ms Sekunden warten
						   digitalRead(I_O_PIN))					  // solange I/O Taste nicht gedrückt ist, also high ist
					{
					} // warten bis Encoder den Wert geändert hat oder die Zeit vergangen ist oder I/O Taste gedrückt ist

					if (digitalRead(I_O_PIN))
					{
						lcd.setCursor(13, 1); // Setz Curser auf Charakter 14, Zeile 2
						lcd.print("EIN");
						digitalWrite(LL, EIN);
						digitalWrite(LM, EIN);
						digitalWrite(LR, EIN);

						start_time = millis();

						while (Encoder_count_neu == Encoder_count_alt &&  // logisches UND
							   (millis() - start_time < WAIT_TIME_LED) && // 750 ms Sekunden warten
							   digitalRead(I_O_PIN))					  // solange I/O Taste nicht gedrückt ist, also high ist
						{

						} // warten bis Encoder den Wert geändert hat oder die Zeit vergangen ist oder I/O Taste gedrückt ist

					} // end if (digitalRead(I_O_PIN))

					if (!digitalRead(I_O_PIN))
						return; // Bedinerabbruch, Servicemenü verlassen

				} while (Encoder_count_neu == Encoder_count_alt); //  solange bis encoder den Wert geändert hat

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end case LED_TEST

			case BECHER_TEST: /*  IMMER NUR EIN Programm DURCHLAUF  */
				bl_LCD_OK = false;
				bm_LCD_OK = false;
				br_LCD_OK = false;
				bl_LCD_NO = false;
				bm_LCD_NO = false;
				br_LCD_NO = false;

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

					if (!digitalRead(I_O_PIN))
						return; // Bedinerabbruch, Servicemenü verlassen

				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end case BECHER_TEST

			case TON_TEST:					   /*  IMMER NUR EIN Programm DURCHLAUF  */
				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("TON TEST    :I/O"); // Text in erster Zeile anzeigen
				lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 2, Zeile 1
				lcd.print("     Sirene     "); // Text in zweiter Zeile anzeigen

				do
				{
					for (int i = 200; i <= 4000; i = i + 2) // Sirene aufwärts
					{
						tone(TONE_PIN, i);
						delay(2); // 2 ms warten
						if (Encoder_count_neu != Encoder_count_alt || !digitalRead(I_O_PIN))
							break;
					} //  end  for (int i = 200; i <= 4000; i = i + 2) // Sirene aufwärts

					if (Encoder_count_neu == Encoder_count_alt && !digitalRead(I_O_PIN))
						for (int i = 4000; i >= 200; i = i - 2) // Sirene abwärts
						{
							tone(TONE_PIN, i);
							delay(2); // 2 ms warten
							if (Encoder_count_neu != Encoder_count_alt || !digitalRead(I_O_PIN))
								break;
						} //  end  for (int i = 4000; i >= 200; i = i - 2) // Sirene abwärts

				} while (Encoder_count_neu == Encoder_count_alt && digitalRead(I_O_PIN)); // warten bis Encoder den Wert geändert hat

				noTone(TONE_PIN); // Alle Töne ausschalten

				if (!digitalRead(I_O_PIN))
					return; // Bedinerabbruch, Servicemenü verlassen

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end case TON_TEST

			case ARM_TEST:					   /*  IMMER NUR EIN Programm DURCHLAUF  */
				lcd.setCursor(0, 0);		   // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("ARM TEST    :I/O"); // Text in erster Zeile anzeigen

				armposition = ARM_NO_POS;		// keine Position, Startwert
				armposition_alt = OUT_OF_RANGE; // Startwert für Erstanfang

				do
				{
					read_armposition(); // Armposition einlesen

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

					} // end if (armposition != armposition_alt)

					if (!digitalRead(I_O_PIN))
						return; // Bedinerabbruch, Servicemenü verlassen

				} while (Encoder_count_neu == Encoder_count_alt); // warten bis Encoder den Wert geändert hat

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end case ARM_TEST

			case WAAGE_KALIBRIERUNG: /*  DAUERDURCHLAUF  */
				if (!digitalRead(I_O_PIN))
					return; // Bedinerabbruch, Servicemenü verlassen

				lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 1
				lcd.print("W. KALIBRI. :ENT");

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{
					on_off_encoder = false; // Encoder Interrupt ausschalten

					lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
					lcd.print("Tara        :I/O");
					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print(" wird ermittelt ");

					while (!scale.is_ready() && digitalRead(I_O_PIN)) // scale.is_ready --> Waage ist bereit wenn true
					{
					} // warten bis die Waage bereit ist oder Bedienrabbruch

					if (digitalRead(I_O_PIN)) // weiter wenn I/O nicht gedrückt wurde
					{
						Leergew_einheiten = scale.read_average(20); // Leergewicht einlesen, Durchschnitt von 20 Messungen

						lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
						lcd.print("            :ENT");
						lcd.setCursor(0, 1);				// Setz Curser auf Charakter 1, Zeile 2
						lcd.print(long(Leergew_einheiten)); // Leergewicht anzeigen ohne Kommastellen

						while (digitalRead(ENTER_PIN) && digitalRead(I_O_PIN)) // negatve Logik, warten auf Bestätigung des Leergewichtwertes,
						{													   // weiter mit ENTER oder I/O Taste (Bedienrabbruch)
						} // zum ansehen der Leergewichteinheiten, weiter mit ENTER oder I/O Taste (Bedienrabbruch)

						if (!digitalRead(ENTER_PIN)) // weiter wenn ENTER gedrückt wurde
						{
							lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
							lcd.print("G. auflegen :I/O");
							lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
							lcd.print("bereit ?    :ENT");

							while (!digitalRead(ENTER_PIN)) // warten bis Enter taste losgelasen wird
							{
								// Wird nicht auf I/O Taste (Bedienrabbruch) getestet
							};
							delay(5); // Entprellzeit

							while (digitalRead(ENTER_PIN) && digitalRead(I_O_PIN))
							{
							} // auf Bestätigung des Gewichtauflegen warten, weiter mit ENTER oder I/O Taste (Bedienrabbruch)

							if (!digitalRead(ENTER_PIN)) // weiter wenn ENTER gedrückt wurde
							{
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
									// Wird nicht auf I/O Taste (Bedienrabbruch) getestet
								};
								delay(5); // Entprellzeit

								on_off_encoder = true;								   // Encoder Interrupt einschalten
																					   // Sollgewicht eingeben
								while (digitalRead(ENTER_PIN) && digitalRead(I_O_PIN)) // warten auf Bestätigung des eingegebenen Gewichtwertes, weiter mit ENTER oder I/O Taste )
								{

									if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value
									{
										Encoder_count_alt = Encoder_count_neu;

										lcd.setCursor(6, 1);		  // Set cursor to column 7, row 2
										lcd.print("    ");			  // Clear previous value
										lcd.setCursor(6, 1);		  // Set cursor to column 7, row 2
										lcd.print(Encoder_count_neu); // im Encoder_count_neu steht das Gewicht in Gramm
									} // end if (Encoder_count_neu != Encoder_count_alt)

								} // end  while (digitalRead(ENTER_PIN) && digitalRead(I_O_PIN))
								// Bestätigung des eingegebenen Gewichtwertes, weiter mit ENTER oder I/O Taste (Bedienerabbruch)

								if (digitalRead(I_O_PIN)) // weiter wenn I/O nicht gedrückt wurde
								{
									on_off_encoder = false; // Encoder Interrupt ausschalten

									while (!scale.is_ready() && digitalRead(I_O_PIN)) // scale.is_ready --> Waage ist bereit wenn true
									{
									} // warten bis die Waage bereit ist oder I/O Taste gedrückt wird

									if (digitalRead(I_O_PIN)) // weiter wenn I/O nicht gedrückt wurde
									{
										Eichgew_einheiten = scale.read_average(3); // Waagenwert einlesen, Durchschnitt von 3 Messungen

										// Differenz der Gewichtseinheit minus der Leereinheitdurch,
										// Dividiert durch das eingegeben Gewicht in Gramm (Encoder_count_neu)
										Korrekturfaktor = (Eichgew_einheiten - Leergew_einheiten) / Encoder_count_neu;

										////////// Korekturwert und Leergew_einheiten im EEPROM abspeichern //////////

										// Schreibt die Variable Korrekturfaktor auf die EEPROM Adsresse:
										// anzahl_daten + EEPROM_ADRESSABSTAND
										// ist die Anzahl (Adresse) der im EEPROM bereits benutzten Datenbytes
										EEPROM.put(anzahl_daten + EEPROM_ADRESSABSTAND, Korrekturfaktor);

										// Schreibt die Variable Leergew_einheiten auf die EEPROM Adsresse:
										// anzahl_daten + EEPROM_ADRESSABSTAND + EEPROM_ADRESSABSTAND
										// ist die Anzahl (Adresse) der im EEPROM bereits benutzten Datenbytes
										EEPROM.put(anzahl_daten + EEPROM_ADRESSABSTAND + EEPROM_ADRESSABSTAND, Leergew_einheiten);

										Musik(MELODIE_ENTER);
									} // end if (digitalRead(I_O_PIN))
								} // end if (digitalRead(I_O_PIN))
							} // end if (!digitalRead(ENTER_PIN))
						} // end if (!digitalRead(ENTER_PIN))
					} // end if (digitalRead(I_O_PIN))

					while (!digitalRead(I_O_PIN)) // warten bis I/O Taste losgelasen wird
					{
					}
					delay(5); // Entprellzeit

					come_back();							// Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
					Encoder_count_neu = WAAGE_KALIBRIERUNG; // Verbleib im Menü WAAGE_KALIBRIERUNG
				} // end if (!digitalRead(ENTER_PIN))

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
				break;							  //  end case WAAGE_KALIBRIERUNG

			case WAAGE_TEST: /*  IMMER NUR EIN Programm DURCHLAUF  */
				if (!digitalRead(I_O_PIN))
					return; // Bedinerabbruch, Servicemenü verlassen

				lcd.print("WAAGE:         g");

				Gewicht_alt = 0; // Gewicht_alt auf 0 setzen, damit Gewicht angezeigt wird

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

				} while (Encoder_count_neu == Encoder_count_alt && digitalRead(I_O_PIN)); // warten bis Encoder den Wert geändert hat oder Bedienerabbruch

				if (Encoder_count_neu != Encoder_count_alt)
					come_back(); // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften

				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end case WAAGE_TEST

			case RELAIS_TEST: /*  DAUERDURCHLAUF  */
				if (!digitalRead(I_O_PIN))
					return; // Bedinerabbruch, Servicemenü verlassen

				lcd.print("RELAIS TEST :ENT");

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{

					while (!digitalRead(ENTER_PIN)) // warten bis Enter taste losgelasen wird
					{
						// Wird nicht auf I/O Taste (Bedienrabbruch) getestet
					};
					delay(5); // Entprellzeit

					min_counter = 0;
					max_counter = anzahlrelais - 1;	  // zur Adressierug des relais Arryas
					Encoder_count_neu = min_counter;  // Startwert für Encoder; unterschiedlich, damit beim ersten Durchgang sofort ausgeführt wird
					Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung für Encoder herstellen

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
								lcd.print("Ruett.Links :ENT");
								break;
							case RELAIS_RM:
								lcd.print("Ruett.Mitte :ENT");
								break;
							case RELAIS_RR:
								lcd.print("Ruett.Rechts:ENT");
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
								lcd.print("ER: case Relai");
								break;
							} // end switch (Encoder_count_neu)
						} // end if (Encoder_count_neu != Encoder_count_alt)

						if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
						{
							on_off_encoder = false; // Encoder Interrupt ausschalten
													//						lcd.setCursor(0, 1);	// Setz Curser auf Charakter 1, Zeile 2
													//						lcd.print(relais[Encoder_count_neu]);

							digitalWrite(relais[Encoder_count_neu], EIN); // Relais einschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

							while (!digitalRead(ENTER_PIN) && digitalRead(I_O_PIN)) // warten bis Enter taste losgelasen
							{
							} // Relais halten bis Enter Taste losgelassen

							digitalWrite(relais[Encoder_count_neu], AUS); // Relais ausschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

							on_off_encoder = true; // Encoder Interrupt einschalten
						} // end  if (!digitalRead(ENTER_PIN))

					} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

					Musik(MELODIE_ENDE);
					Encoder_count_neu = RELAIS_TEST; // Bedienerabbruch, Verbleib im Menü RELAIS_TEST

				} // end  if (!digitalRead(ENTER_PIN))

				while (!digitalRead(I_O_PIN)) // warten bis I/O Taste losgelasen wird
				{
				} // end   while (!digitalRead(I_O_PIN))

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung äussere Schleife herstellen

				break; //  end case RELAIS_TEST

			case DATA_RESET: /*  DAUERDURCHLAUF  */
				if (!digitalRead(I_O_PIN))
					return; // Bedinerabbruch, Servicemenü verlassen

				lcd.print("DATA Reset  :ENT");

				if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
				{

					while (!digitalRead(ENTER_PIN)) // warten bis Enter taste losgelasen wird
					{
						// Wird nicht auf I/O Taste (Bedienrabbruch) getestet
					};
					delay(5); // Entprellzeit

					lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
					lcd.print("Sicher?     :ENT");

					do
					{
						if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
						{
							//  Keine I/O Prüfung, EEPROM Aktivitäten werden nicht unterbrochen
							on_off_encoder = false; // Encoder Interrupt ausschalten

							//  lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
							//  lcd.print("warten  loeschen");

							//  EEPROM komplett löschen
							//	for (int i = 0; i <= MAX_EEPROM_ADRESSE; i++)
							//		EEPROM.update(i, 255); // EEPROM löschen (FF = leer)

							//	Musik(MELODIE_OK);

							lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
							lcd.print("warten programm");

							//  erstmalige Dateneingabe initialisieren
							for (int i = 0; i < ARM_ANZAHL; i++) // 0 bis 2 Arme (3 Arme)
							{
								strcpy(daten[i].ueberschrift, "UEBERSCHRIFT1"); // Initialisierung der Überschrift - 13 Caraktere

								for (int j = 0; j < (MAX_GEWICHTANZAHL - 1); j++) // 0 bis 9 Gewichte pro Arm (10 Gewichte)
									daten[i].gewicht[j] = 1;			 // Initialisierung der Gewichte in g, nicht 0 wegen Division

								// Vorfixierte Gewichte in g
								if (i == 1) // Bei ARM mitte
								{
									daten[i].gewicht[2] = FIXGEWICHT_01;
									daten[i].gewicht[3] = FIXGEWICHT_02;
									daten[i].gewicht[4] = FIXGEWICHT_03;
									daten[i].gewicht[5] = FIXGEWICHT_04;
								}//  end if (i == 1)
								else
								{ // Bei ARM links und rechts
									daten[i].gewicht[2] = FIXGEWICHT_11;
									daten[i].gewicht[3] = FIXGEWICHT_12;
									daten[i].gewicht[4] = FIXGEWICHT_13;
									daten[i].gewicht[5] = FIXGEWICHT_14;
								}  // end else if (i == 1)
							} // end for (int i = 0; i < ARM_ANZAHL; i++)

							// Datenspeicherung im EEPROM:
							EEPROM.put(0, daten); // Schreiben der komletten Variablen (Datenstruktur) Daten auf den EEPROM, ab Adresse 0

							Musik(MELODIE_OK);

							on_off_encoder = true; // Encoder Interrupt einschalten

							break; // bricht die do while (digitalRead(I_O_PIN)) ab
						} // end  if (!digitalRead(ENTER_PIN))
					} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

					Musik(MELODIE_ENDE);

					Encoder_count_neu = DATA_RESET; // Verbleib im Menü DATA_RESET

				} // end  if (!digitalRead(ENTER_PIN))

				come_back();					  // Rückkehrerkennung, Encoder Grenzen setzen und interrupt zulassen, erste LCD-Zeile Beschriften
				Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

				break; //  end  case DATA_RESET

			default:
				lcd.print(Encoder_count_neu);
				lcd.print("ER: case Servi");
				break; //  end default

			} // end switch (Encoder_count_neu)
		} // end if (Encoder_count_neu != Encoder_count_alt)
	} while (digitalRead(I_O_PIN)); // solange I/O Taste nicht gedrückt wurde

} // end service

//  ************************** Service Ende **************************************