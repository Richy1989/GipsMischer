#include "service.h"

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
		// Serial.println("Interrupt");

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
	Serial.println();
	Serial.println("jetzt gehts los");

	// Eingänge zur Armposition Erkennung:
	pinMode(AL, INPUT_PULLUP); // Eingang Armposition LINKS
	pinMode(AM, INPUT_PULLUP); // Eingang Armposition MITTE
	pinMode(AR, INPUT_PULLUP); // Eingang Armposition RECHTS

	// Eingänge zur Gipsbecher Erkennung:
	pinMode(BL, INPUT_PULLUP); // Eingang Armposition LINKS, pullup
	pinMode(BM, INPUT_PULLUP); // Eingang Armposition MITTE, pullup
	pinMode(BR, INPUT_PULLUP); // Eingang Armposition RECHTS, pullup

	// Ausgang zur Gipsbecher Erkennung:
	pinMode(TONE, OUTPUT); // Ausgang zur Tone Erzeugung

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
	for (int i = 0; i < anzahlrelais; i++) // Pointer von 0 bis 9
	{
		pinMode(relais[i], OUTPUT);	  // Ausgänge der Relais als OUTPUT konfigutieren
		digitalWrite(relais[i], AUS); // Relais ausschalten (negative Logik: Ausgang ist also high, wenn Relais AUS ist)
	}

	// Front LED ausschalten. Verkehrte Logik: LOW = EIN, HIGH = AUS
	digitalWrite(LL, EIN);
	digitalWrite(LM, EIN);
	digitalWrite(LR, EIN);

	//  attach interrupt-routin,
	//  Interrupt 2, Pin 19, Stützpunkt Pin 46, FlBaKa Pin 17
	attachInterrupt(digitalPinToInterrupt(ENCODER_A), read_encoder, CHANGE);

	//  attach interrupt-routin,
	//  Interrup 3, Pin 18, Stützpunkt Pin 47, FlBaKa Pin 18
	attachInterrupt(digitalPinToInterrupt(ENCODER_B), read_encoder, CHANGE);

	// Initialisierung LCD
	lcd.begin(LCD_CHARACTERS, LCD_ZEILEN); // 16 Charakters, 2 Zeilen

	// Initialisierung Waage
	// Eingang/Ausgang für I2C Bus bei HX711 (Interruptfähige Eingänge)
	// INPUT oder OUTPUT wird durch Library HX711.h gesetzt
	// pinMode(DATA_PIN, OUTPUT); // Daten PIN
	// pinMode(CLOCK_PIN, OUTPUT); // Clock PIN
	scale.begin(DATA_PIN, CLOCK_PIN); // dataPin =20, clockPin = 21, gain = 128 / Interruptfähige Pins

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
	// Anfang LCD Testroutine

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
	   //  lcd.begin(LCD_CHARACTERS, LCD_ZEILEN);                //16 Charakters, 2 Zeilen
	   //}  //  end if (start == 1)


	   //Serial.println("läuft normal");


	  //Ende LCD Testroutine
	*/

	/*
	int wert; // ausgelesener Wert

	  pinMode(A0, INPUT_PULLUP);    // Ausgang als INPUT konfigutieren
	  pinMode(A9, INPUT_PULLUP);    // Ausgang als INPUT konfigutieren

	  for (int i = 0; i < 10; i++)   //  Ausgangspointer zeigt auf Array 0 bis 9
	  {
		 pinMode(relais[i], OUTPUT);    // Ausgänge der Relais als OUTPUT konfigutieren
		 digitalWrite(relais[i], AUS);  // Relais ausschalten
	  }

	  lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	  lcd.print("Ausgang");
	  lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 1
	  lcd.print("Rotation");

	  wert = 14;

	  do
		{
		  wert=wert+1;  //Ausgangspointer hochzählen

		  if (wert > 9)  // Wenn Ausgangspointer größer als 9
			{
			  wert = 0; // Wert auf 0 setzen, wenn 10 erreicht ist
			  lcd.setCursor(8, 0);
			  lcd.print("xxxxx");
		   }

		  lcd.setCursor(8, 0);
		  lcd.print(wert);

		  for (int i = 1; i < 10; i++)  // 10 mal pulsen
		   {
			 lcd.setCursor(9, 1);
			 lcd.print(i);

			 digitalWrite(relais[wert], EIN); // Relais 1 einschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS
			 delay (500); // 100 ms warten
			 digitalWrite(relais[wert], AUS); // Relais 1 ausnschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS
			 delay (500); // 100 ms warten
			}

	} while (true);  //

	//  Ende Relais Sondertest  **************************************
	*/

	////////////////////////////////// Test EEPROM Anfang ///////////////////////////////////////////
	service(); // Aufruf der EEPROM Testfunktion
				   ////////////////////////////////// Test EEPROM Ende ///////////////////////////////////////////

	//  ************************** Encoder Testroutine Anfang **************************************
	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("ENCODER   P: I/O");
	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("ENC: ");

	min_counter = 0;
	max_counter = MAX_GEWICHT;			 // Maximales messbares Gewicht in Gramm
	Encoder_count_neu = min_counter;	 // Startwert für Encoder
	Encoder_count_alt = min_counter - 1; // Startwert für Encoder
	on_off_encoder = true;			 // Encoder Interrupt einschalten

	do
	{

		if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value to serial
		{
			Serial.println(Encoder_count_neu);
			Encoder_count_alt = Encoder_count_neu;

			lcd.setCursor(5, 1); // Setz Curser auf Charakter 6, Zeile 2
			lcd.print("   ");	 // Lösche die Zeile
			lcd.setCursor(5, 1); // Setz Curser auf Charakter 6, Zeile 2
			lcd.print(Encoder_count_neu);
		} // end if (Encoder_count_neu != Encoder_count_alt)
	} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

	on_off_encoder = false; // Encoder Interrupt ausschalten

	//  ************************** Encoder Testroutine Ende **************************************

	//  ************************** Armposition Erkennung Anfang **************************************
	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("  weiter ENTER  ");
	lcd.setCursor(0, 1);		   // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("                "); // Lösche die Zeile

	Anzeige = ARM_NO_POS; // Startwert für Armposition
	Anzeige_alt = 255;	  // Startwert für Anzeige

	do //  ************************** Armposition Erkennung Beginn **************************************
	{
		if (!digitalRead(AL))
			Anzeige = ARM_LINKS; // Negative Logik
		else
		{
			if (!digitalRead(AM))
				Anzeige = ARM_MITTE;
			else
			{
				if (!digitalRead(AR))
					Anzeige = ARM_RECHTS;
				else
					Anzeige = ARM_NO_POS;
			} // end else if (!digitalRead(AM))
		} // end else if (!digitalRead(AL))

		if (Anzeige != Anzeige_alt)
		{
			lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2

			switch (Anzeige)
			{
			case ARM_LINKS:
				lcd.print("AL");
				break;
			case ARM_MITTE:
				lcd.print("AM");
				break;
			case ARM_RECHTS:
				lcd.print("AR");
				break;
			case ARM_NO_POS:
				lcd.print("NA");
				break;
			default:
				lcd.print("xx");
				break;
			} // end switch (Anzeige)

			Anzeige_alt = Anzeige;
		}
	} while (digitalRead(ENTER_PIN)); // solange Entertaste nicht gedrückt ist, also high ist
									  //  ************************** Armposition Erkennung Ende **************************************

	//  ************************** Becher Erkennung Anfang **************************************
	Anzeige_alt = 255;
	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("  weiter WAAGE  ");

	do
	{
		if (!digitalRead(BL))
			Anzeige = BECHER_LINKS;
		else if (!digitalRead(BM))
			Anzeige = BECHER_MITTE;
		else if (!digitalRead(BR))
			Anzeige = BECHER_RECHTS;
		else
			Anzeige = NO_BECHER;

		if (Anzeige != Anzeige_alt)
		{
			lcd.setCursor(3, 1); // Setz Curser auf Charakter 4, Zeile 2

			switch (Anzeige)
			{
			case BECHER_LINKS:
				lcd.print("BL");
				break;
			case BECHER_MITTE:
				lcd.print("BM");
				break;
			case BECHER_RECHTS:
				lcd.print("BR");
				break;
			case NO_BECHER:
				lcd.print("NB");
				break;
			default:
				lcd.print("xx");
				break;
			} // end switch (Anzeige)

			Anzeige_alt = Anzeige;
		}
	} while (digitalRead(WAAGE_PIN)); // solange Waage Taste nicht gedrückt ist, also high ist
									  //  ************************** Becher Erkennung Ende **************************************

	//  ************************** LED Front Anfang **************************************
	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("  weiter  I/O   ");

	do
	{
		lcd.setCursor(7, 1); // Setz Curser auf Charakter 8, Zeile 2
		lcd.print("LL");
		digitalWrite(LL, AUS);
		delay(750);
		digitalWrite(LL, EIN);

		lcd.setCursor(7, 1); // Setz Curser auf Charakter 8, Zeile 2
		lcd.print("LM");
		digitalWrite(LM, AUS);
		delay(750);
		digitalWrite(LM, EIN);

		lcd.setCursor(7, 1); // Setz Curser auf Charakter 8, Zeile 2
		lcd.print("LR");
		digitalWrite(LR, AUS);
		delay(750);
		digitalWrite(LR, EIN);
	} while (digitalRead(I_O_PIN)); // solange I/O Taste nicht gedrückt ist, also high ist
	//  ************************** LED Front Ende **************************************

	//  ************************** Buzzer Anfang **************************************
	lcd.setCursor(0, 0); // Set cursor to column 1, row 1
	lcd.print("  weiter Enter  ");

	lcd.setCursor(11, 1); // Setz Curser auf Charakter 10, Zeile 2
	lcd.print("BZ");

	do
	{
		// Sequenz für POSITIVES Ereignis

		// Die Frequenzen repräsentieren die Noten der Tonleiter (C4, D4, E4, F4, G4).
		tone(TONE, 523); // C4 (Do)
		delay(200);		 // 300 ms warten
		tone(TONE, 587); // D4 (Re)
		delay(200);		 // 300 ms warten
		tone(TONE, 659); // E4 (Mi)
		delay(200);		 // 300 ms warten
		tone(TONE, 698); // F4 (Fa)
		delay(200);		 // 300 ms warten
		tone(TONE, 784); // G4 (Sol)
		delay(200);		 // 300 ms warten

		noTone(TONE); // Alle Töne ausschalten

		delay(750); // Warte 750 ms

		// Sequenz für NEGATIVES Ereignis

		tone(TONE, 261); // C4 (Do)
		delay(400);		 // 400 ms warten
		tone(TONE, 220); // A3 (La)
		delay(400);		 // 400 ms warten
		tone(TONE, 196); // G3 (Sol)
		delay(400);		 // 400 ms warten
		tone(TONE, 174); // F3 (Fa)
		delay(400);		 // 400 ms warten
						 // tone(TONE, 165); // E3 (Mi)
						 // delay(400);            // 400 ms warten
						 //  tone(TONE, 147); // D3 (Re)
		// delay(400);            // 400 ms warten

		noTone(TONE); // Alle Töne ausschalten

		delay(750); // Warte 750 ms

	} while (digitalRead(ENTER_PIN)); // solange Enter Taste nicht gedrückt ist, also high ist
	//  ************************** Buzzer Ende **************************************

	//  ************************** Waage Kalibrierung Anfang **************************************

	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("LeerGew. P:Waage");
	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print(" wird ermittelt ");
	while (digitalRead(WAAGE_PIN))
	{
	} // warten bis Waage Taste gedrückt wird

	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("  bitte warten  ");

	while (scale.is_ready())
	{
	} // warten bis die Waage bereit ist
	Leergew_einheiten = scale.read_average(20); // Leergewicht einlesen, Durchschnitt von 20 Messungen
	lcd.setCursor(0, 0);						// Setz Curser auf Charakter 1, Zeile 1
	lcd.print("LeerGewich P:I/O");
	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("  gespeichert   ");
	while (digitalRead(I_O_PIN))
	{
	} // warten bis I/O Taste gedrückt wird

	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("LeerGew. P:Enter");
	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("LeerGew:");
	lcd.setCursor(8, 1); // Setz Curser auf Charakter 9, Zeile 2
	lcd.print(Leergew_einheiten);
	while (digitalRead(ENTER_PIN))
	{
	} // warten bis Enter Taste gedrückt wird

	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("Gewicht auflegen");
	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("ready WaageTaste");
	while (digitalRead(WAAGE_PIN))
	{
	} // warten bis Waage Taste gedrückt wird

	lcd.setCursor(0, 0); // Set cursor to column 1, row 1
	lcd.print("Gewicht  P:Enter");
	lcd.setCursor(0, 1); // Set cursor to column 1, row 2
	lcd.print("Tara Soll:     g");

	min_counter = 0;
	max_counter = MAX_GEWICHT;		 // Maximales messbares Gewicht in Gramm
	Encoder_count_neu = min_counter; // Gewicht des Sollgewichtes in Gramm
	Encoder_count_alt = min_counter - 1;

	on_off_encoder = true; // Encoder Interrupt einschalten

	do
	{
		if (Encoder_count_neu != Encoder_count_alt) // If count has changed print the new value
		{
			Encoder_count_alt = Encoder_count_neu;

			lcd.setCursor(11, 1);		  // Set cursor to column 11, row 2
			lcd.print("    ");			  // Clear previous value
			lcd.setCursor(11, 1);		  // Set cursor to column 11, row 2
			lcd.print(Encoder_count_neu); // im Encoder_count_neu steht der Gewichtswert in Gramm
		} // end if (Encoder_count_neu != Encoder_count_alt)
	} while (digitalRead(ENTER_PIN)); // warten bis Enter Taste gedrückt wird

	on_off_encoder = false; // Encoder Interrupt ausschalten
								 //  ************************** Waage Kalibrierung Ende **************************************

	//  ************************** Wiegefunktion Anfang **************************************

	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("Messung  P:Waage");

	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("Initialisierung ");

	while (scale.is_ready())
	{
	} // warten bis die Waage bereit ist

	Eichgew_einheiten = scale.read_average(3); // Waagenwert einlesen, Durchschnitt von 3 Messungen

	// Differenz der Gewichtseinheit minus der Leereinheitdurch,
	// Dividiert durch das eingegeben Gewicht in Gramm (Encoder_count_neu)
	Korrekturfaktor = (Eichgew_einheiten - Leergew_einheiten) / Encoder_count_neu;

	Gewicht = 0;	  // Gewicht auf 0 setzen
	Gewicht_alt = -1; // Gewicht_alt auf -1 setzen

	lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
	lcd.print("Gewicht:        g");

	do
	{
		while (scale.is_ready())
		{
		} // warten bis die Waage bereit ist

		Gewicht = (scale.read() - Leergew_einheiten) / Korrekturfaktor;

		if (Gewicht != Gewicht_alt) // Anzeigen des Gewichtes nur wenn es sich geändert hat
		{
			Gewicht_alt = Gewicht;			  // Gewicht_alt aktualisieren
			lcd.setCursor(9, 1);			  // Setze Cursor auf die 9. Stelle der 2. Zeile
			lcd.print("    ");				  // Clear previous value
			lcd.setCursor(9, 1);			  // Setze Cursor auf die 9. Stelle der 2. Zeile
			lcd.print((unsigned int)Gewicht); // Gewicht anzeigen, casten auf unsigned int
		} // end if (Gewicht != Gewicht_alt)

	} while (digitalRead(WAAGE_PIN));

	//  ************************** Wiegefunktion Ende **************************************

	//  ************************** Relais Test Anfang **************************************

	min_counter = 0;
	max_counter = anzahlrelais - 1;		 // zur Adressierug des relais Arryas
	Encoder_count_neu = min_counter;	 // Startwert für Encoder; unterschiedlich, damit beim ersten Durchgang sofort ausgeführt wird
	Encoder_count_alt = min_counter - 1; // Erststartbedingung für Encoder herstellen

	lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
	lcd.print("Test:ENT end:I/O");

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
				lcd.print("Gips Motor Links");
				break;
			case RELAIS_MM:
				lcd.print("Gips Motor Mitte");
				break;
			case RELAIS_MR:
				lcd.print("Gips Motor Recht");
				break;
			case RELAIS_RL:
				lcd.print(" Ruettler Links ");
				break;
			case RELAIS_RM:
				lcd.print(" Ruettler Mitte ");
				break;
			case RELAIS_RR:
				lcd.print(" Ruettler Rechts");
				break;
			case RELAIS_VL:
				lcd.print("H2O Ventil Links");
				break;
			case RELAIS_VM:
				lcd.print("H2O Ventil Mitte");
				break;
			case RELAIS_VR:
				lcd.print("H2O Ventil Recht");
				break;
			case RELAIS_WP:
				lcd.print("   H2O Pumpe    ");
				break;
			default:
				lcd.print("xx??xx");
				break;
			} // end switch (Encoder_count_neu)
		} // end if (Encoder_count_neu != Encoder_count_alt)

		if (!digitalRead(ENTER_PIN)) // Wenn Entertaste gedrückt ist, also low
		{
			on_off_encoder = false; // Encoder Interrupt ausschalten
			lcd.setCursor(0, 1);		 // Setz Curser auf Charakter 1, Zeile 2
			lcd.print(relais[Encoder_count_neu]);

			digitalWrite(relais[Encoder_count_neu], EIN); // Relais einschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

			while (!digitalRead(ENTER_PIN))
			{
			}// Relais halten bis Enter Taste losgelassen

			digitalWrite(relais[Encoder_count_neu], AUS); // Relais ausschalten, Verkehrte Logik: LOW = EIN, HIGH = AUS

			on_off_encoder = true; // Encoder Interrupt einschalten
		} // if(!digitalRead(ENTER_PIN))

	} while (digitalRead(I_O_PIN)); // solange I/O Ttaste nicht gedrückt ist, also high ist

	//  ************************** Relais Test Ende **************************************

} // end void loop() **********************************************************************
