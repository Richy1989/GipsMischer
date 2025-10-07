#include "hauptprogramm.h"

///////////////////////////////////// Anfang Begrüßugng auf dem LCD  ///////////////////////////////////////////
//  Begrüßung auf dem LCD Display
//  und Entscheidung ob nicht offizielle Service Routine oder das Hauptprogramm gestartet wird

void greeting()
{
   // return;

    start_time = millis(); // Startzeit für Begrüßung setzen

    do
    {
        //  Wenn Taste ENTER und I/O Taste gleichzeitig gedrückt ist, also low
        //  wird die Service Routine aufgerufen
        //  (Enter Taste ist die linke Taste auf der Bedieneinheit)
        //  (I/O Taste ist die rechte Taste auf der Bedieneinheit)
        if (!digitalRead(ENTER_PIN) && !digitalRead(I_O_PIN))
        {
            service(); // Aufruf der Service Routine

            while (!digitalRead(I_O_PIN)) // warten bis I/O Taste losgelasen wenn vom Servoce zurück
            {
            } // end while (!digitalRead(I_O_PIN))

            start_time = millis(); // Startzeit für Begrüßung setzen
        } // end if (!digitalRead(ENTER_PIN) && !digitalRead(I_O_PIN))

        // Laufschrift zur Begrüßung:
        // *   Zahntechnik   * *     powered    *  *    Richard     *
        // *    Obwegeser   *  *       by       *  *    LEOPOLD     *

        if (millis() - start_time < WAIT_TIME_2) // Anzeige der ersten 2 Sekunden
        {
            lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
            lcd.print("   Zahntechnik   ");
            lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
            lcd.print("    Obwegeser   ");
        } // end if (delta_time < WAIT_TIME_2)
        else
        {
            if (millis() - start_time < WAIT_TIME_3) // Anzeige der zweiten 2 Sekunden
            {
                lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("    powered     ");
                lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("       by       ");
            } // end else...if (delta_time < WAIT_TIME_3)
            else if (millis() - start_time < WAIT_TIME_4) // Anzeige der dritten 2 Sekunden
            {
                lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
                lcd.print("    Richard     ");
                lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("    LEOPOLD     ");
            } // end else...if (delta_time < WAIT_TIME_4)
        } // end if (delta_time < WAIT_TIME_2)
    } while (millis() - start_time < WAIT_TIME_4);
} //  end greeting()
///////////////////////////////////// Ende Begrüßugng auf dem LCD  ///////////////////////////////////////////

///////////////////////////////////// Begin cursor_start  ///////////////////////////////////////////
//  Gibt die Position des übergebenen Charakters zurück,
//  damit das editieren mit dem bestehenden Charakter beginnen kann
int edit_cursor_start(char buchstabe)
{
    for (unsigned i = 0; i <= anzahl_texteingabe; i++)
        if (texteingabe[i] == buchstabe)
            return (i); // Position des Charakters im Array texteingabe zurückgeben

    return (0); // wenn der Character nicht gefunden wurde, Pointer auf erster Char der der Liste (A) zurückgeben
} //  end cursor_start()
///////////////////////////////////// Ende cursor_start  ///////////////////////////////////////////

///////////////////////////////////// Beginn check_armposition  ///////////////////////////////////////////
void check_armposition()
{
    //  Armpositionen einlesen und in Variable armposition speichern
    //  ARM_LINKS, ARM_MITE, ARM_RECHTS, ARM_NO_POS

    armposition = ARM_LINKS;
    armposition_alt = ARM_LINKS;
    return;

    read_armposition();

    if (armposition_alt != armposition && armposition == ARM_NO_POS)
    {
        // Fehleranzeige, keine Armposition erkannt wird
        lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 1
        lcd.print("NO ARM POSITION!");

        // FEHLERERKENNUNG aktivieren!!!!

        Musik(MELODIE_FEHLER);
        delay(3000);
    } // end if (armposition_alt != armposition)
    else
        armposition_alt = armposition;
} // end void check_armposition()
///////////////////////////////////// Ende check_armposition  ///////////////////////////////////////////

//////////////////////////////////////// Beginn LCD_fortschtitt ///////////////////////////////////////////
void LCD_fortschtitt(float max, float teil)
{
    int x = (int)(((teil / max) * 13) + 0.5); // Wert auf 13 Character aufteilen

    lcd.setCursor(3, 1); // Setz Curser auf Charakter 4, Zeile 2

    for (int i = 1; i < 14; i++) // 1 bis 14; also 13 Character für Fortschrittsanzeige
    {
        if (i <= x)
            lcd.write(byte(255)); // voll ausgefülltes Rechteck (zeigt Fortschritt an)
        else
            lcd.write(byte(RECHTECK)); // leeres Rechteck
    } // end for (int i = 1; i < 14; i++)
} // end LCD_fortschtitt(float prozent)
//////////////////////////////////////// Ende LCD_fortschtitt ///////////////////////////////////////////

//////////////////////////////////////// Beginn mainprogramm ///////////////////////////////////////////
void mainprogramm()
{
    int LCD_cursor_position;                      // Curserposition auf dem LCD (3 bis 16)
    bool write_to_LCD[MAX_CURSOR_POSITIONEN + 1]; // 0 bis 11, 12 Positionen, damit immer nur einmalig auf den LCD geschrieben wird

    greeting(); // Programmsart und Entscheidung: Serviceroutine oder Hauptprogramm

    on_off_encoder = true; // Encoder Interrupt einschalten

    lcd.clear(); // LCD löschen

    ////////////////////////////////// Beginn Arbeits Programm ///////////////////////////////////////////

    min_counter = 0;                     // Minimalwert für Encoder
    max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

    Encoder_count_neu = min_counter;  // Start mit Zeile 0 (Überschrift)
    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
    Encoder_count_store = OUT_OF_RANGE;

    on_off_encoder = true;          // Encoder Interrupt einschalten
    armposition_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

    do
    {
        check_armposition();

        /*
        LED_anzeigen();
        */

        if (Encoder_count_neu != Encoder_count_alt)
        {

            if (Encoder_count_store != Encoder_count_neu) // LCD muss neu beschrieben werden
            {
                for (int i = 0; i <= MAX_CURSOR_POSITIONEN; i++)
                {
                    write_to_LCD[i] = true; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end for (int i = 0; i <= MAX_CURSOR_POSITIONEN; i++)
            }

            Encoder_count_alt = Encoder_count_neu;
            Encoder_count_store = Encoder_count_neu;

            switch (Encoder_count_neu) //  Menüauswahl POS 01 bis POS 12
            {
            case 0: /*  POS 01 - Überschrift  */
                if (write_to_LCD[Encoder_count_neu])
                {
                    lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
                    lcd.print("01");
                    lcd.write(byte(CURSOR));                    //  Zeichen "cursor" anzeigen
                    lcd.print(daten[armposition].ueberschrift); // komplette Überschrift anzeigen

                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("02     g      ml");

                    lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print(daten[armposition].gewicht[0]); //  Referenzmenge Gipsgewicht in g

                    lcd.setCursor(10, 1);                     // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print(daten[armposition].gewicht[1]); // Referenzmenge Waser in g

                    write_to_LCD[Encoder_count_neu] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end if (erstmalig)

                // wenn ENTER Taste gedrückt, beginnt editieren des Strigs Überschrift
                if (!digitalRead(ENTER_PIN))
                {
                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(ENTER_PIN))
                    { // warten bis Taste ENTER losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    min_counter = EDIT_CHAR_CURSOR_SART; // Minimalwert für Encoder 0
                    max_counter = anzahl_texteingabe;    // Maximalwert für Encoder (0 bis 39, also 40 Positionen)

                    LCD_cursor_position = EDIT_LCD_CURSOR_SART; // Startposition des Cursors am LCD für die Charactereingabe

                    // Pointer auf den Character im Character Array, welcher zur Anzeige gebracht wird
                    Encoder_count_neu = edit_cursor_start(daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET]);
                    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                    do
                    {
                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(LCD_cursor_position, 0);     // Setz Curser auf Charakter 3, Zeile 1
                            lcd.print(texteingabe[Encoder_count_neu]); // Character des Character Array auf den LCD schreiben
                            lcd.setCursor(LCD_cursor_position, 0);     // Setz Curser auf Charakter 3, Zeile 1
                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)

                        // wenn ENTER Taste gedrückt ein Charakter nach Rechts zum editieren
                        if (!digitalRead(ENTER_PIN))
                        {
                            delay(4 * ENTPRELL_ZEIT); // Entprellzeit
                            while (!digitalRead(ENTER_PIN))
                            { // warten bis Taste ENTER losgelasen wird
                            } //  end while (!digitalRead(ENTER_PIN))
                            delay(4 * ENTPRELL_ZEIT); // Entprellzeit

                            lcd.noBlink(); // Cursor blinken ausschalten

                            //  abspeichern des Character in der Überschrift
                            daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET] = texteingabe[Encoder_count_neu]; // Character in das Überschrift Array schreiben);

                            if (LCD_cursor_position++ <= EDIT_LCD_CURSOR_MAX)
                            { // nur wenn der Cursor noch nicht am Ende der Überschrift ist
                                Encoder_count_neu = edit_cursor_start(daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET]);

                            } // end if (LCD_cursor_position <= EDIT_LCD_CURSOR_MAX)

                        } // end if (!digitalRead(ENTER_PIN))
                    } while (LCD_cursor_position <= EDIT_LCD_CURSOR_MAX);

                    // Überschrift wurde editiert, daher Datenspeicherung im EEPROM:
                    EEPROM.put(0, daten); // Schreiben der komletten Variablen (Datenstruktur) Daten auf den EEPROM, ab Adresse 0

                    min_counter = 0;                     // Minimalwert für Encoder
                    max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                    Encoder_count_neu = 0; // Verbleib im Manü case 0, (POS1 Überschrift)
                } // end if (!digitalRead(ENTER_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
                break;                            //  end case 0 (POS 01 - Überschrift)

            case 1: /*  POS 02 - Referenz Gipsgewicht und Wassergewicht  */
                if (write_to_LCD[Encoder_count_neu])
                {
                    lcd.setCursor(2, 0); // Setz Curser auf Charakter 3, Zeile 1
                    lcd.write(" ");      //  Löschen des alten Cursors in Zeile 1

                    //  Zeichen "cursor" anzeigen
                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("02     g      ml");
                    //  lcd.print("02>1234g  1234ml");  erste Zahl ab 3. Stelle, zweite Zahl ab 10. Stelle

                    lcd.setCursor(2, 1);     // Setz Curser auf Charakter 3, Zeile 2
                    lcd.write(byte(CURSOR)); //  Zeichen "cursor" anzeigen

                    lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print(daten[armposition].gewicht[0]); //  Referenzmenge Gipsgewicht in g

                    lcd.setCursor(10, 1);                     // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print(daten[armposition].gewicht[1]); // Referenzmenge Waser in g

                    write_to_LCD[Encoder_count_neu] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end if (erstmalig)

                if (!digitalRead(ENTER_PIN))
                {
                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(ENTER_PIN))
                    { // warten bis Taste ENTER losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    min_counter = MIN_GEWICHT_EINGABE; // Minimalwert für Encoder 1 (Division durch 0 vermeiden)
                    max_counter = MAX_GEWICHT_EINGABE; // Maximalwert für Encoder (Gewicht 2000 g)

                    //  Bestehendes Gipsgewicht in den Editor schreiben
                    Encoder_count_neu = daten[armposition].gewicht[0];
                    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                    do
                    { // editiereb der Referenzmenge Gipsgewicht  -------------------------------
                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print("    ");            //  Platzhalter zum Löschen der alten Zahl
                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print(Encoder_count_neu); //  Referenzmenge Gipsgewicht in g

                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)
                    } while (digitalRead(ENTER_PIN)); // Ende der Eingabe mit ENTER Taste

                    lcd.noBlink(); // Cursor blinken ausschalten

                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(ENTER_PIN))
                    { // warten bis Taste ENTER losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    //  Referenzmenge  Gipsgewicht in die Datenstruktur schreiben
                    daten[armposition].gewicht[0] = Encoder_count_neu;

                    //  Bestehende Wassermenge in den Editor schreiben
                    Encoder_count_neu = daten[armposition].gewicht[1];

                    do
                    { // editiereb der Referenzmenge Wassermenge -------------------------------
                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(10, 1);         // Setz Curser auf Charakter 1, Zeile 2
                            lcd.print("    ");            //  öschen der alten Zahl
                            lcd.setCursor(10, 1);         // Setz Curser auf Charakter 1, Zeile 2
                            lcd.print(Encoder_count_neu); //  Referenzmenge Wasser in g
                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)
                    } while (digitalRead(ENTER_PIN)); // Ende der Eingabe mit ENTER Taste

                    lcd.noBlink(); // Cursor blinken ausschalten

                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(ENTER_PIN))
                    { // warten bis Taste ENTER losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    //  Referenzmenge  Wassermenge in die Datenstruktur schreiben
                    daten[armposition].gewicht[1] = Encoder_count_neu;

                    // Referenzmenge Gipsgewicht und Wassermenge wurden editiert, daher Datenspeicherung im EEPROM:
                    EEPROM.put(0, daten);
                    // Schreiben der komletten Variablen (Datenstruktur) Daten auf den EEPROM, ab Adresse 0

                    // Neue Gewichtverhältnisse berechnen, // Gips Referenzgewicht  /  Wasser Referenzgewicht
                    gips_zu_h2o_verhaeltnis[armposition] = daten[armposition].gewicht[0] / daten[armposition].gewicht[0];

                    min_counter = 0;                     // Minimalwert für Encoder
                    max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                    Encoder_count_neu = 1; // Verbleib im Manü case 1, (POS 02 - Referenz Gipsgewicht und Wassergewicht )
                } // end if (!digitalRead(ENTER_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
                break;                            //  end case 1 (POS 02 - Referenz Gipsgewicht und Wassergewicht )

            case 2: /*  POS 03 - Gipsgewicht fix immer um 160/130 g erhöht ==> 160 g / 130g  */
            case 3: /*  POS 04 - Gipsgewicht fix immer um 160/130 g erhöht ==> 320 g / 260g  */
            case 4: /*  POS 05 - Gipsgewicht fix immer um 160/130 g erhöht ==> 480 g / 390g  */
            case 5: /*  POS 06 - Gipsgewicht fix immer um 160/130 g erhöht ==> 640 g / 520g  */
                if (write_to_LCD[Encoder_count_neu])
                {
                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("0      g fixiert");
                    lcd.setCursor(1, 1);              // Setz Curser auf Charakter 2, Zeile 2
                    lcd.print(Encoder_count_neu + 1); //  Positionsanzeige
                    lcd.write(byte(CURSOR));
                    lcd.print(daten[armposition].gewicht[Encoder_count_neu]); //  Referenzmenge Gipsgewicht in g

                    write_to_LCD[Encoder_count_neu] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } //  end if (erstmalig)

                // Gips Abfüllung beginnt ---------------------------------------

                if (!digitalRead(I_O_PIN))
                {
                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(I_O_PIN))
                    { // warten bis Taste I/O  losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    start_time = millis();

                    while (scale.is_ready()) // scale.is_ready --> Waage ist bereit wenn true
                        if (millis() - start_time > WAAGE_READY_TIME)
                            break; // while () ... Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt

                    if (millis() - start_time > WAAGE_READY_TIME) // WAAGE_READY_TIME)
                    {                                             //  Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt
                        lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 4, Zeile 2
                        lcd.print("Waage defekt!");
                        Musik(MELODIE_FEHLER);
                        Encoder_count_store = OUT_OF_RANGE; // LCD Grundanzeige wieder herstellen
                        //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                        Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                        break; //  case 2 - 5 Abbruch, (POS 03 - 06 - Gipsgewicht fix immer um 160/130 g erhöht)
                    } // end if (millis() - start_time < WAAGE_READY_TIME)

                    teilgewicht_gips = daten[armposition].gewicht[Encoder_count_neu];          // Gipsgewicht
                    teilgewicht_h2o = teilgewicht_gips / gips_zu_h2o_verhaeltnis[armposition]; // Gipsgewicht

                    gesamtgewicht =      // Gesamtgewicht =
                        teilgewicht_gips // Gipsgewicht
                        +                // +
                        teilgewicht_h2o  // Wassergewicht
                        ;

                    digitalWrite(relais[armposition], EIN);     // Relais Gipsmotor [ARM Position] einschalten
                    digitalWrite(relais[armposition + 3], EIN); // Relais Rüttler [ARM Position] einschalten

                    do // Gips Abfüllung
                    {
                        // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                        // Dividiert Korrekturfaktor
                        Gewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;

                        LCD_fortschtitt(gesamtgewicht, Gewicht); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                    } while (Gewicht < teilgewicht_gips); // Auf Gipsgewicht prüfen

                    digitalWrite(relais[armposition], AUS);     // Relais Gipsmotor [ARM Position] ausschalten
                    digitalWrite(relais[armposition + 3], AUS); // Relais Rüttler [ARM Position] ausschalten

                    //  Wasser Abfüllung beginnt ---------------------------------------

                    digitalWrite(relais[armposition + 6], EIN); // Relais Ventil [ARM Position] einschalten
                    digitalWrite(RELAIS_WP, EIN);               // Relais Wasserpumpe einschalten

                    do // H2O Abfüllung
                    {
                        // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                        // Dividiert Korrekturfaktor
                        Gewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;

                        Serial.println("Gewicht IST "); ///////////////////////////
                        Serial.println(Gewicht);        ///////////////////////////

                        LCD_fortschtitt(gesamtgewicht, Gewicht); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                    } while (Gewicht < gesamtgewicht); // Auf Gipsgewicht + Wassergewicht prüfen

                    digitalWrite(RELAIS_WP, AUS);               // Relais Wasserpumpe ausschalten
                    digitalWrite(relais[armposition + 6], AUS); // Relais Ventil [ARM Position] ausschalten

                    Musik(MELODIE_OK);
                    //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                    Encoder_count_store = OUT_OF_RANGE; // LCD muss wieder beschrieben werden --> Fortschrittsanzeige
                } // end    if (!digitalRead(I_O_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                break; //  end case 2 - 5, (POS 03 - 06 - Gipsgewicht fix immer um 160/130 g erhöht)

            case 6: /*  POS 07 - Gipsgewicht in g eingeben default ist 1  */
            case 7: /*  POS 08 - Gipsgewicht in g eingeben default ist 1  */
            case 8: /*  POS 09 - Gipsgewicht in g eingeben default ist 1  */
            case 9: /*  POS 10 - Gipsgewicht in g eingeben default ist 1  */
                if (write_to_LCD[Encoder_count_neu])
                {
                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("0       g variab");

                    if (Encoder_count_neu == 9) /*POS 10, zweistellig auf LCD schreiben*/
                    {
                        lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                        lcd.print("10");
                    } // end if (Encoder_count_neu == 9)
                    else
                    {
                        lcd.setCursor(1, 1);              // Setz Curser auf Charakter 2, Zeile 2
                        lcd.print(Encoder_count_neu + 1); //  Positionsanzeige
                    } // end else ...if (Encoder_count_neu == 9)

                    lcd.write(byte(CURSOR));
                    lcd.print(daten[armposition].gewicht[Encoder_count_neu]); //  Referenzmenge Gipsgewicht in g

                    write_to_LCD[Encoder_count_neu] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end if (erstmalig)

                // Gipsmenge editieren beginnt ---------------------------------------
                // "Encoder_count_neu" muss im "Encoder_count_store" zwischengespeichert werden
                if (!digitalRead(ENTER_PIN))
                {
                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(ENTER_PIN))
                    { // warten bis Taste ENTER losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    Encoder_count_store = Encoder_count_neu; // Zwischenspeicherung der Position

                    //  Bestehendes Gipsgewicht in den Editor schreiben
                    Encoder_count_neu = daten[armposition].gewicht[Encoder_count_neu];
                    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                    min_counter = MIN_GEWICHT_EINGABE; // Minimalwert für Encoder 1 (Division durch 0 vermeiden)
                    max_counter = MAX_GEWICHT_EINGABE; // Maximalwert für Encoder (Gewicht 2000 g)

                    do
                    { // editieren der Referenzmenge Gipsgewicht  -------------------------------
                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print("    ");            //  Platzhalter zum Löschen der alten Zahl
                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print(Encoder_count_neu); //  Referenzmenge Gipsgewicht in g

                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)
                    } while (digitalRead(ENTER_PIN)); // Ende der Eingabe mit ENTER Taste

                    lcd.noBlink(); // Cursor blinken ausschalten

                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(ENTER_PIN))
                    { // warten bis Taste ENTER losgelasen wird
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    // Gipsgewicht  wurden editiert, daher Datenspeicherung im EEPROM:
                    daten[armposition].gewicht[Encoder_count_store] = Encoder_count_neu;

                    EEPROM.put(0, daten);
                    // Schreiben der komletten Variablen (Datenstruktur) Daten auf den EEPROM, ab Adresse 0

                    min_counter = 0;                     // Minimalwert für Encoder
                    max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                    //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 6, 7, 8, 9
                    Encoder_count_neu = Encoder_count_store; // Rückkehr zur ursprünglichen Position
                } // end if (!digitalRead(ENTER_PIN))   //   ENDE des editieren der Referenzmenge Gipsgewicht

                // Gips- / Wasser-Abfüllung beginnt ---------------------------------------
                if (!digitalRead(I_O_PIN))
                {
                    delay(ENTPRELL_ZEIT); // Entprellzeit
                    while (!digitalRead(I_O_PIN))
                    { // warten bis Taste I/O  losgelasen wird
                    } //  end while (digitalRead(I_O_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    start_time = millis();

                    while (scale.is_ready()) // scale.is_ready --> Waage ist bereit wenn true
                        if (millis() - start_time > WAAGE_READY_TIME)
                            break; // while () ... Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt

                    if (millis() - start_time > WAAGE_READY_TIME) // WAAGE_READY_TIME)
                    {                                             //  Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt
                        lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 4, Zeile 2
                        lcd.print("Waage defekt!");
                        Musik(MELODIE_FEHLER);
                        Encoder_count_store = OUT_OF_RANGE; // LCD Grundanzeige wieder herstellen
                        //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                        Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                        break; //  case 2 - 5 Abbruch, (POS 03 - 06 - Gipsgewicht fix immer um 160/130 g erhöht)
                    } // end if (millis() - start_time < WAAGE_READY_TIME)

                    eigengewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor; // gemessene Grundgewicht
                    teilgewicht_gips = daten[armposition].gewicht[Encoder_count_neu];             // eingegebenes Gipsgewicht

                    teilgewicht_h2o = teilgewicht_gips / gips_zu_h2o_verhaeltnis[armposition]; // berechnetes Wassergewicht

                    gesamtgewicht =  // Gesamtgewicht =
                        eigengewicht // Eigengewicht der Waagschale
                        +
                        teilgewicht_gips // Gipsgewicht
                        +                // +
                        teilgewicht_h2o  // Wassergewicht
                        ;

                    digitalWrite(relais[armposition], EIN);     // Relais Gipsmotor [ARM Position] einschalten
                    digitalWrite(relais[armposition + 3], EIN); // Relais Rüttler [ARM Position] einschalten

                    Serial.print("Leergewichteinheiten "); ///////////////////////////
                    Serial.println(Leergew_einheiten);     ///////////////////////////
                    Serial.print("Korrekturfaktor ");      ///////////////////////////
                    Serial.println(Korrekturfaktor);       ///////////////////////////
                    Serial.print("Eigengewicht ");         ///////////////////////////
                    Serial.println(eigengewicht);          ///////////////////////////
                    Serial.print("Gesamtgewicht ");        ///////////////////////////
                    Serial.println(gesamtgewicht);         ///////////////////////////
                    Serial.print("Teilgewicht GIPS ");     ///////////////////////////
                    Serial.println(teilgewicht_gips);      ///////////////////////////
                    Serial.print("Teilgewicht H2O ");      ///////////////////////////
                    Serial.println(teilgewicht_h2o);       ///////////////////////////

                    // warten bis Taste Enter gedrückt wird, also low wird, dann weiter
                    while (digitalRead(ENTER_PIN))
                        delay(100 * ENTPRELL_ZEIT); // Entprellzeit//  end while (digitalRead(ENTER_PIN))

                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    do // Gips Abfüllung
                    {
                        // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                        // Dividiert Korrekturfaktor
                        // Abzüglich des Eigengewicht der Waagscghale
                        Gewicht = ((scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor) -
                                  eigengewicht; // Abzüglich des Eigengewichts der Waagschale

                        Serial.println("Gewicht IST "); ///////////////////////////
                        Serial.println(Gewicht);        ///////////////////////////

                        LCD_fortschtitt(gesamtgewicht - eigengewicht, Gewicht); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                    } while (Gewicht - eigengewicht < teilgewicht_gips); // Auf Gipsgewicht prüfen

                    digitalWrite(relais[armposition], AUS);     // Relais Gipsmotor [ARM Position] ausschalten
                    digitalWrite(relais[armposition + 3], AUS); // Relais Rüttler [ARM Position] ausschalten

                    //  Wasser Abfüllung beginnt ---------------------------------------

                    digitalWrite(relais[armposition + 6], EIN); // Relais Ventil [ARM Position] einschalten
                    digitalWrite(RELAIS_WP, EIN);               // Relais Wasserpumpe einschalten

                    Serial.print("Gesamtgewicht ");    ///////////////////////////
                    Serial.println(gesamtgewicht);     ///////////////////////////
                    Serial.print("Teilgewicht GIPS "); ///////////////////////////
                    Serial.println(teilgewicht_gips);  ///////////////////////////
                    Serial.print("Teilgewicht H2O ");  ///////////////////////////
                    Serial.println(teilgewicht_h2o);   ///////////////////////////

                    // warten bis Taste Enter gedrückt wird, also low wird, dann weiter
                    while (digitalRead(ENTER_PIN))
                    {
                        delay(ENTPRELL_ZEIT); // Entprellzeit
                    } //  end while (digitalRead(ENTER_PIN))
                    delay(ENTPRELL_ZEIT); // Entprellzeit

                    do // H2O Abfüllung
                    {
                        // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                        // Dividiert Korrekturfaktor
                        // Abzüglich des Eigengewicht der Waagscghale
                        Gewicht = ((scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor); // gemessenes Gewicht inklusive der Waagscale

                        Serial.println("Gewicht IST "); ///////////////////////////
                        Serial.println(Gewicht);        ///////////////////////////

                        LCD_fortschtitt(gesamtgewicht - eigengewicht, Gewicht - eigengewicht); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                    } while (Gewicht < gesamtgewicht); // Auf Gipsgewicht + Wassergewicht prüfen

                    digitalWrite(RELAIS_WP, AUS);               // Relais Wasserpumpe ausschalten
                    digitalWrite(relais[armposition + 6], AUS); // Relais Ventil [ARM Position] ausschalten

                    Musik(MELODIE_OK);
                    //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 6, 7, 8, 9
                    Encoder_count_store = OUT_OF_RANGE; // LCD muss wieder beschrieben werden --> Fortschrittsanzeige
                } // end    if (!digitalRead(I_O_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                break; //  end case 6 - 9, (POS 07 - 10 - Gipsgewicht in g eingeben default ist 1)

            case 10: /*  POS 11 - Gipsentleerung  */
                if (write_to_LCD[10])
                {
                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("11 GIPS Entleer.");
                    lcd.setCursor(2, 1); // Setz Curser auf Charakter 3, Zeile 2
                    lcd.write(byte(CURSOR));

                    write_to_LCD[10] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end if (erstmalig)

                // Gipsentleerrn beginnt ---------------------------------------
                if (!digitalRead(I_O_PIN))
                {
                    on_off_encoder = false; // Encoder Interrupt ausschalten

                    digitalWrite(relais[armposition], EIN); // Relais Gipsmotor[ARM Position] einschalten

                    while (!digitalRead(I_O_PIN)) // warten bis I/O taste losgelasen
                    {
                        delay(ENTPRELL_ZEIT);
                    } // Relais halten bis I/O Taste losgelassen

                    digitalWrite(relais[armposition], AUS); // Relais Gipsmotor [ARM Position] ausschalten

                    on_off_encoder = true;  // Encoder Interrupt einschalten
                    Encoder_count_neu = 10; //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 10
                } // end  if (!digitalRead(I_O_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
                break;                            //  end case 10, (POS 11 - Gipsentleerung)

            case 11: /*  POS 12 - Wasserentnahme  */
                if (write_to_LCD[11])
                {
                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("12 H2O  Entnahme");
                    lcd.setCursor(2, 1); // Setz Curser auf Charakter 3, Zeile 2
                    lcd.write(byte(CURSOR));

                    lcd.setCursor(2, 0); // Setz Curser auf Charakter 3, Zeile 1
                    lcd.print(" ");      // Löschen des Curseors in Zeile 1

                    write_to_LCD[11] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end if (erstmalig)

                // Wasserentnahme beginnt ---------------------------------------
                if (!digitalRead(I_O_PIN))
                {
                    on_off_encoder = false; // Encoder Interrupt ausschalten

                    digitalWrite(RELAIS_WP, EIN); // Relais Wasserpumpe einschalten

                    while (!digitalRead(I_O_PIN)) // warten bis I/O Taste  losgelasen
                    {
                        delay(ENTPRELL_ZEIT);
                    } // Relais halten bis I/O Taste losgelassen

                    digitalWrite(RELAIS_WP, AUS); // Relais Wasserpumpe ausschalten

                    on_off_encoder = true;  // Encoder Interrupt einschalten
                    Encoder_count_neu = 11; //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 11
                } // end  if (!digitalRead(I_O_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                break; //  end case 11, (POS 12 - Wasserentnahme)

            default:
                lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("Error");  // Fehlerausgabe
                Musik(MELODIE_FEHLER);
                break;
            } // end switch (curser_position)
        } // end if (Encoder_count_neu != Encoder_count_alt
    } while (true);
} //  end mainprogramm()
////////////////////////////////// Ende mainprogramm  ///////////////////////////////////////////
