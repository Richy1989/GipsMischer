#include "hauptprogramm.h"

///////////////////////////////////// Begin press_key  ///////////////////////////////////////////
// wartet bis eine Taste gedrückt wird (Prellzeit wird berücksichtigt)
boolean press_key(unsigned int taste, unsigned int scan)
{
    switch (scan)
    {
    case SCAN:                   // scaned die Taste einmalig (low ist gedrückt, high ist losgelassen)
        if (!digitalRead(taste)) // ist die Taste low, gedrückt
        {
            delay(ENTPRELL_ZEIT); // Entprellzeit
            if (!digitalRead(taste))
                return (true); // Taste ist low, gedrückt
        } // end if (digitalRead(taste))

        return (false); // Taste ist high, losgelassen
        break;          // end  case SCAN

    case WAIT:                     // scaned die Taste bis die Taste gedrückt wird (low ist gedrückt, high ist losgelassen)
        while (digitalRead(taste)) // so lange die Taste high, losgelassen ist
        {                          // warten bis "Taste" low, gedrückt wird
        }
        delay(ENTPRELL_ZEIT); // Entprellzeit
        return (true);        // Taste ist gedrückt
        break;                // end  case WAIT

    case TASTE_KOMPLETT:              // scaned die Taste bis die Taste wieder loshelassen wird (low ist gedrückt, high ist losgelassen)
        if (release_key(taste, WAIT)) ///  Warten bis die Taste losgelasen wird
            return (true);            // Taste ist losgelassen (ein kompletter Tastebdruck ist absolviert, high-->low-->high)

        return (false); // Taste ist high, losgelassen KEIN kompletter Tastebdruck
        break;

    default:
        return (false); // falsche übergabe in scan enthalten
        break;
    } // end switch (scan)
} //  end press_key()
///////////////////////////////////// Ende press_key  ///////////////////////////////////////////

///////////////////////////////////// Begin release_key  ///////////////////////////////////////////
// wartet bis eine Taste losgelassen wird  (Prellzeit wird berücksichtigt)
boolean release_key(unsigned int taste, unsigned int scan)
{
    if (scan == SCAN)
    {
        if (digitalRead(taste)) // ist die Taste high, losgelassen
        {
            delay(ENTPRELL_ZEIT);   // Entprellzeit
            if (digitalRead(taste)) // ist die Taste losgelassen
                return (true);      // Taste ist losgelassen
        } // end if (digitalRead(taste))
        else                // taste ist low, gedrückt
            return (false); // Taste ist gedrückt
    } // end if (scan == SCAN)

    while (!digitalRead(taste)) // so lange die Taste low, gedrückt ist
    {                           // warten bis "Taste" high, losgelasen wird
    }
    delay(ENTPRELL_ZEIT); // Entprellzeit
    return (true);        // Taste ist high, losgelassen
} //  end release_key()
///////////////////////////////////// Ende release_key  ///////////////////////////////////////////

///////////////////////////////////// Anfang Begrüßugng auf dem LCD  ///////////////////////////////////////////
//  Begrüßung auf dem LCD Display
//  und Entscheidung ob nicht offizielle Service Routine oder das Hauptprogramm gestartet wird

void greeting()
{
    start_time = millis(); // Startzeit für Begrüßung setzen

    do
    {
        // press_key(ENTER_PIN, SCAN);
        //  Wenn Taste ENTER und I/O Taste gleichzeitig gedrückt ist, also low
        //  wird die Service Routine aufgerufen
        //  (Enter Taste ist die linke Taste auf der Bedieneinheit)
        //  (I/O Taste ist die rechte Taste auf der Bedieneinheit)
        if (press_key(ENTER_PIN, SCAN) && press_key(I_O_PIN, SCAN))
        {
            service(); // Aufruf der Service Routine

            release_key(I_O_PIN, WAIT); // Da Serviceroutine mit I/O abgebrochen wird

            start_time = millis(); // Startzeit für Begrüßung setzen
        } // end if (!digitalRead(ENTER_PIN) && !digitalRead(I_O_PIN))

        if (press_key(WAAGE_PIN, SCAN))
        {
            release_key(WAAGE_PIN, WAIT);
            //    Musik(MELODIE_SMOKE_ON_THE_WATER);
            Musik(MELODIE_ANFANG);
            break; // Begrüßung beenden und ins Hauptprogramm wechseln
        } // end if (!digitalRead(WAAGE_PIN))

        // Laufschrift zur Begrüßung:
        // *   Zahntechnik   * *   powered by   *  *    Richard     *
        // *    Obwegeser   *  *V-1.00 :Nov 2025*  *    LEOPOLD     *

        if (millis() - start_time < WAIT_TIME_2) // Anzeige der ersten 3 Sekunden
        {
            lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
            lcd.print("   Zahntechnik   ");
            lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
            lcd.print("    Obwegeser   ");
        } // end if (delta_time < WAIT_TIME_2)
        else
        {
            if (millis() - start_time < WAIT_TIME_3) // Anzeige der zweiten 3 Sekunden
            {
                lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("   powered by   ");
                lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("V-1.00 :Nov 2025");
            } // end else...if (delta_time < WAIT_TIME_3)
            else if (millis() - start_time < WAIT_TIME_4) // Anzeige der dritten 3 Sekunden
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
    int LED_pointer;                              // LED Adressierung im no Arm Positions Alarmfall

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

    // nur zum Test Armposition vorgeben ------------------------------
    armposition = ARM_LINKS;
    armposition_alt = ARM_LINKS;

    do
    {
        // Begin WAAGE FUNKTION     Taste Waage wurde gedrückt -----------------------------------
        // Armposition spielt in dieser Funktion keine Rolle
        if (press_key(WAAGE_PIN, SCAN)) //  Waagefunktion
        {
            release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

            lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
            lcd.print(" Waagefunktion  ");
            lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 1
            lcd.print("Gewicht:       g");

            start_time = millis();

            while (scale.is_ready()) // scale.is_ready --> Waage ist bereit wenn true
                if (millis() - start_time > WAAGE_READY_TIME)
                    break; // while () ... Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt

            if (millis() - start_time > WAAGE_READY_TIME) // WAAGE_READY_TIME)
            {                                             //  Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt
                lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 4, Zeile 2
                lcd.print("Waage defekt!");

                Musik(MELODIE_FEHLER);
                delay(LCD_TIME); // Anzeige der Fehlermeldung

                Encoder_count_store = OUT_OF_RANGE; // LCD Grundanzeige wieder herstellen
                //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                break; //  Abbruch der Waagefunktion, zurück ins Hauptmenü
            } // end if (millis() - start_time < WAAGE_READY_TIME)

            Gewicht_alt = OUT_OF_RANGE;                                                         // Gewicht_alt auf 0 setzen, damit Gewicht angezeigt wird
            gewicht_waagschale = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor; // gemessene Grundgewicht

            do
            {
                // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                // Dividiert Korrekturfaktor
                Gewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;

                if (Gewicht != Gewicht_alt) // Anzeigen des Gewichtes nur wenn es sich geändert hat
                {
                    Gewicht_alt = Gewicht; // Gewicht_alt aktualisieren
                    lcd.setCursor(9, 1);   // Setze Cursor auf die 8. Stelle der 2. Zeile
                    lcd.print("     ");    // Clear previous value
                    lcd.setCursor(9, 1);

                    int mom_Gewicht = (int)(Gewicht - gewicht_waagschale);

                    if (Gewicht > MAX_GEWICHT) // Maximalgewicht (inklusive gewicht_waagschale) überschritten
                    {
                        lcd.setCursor(0, 1);           // Setz Curser auf Charakter 1, Zeile 2
                        lcd.print("Gewicht zu gross"); // allgemeiner Fehler

                        Musik(MELODIE_FEHLER);
                        delay(LCD_TIME);                  // Anzeige der Fehlermeldung
                        Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
                    } //  end  if (Gewicht > MAX_GEWICHT)
                    else
                    {
                        if (mom_Gewicht == -1)
                            mom_Gewicht = 0; // Tolleranzbereich -1g bis 0g, damit Aneige um den 0 Punkt stabil bleibt

                        lcd.print(mom_Gewicht); // Gewicht anzeigen
                    } //  end  if (Gewicht > MAX_GEWICHT)

                } // end if (Gewicht != Gewicht_alt)

            } while (!press_key(WAAGE_PIN, SCAN)); // warten bis Taste Waage wieder gedrückt wird

            release_key(WAAGE_PIN, WAIT); //  Warten bis WAAGE Taste losgelasen wird

            // einstieg in Hauptschleife herstellen
            min_counter = 0;                     // Minimalwert für Encoder
            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

            Encoder_count_neu = min_counter;  // Start mit Zeile 0 (Überschrift)
            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
            Encoder_count_store = OUT_OF_RANGE;

            on_off_encoder = true; // Encoder Interrupt einschalten
                                   //   armposition_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

        } // end if (press_key(WAAGE_PIN, SCAN))
        // Ende WAAGE FUNKTION     Taste Waage wurde wiederum gedrückt -----------------------------------

        // ANFANG Armposition ermitteln -----------------------------------------
        read_armposition();

        if (armposition_alt != armposition)
        {
            // alle Relais ausschalten : Motoren, Rüttler, Ventile und Wasserpumpe
            for (unsigned int i = 0; i < anzahlrelais; i++) // Pointer von 0 bis 9, also 10 Relais
            {
                digitalWrite(relais[i], AUS); // Relais ausschalten (negative Logik: Ausgang ist also high, wenn Relais AUS ist)
            }

            //  alle LED ausschalten
            digitalWrite(LL, AUS);
            digitalWrite(LM, AUS);
            digitalWrite(LR, AUS);

            if (armposition == ARM_NO_POS)
            {
                LED_pointer = 0;              // LED Adressierung im no Arm Positions Alarmfall
                start_time_LED = millis();    // Startzeit für LED Blinkzeit setzen
                start_time_armpos = millis(); // Startzeit für Armpositions Alarm setzen

                // Fehleranzeige, keine Armposition erkannt wird
                lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 1
                lcd.print("NO ARM POSITION!");

                do // warten bis eine gültige Armposition erkannt wird
                {
                    if (start_time_LED + WAIT_TIME_LED_II < millis())
                    {                              //  LED Blinkanzeige (Lauflicht) für keine erkannte Armposition
                        start_time_LED = millis(); //  neu Startzeit für LED Blinkzeit setzen

                        digitalWrite(LL, AUS);
                        digitalWrite(LM, AUS);
                        digitalWrite(LR, AUS);

                        switch (LED_pointer)
                        {
                        case 0:
                            digitalWrite(LL, EIN); // LED links einschalten
                            LED_pointer = 1;
                            break;
                        case 1:
                            digitalWrite(LM, EIN); // LED mitte einschalten
                            LED_pointer = 2;
                            break;
                        case 2:
                            digitalWrite(LR, EIN); // LED rechts einschalten
                            LED_pointer = 0;
                            break;
                        default:
                            break;
                        } // end switch (LED_pointer)
                    } // end if (start_time_LED + WAIT_TIME_LED_II < millis())

                    if (start_time_armpos + WAIT_TIME_ARM_ALARM < millis())
                    {                                 //  akustischer Alarm für keine erkannte Armposition
                        start_time_armpos = millis(); //  neu Startzeit für LED Blinkzeit setzen
                        Musik(MELODIE_FEHLER);
                    } // end if (start_time_armpos + WAIT_TIME_ARM_ALARM < millis())

                    read_armposition();

                } while (armposition == ARM_NO_POS);

            } // end if (armposition == ARM_NO_POS)
            else
            { //  gültige Armposition ist hergestellt
                // richtige LED zur Armposition einschalten  ------------------------------
                digitalWrite(LL, AUS);
                digitalWrite(LM, AUS);
                digitalWrite(LR, AUS);

                switch (armposition)
                {
                case ARM_LINKS:
                    digitalWrite(LL, EIN); // LED links einschalten
                    break;
                case ARM_MITTE:
                    digitalWrite(LM, EIN); // LED mitte einschalten
                    break;
                case ARM_RECHTS:
                    digitalWrite(LR, EIN); // LED rechts einschalten
                    break;
                default:
                    break;
                } // end switch (armposition)
            } // else end if (armposition != ARM_NO_POS)

            // einstieg in Hauptschleife herstellen ------------------------------
            min_counter = 0;                     // Minimalwert für Encoder
            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

            Encoder_count_neu = min_counter;  // Start mit Zeile 0 (Überschrift)
            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
            Encoder_count_store = OUT_OF_RANGE;

            on_off_encoder = true;          // Encoder Interrupt einschalten
            armposition_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
        } // end if (armposition_alt != armposition)

        // ENDE Armposition ermitteln ------------------------------

        read_becher(); // Becherpositionen einlesen und in Array becher speichern

        // Hauptmenü ------------------------------------------------
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
                if (press_key(ENTER_PIN, SCAN)) //  wenn ENTER Taste gedrückt wird
                {
                    release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

                    min_counter = EDIT_CHAR_CURSOR_SART; // Minimalwert für Encoder 0
                    max_counter = anzahl_texteingabe;    // Maximalwert für Encoder (0 bis 39, also 40 Positionen)

                    LCD_cursor_position = EDIT_LCD_CURSOR_SART; // Startposition des Cursors am LCD für die Charactereingabe

                    // Pointer auf den Character im Character Array, welcher zur Anzeige gebracht wird
                    Encoder_count_neu = edit_cursor_start(daten[armposition].ueberschrift[LCD_cursor_position - LCD_CHARACTER_OFFSET]);
                    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                    do
                    {
                        read_armposition();
                        if (armposition != armposition_alt)
                        {
                            min_counter = 0;                     // Minimalwert für Encoder
                            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                            Encoder_count_neu = 0;            // Verbleib im Manü case 0, (POS1 Überschrift)
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; // Abbruch wenn Armposition sich geändert hat
                        }

                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(LCD_cursor_position, 0);     // Setz Curser auf Charakter 3, Zeile 1
                            lcd.print(texteingabe[Encoder_count_neu]); // Character des Character Array auf den LCD schreiben
                            lcd.setCursor(LCD_cursor_position, 0);     // Setz Curser auf Charakter 3, Zeile 1
                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)

                        // wenn ENTER Taste gedrückt ein Charakter nach Rechts zum editieren
                        if (press_key(ENTER_PIN, SCAN)) //  wenn ENTER Taste gedrückt wird
                        {
                            release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

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
                    lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
                    lcd.print("01");     // POS 01 anzeige löschen

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

                if (press_key(ENTER_PIN, SCAN)) //  wenn ENTER Taste gedrückt wird
                {
                    release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

                    min_counter = MIN_GEWICHT_EINGABE; // Minimalwert für Encoder 1 (Division durch 0 vermeiden)
                    max_counter = MAX_GEWICHT_EINGABE; // Maximalwert für Encoder (Gewicht 2000 g)

                    // editieren der Referenzmenge Gips beginnt ---------------------------

                    //  Bestehendes Gipsgewicht in den Editor schreiben
                    Encoder_count_neu = daten[armposition].gewicht[0];
                    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                    do
                    { // editieren der Referenzmenge Gipsgewicht  -------------------------------

                        read_armposition();
                        if (armposition != armposition_alt)
                        {
                            min_counter = 0;                     // Minimalwert für Encoder
                            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                            Encoder_count_neu = 1;            // Verbleib im Manü case 1, (POS 02 - Referenz Gipsgewicht und Wassergewicht)
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; // Abbruch wenn Armposition sich geändert hat
                        }

                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print("    ");            //  Platzhalter zum Löschen der alten Zahl
                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print(Encoder_count_neu); //  Referenzmenge Gipsgewicht in g

                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)
                    } while (!press_key(ENTER_PIN, SCAN)); // Ende der Eingabe mit ENTER Taste

                    release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

                    lcd.noBlink(); // Cursor blinken ausschalten

                    //  Referenzmenge  Gipsgewicht in die Datenstruktur schreiben
                    daten[armposition].gewicht[0] = Encoder_count_neu;

                    // editieren der Referenzmenge Wasser beginnt ---------------------------

                    //  Bestehende Wassermenge in den Editor schreiben
                    Encoder_count_neu = daten[armposition].gewicht[1];

                    do
                    { // editieren der Referenzmenge Wasser -------------------------------

                        read_armposition();
                        if (armposition != armposition_alt)
                        {
                            min_counter = 0;                     // Minimalwert für Encoder
                            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                            Encoder_count_neu = 1;            // Verbleib im Manü case 1, (POS 02 - Referenz Gipsgewicht und Wassergewicht)
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; // Abbruch wenn Armposition sich geändert hat
                        }

                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(10, 1);         // Setz Curser auf Charakter 1, Zeile 2
                            lcd.print("    ");            //  öschen der alten Zahl
                            lcd.setCursor(10, 1);         // Setz Curser auf Charakter 1, Zeile 2
                            lcd.print(Encoder_count_neu); //  Referenzmenge Wasser in g
                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)
                    } while (!press_key(ENTER_PIN, SCAN)); // Ende der Eingabe mit ENTER Taste

                    release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

                    lcd.noBlink(); // Cursor blinken ausschalten

                    //  Referenzmenge  Wassermenge in die Datenstruktur schreiben
                    daten[armposition].gewicht[1] = Encoder_count_neu;

                    // Referenzmenge Gipsgewicht und Wassermenge wurden editiert, daher Datenspeicherung im EEPROM:
                    EEPROM.put(0, daten);
                    // Schreiben der komletten Variablen (Datenstruktur) Daten auf den EEPROM, ab Adresse 0

                    // Gipsverhältnis berechnen und abspeichern
                    gips_verhaeltnis[armposition] =
                        (daten[armposition].gewicht[0] + daten[armposition].gewicht[1]) //  Gips Referenzgewicht + Wasser Referenzgewicht
                        /                                                               //  dividiert durch
                        daten[armposition].gewicht[0]                                   //  Gips Referenzgewicht
                        ;                                                               //  Ergebnis ist Mischungsverhältnis Gips zu Gesamtmenge in Prozent

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
                    if (Encoder_count_neu == 2) // Cursor der Überschrift löschen
                    {
                        lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
                        lcd.print("   ");    // POS 01 anzeige löschen
                    }

                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("0      g fixiert");
                    lcd.setCursor(1, 1);              // Setz Curser auf Charakter 2, Zeile 2
                    lcd.print(Encoder_count_neu + 1); //  Positionsanzeige
                    lcd.write(byte(CURSOR));
                    lcd.print(daten[armposition].gewicht[Encoder_count_neu]); //  Referenzmenge Gipsgewicht in g

                    write_to_LCD[Encoder_count_neu] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } //  end if (erstmalig)

                // H2O Abfüllung beginnt ---------------------------------------

                if (press_key(I_O_PIN, SCAN)) //  wenn I/O Taste gedrückt wird
                {
                    release_key(I_O_PIN, WAIT); //  Warten bis I/O Taste losgelasen wird

                    if (becher[armposition]) //  Becher der Armposition ist vorhanden
                    {
                        start_time = millis();

                        while (scale.is_ready()) // scale.is_ready --> Waage ist bereit wenn true
                            if (millis() - start_time > WAAGE_READY_TIME)
                                break; // while () ... Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt

                        if (millis() - start_time > WAAGE_READY_TIME) // WAAGE_READY_TIME)
                        {                                             //  Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt
                            lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print("Waage defekt!");

                            Musik(MELODIE_FEHLER);
                            delay(LCD_TIME); // Anzeige der Fehlermeldung

                            Encoder_count_store = OUT_OF_RANGE; // LCD Grundanzeige wieder herstellen
                            //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; //  case 2 - 5 Abbruch, (POS 03 - 06 - Gipsgewicht fix immer um 160/130 g erhöht)
                        } // end if (millis() - start_time < WAAGE_READY_TIME)

                        gewicht_waagschale = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;               // gemessene Grundgewicht
                        teilgewicht_gips = daten[armposition].gewicht[Encoder_count_neu] / gips_verhaeltnis[armposition]; // berechnetes Gipsgewicht
                        teilgewicht_h2o = daten[armposition].gewicht[Encoder_count_neu] - teilgewicht_gips;               // berechnetes Wassergewicht
                        teilgewicht_h2o_waagschale = gewicht_waagschale + teilgewicht_h2o;

                        gesamtgewicht =        // zu messendes Gesamtgewicht =
                            gewicht_waagschale // Eigengewicht der Waagschale
                            +                  // plus
                            teilgewicht_gips   // Gipsgewicht
                            +                  // plus
                            teilgewicht_h2o    // Wassergewicht
                            ;                  // Berechnung des zu messendes Gesamtgewicht

                        //  H2O Abfüllung beginnt ---------------------------------------

                        digitalWrite(relais[armposition + 6], EIN); // Relais H2O-Ventil [ARM Position] einschalten
                        digitalWrite(RELAIS_WP, EIN);               // Relais Wasserpumpe einschalten

                        do // H2O Abfüllung
                        {
                            read_armposition();
                            if (armposition != armposition_alt)
                            {
                                min_counter = 0;                     // Minimalwert für Encoder
                                max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                                //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                                break; // Abbruch wenn Armposition sich geändert hat
                            } // end if (armposition != armposition_alt)

                            // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                            // Dividiert Korrekturfaktor
                            Gewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;

                            LCD_fortschtitt(gesamtgewicht - gewicht_waagschale, Gewicht - gewicht_waagschale); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                        } while (Gewicht < teilgewicht_h2o_waagschale); // Auf Wassergewicht prüfen (inklusive Waagschalen Gewicht)

                        digitalWrite(RELAIS_WP, AUS);               // Relais Wasserpumpe ausschalten
                        digitalWrite(relais[armposition + 6], AUS); // Relais H2O-Ventil [ARM Position] ausschalten

                         Musik(MELODIE_TON_1000);  // Kurzer Signalton zum Ende der Wasserabfüllung

                        //  Gips Abfüllung beginnt ---------------------------------------

                        digitalWrite(relais[armposition], EIN);     // Relais Gipsmotor [ARM Position] einschalten
                        digitalWrite(relais[armposition + 3], EIN); // Relais Rüttler [ARM Position] einschalten

                        do // Gips Abfüllung
                        {
                            read_armposition();
                            if (armposition != armposition_alt)
                            {
                                min_counter = 0;                     // Minimalwert für Encoder
                                max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                                //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                                break; // Abbruch wenn Armposition sich geändert hat
                            } // end if (armposition != armposition_alt)

                            // Waage wird nicht auf Funtionsfähigkeit überprüft,
                            // da dies bereits vor der Gipsabfüllung erfolgte

                            // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                            // Dividiert Korrekturfaktor
                            Gewicht = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;

                            LCD_fortschtitt(gesamtgewicht - gewicht_waagschale, Gewicht - gewicht_waagschale); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                        } while (Gewicht < gesamtgewicht); // Auf Gipsgewicht + Wassergewicht prüfen

                        digitalWrite(relais[armposition], AUS);     // Relais Gipsmotor [ARM Position] ausschalten
                        digitalWrite(relais[armposition + 3], AUS); // Relais Rüttler [ARM Position] ausschalten

                        Musik(MELODIE_OK);
                        //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                        Encoder_count_store = OUT_OF_RANGE; // LCD muss wieder beschrieben werden --> Fortschrittsanzeige

                    } //  end  if (becher[armposition])
                    else
                    {
                        lcd.setCursor(0, 1);
                        lcd.print("Kein Gips-Becher");

                        //       lcd.setCursor(3, 1);
                        //       lcd.print("?Gips-Becher?");

                        Musik(MELODIE_FEHLER);
                        delay(LCD_TIME); // Anzeige der Fehlermeldung

                        Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen, LCD muss wieder beschrieben werden
                    } //  end else  if (becher[armposition])
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
                if (press_key(ENTER_PIN, SCAN)) //  wenn ENTER Taste gedrückt wird
                {
                    release_key(ENTER_PIN, WAIT); //  Warten bis ENTER Taste losgelasen wird

                    Encoder_count_store = Encoder_count_neu; // Zwischenspeicherung der Position

                    //  Bestehendes Gipsgewicht in den Editor schreiben
                    Encoder_count_neu = daten[armposition].gewicht[Encoder_count_neu];
                    Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                    min_counter = MIN_GEWICHT_EINGABE; // Minimalwert für Encoder 1 (Division durch 0 vermeiden)
                    max_counter = MAX_GEWICHT_EINGABE; // Maximalwert für Encoder (Gewicht 2000 g)

                    do
                    { // editieren der Referenzmenge Gipsgewicht  -------------------------------
                        read_armposition();
                        if (armposition != armposition_alt)
                        {
                            min_counter = 0;                     // Minimalwert für Encoder
                            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                            //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; // Abbruch wenn Armposition sich geändert hat
                        } // end if (armposition != armposition_alt)

                        if (Encoder_count_neu != Encoder_count_alt)
                        {
                            Encoder_count_alt = Encoder_count_neu;

                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print("    ");            //  Platzhalter zum Löschen der alten Zahl
                            lcd.setCursor(3, 1);          // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print(Encoder_count_neu); //  Referenzmenge Gipsgewicht in g

                            lcd.blink();
                        } // end if (Encoder_count_neu != Encoder_count_alt)
                        //     } while (digitalRead(ENTER_PIN)); // Ende der Eingabe mit ENTER Taste
                    } while (press_key(ENTER_PIN, SCAN)); // Ende der Eingabe mit ENTER Taste

                    lcd.noBlink(); // Cursor blinken ausschalten

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
                if (press_key(I_O_PIN, SCAN)) //  wenn I/O Taste gedrückt wird
                {
                    release_key(I_O_PIN, WAIT); //  Warten bis I/O Taste losgelasen wird

                    if (becher[armposition]) //  Becher der Armposition ist vorhanden
                    {
                        start_time = millis();

                        while (scale.is_ready()) // scale.is_ready --> Waage ist bereit wenn true
                            if (millis() - start_time > WAAGE_READY_TIME)
                                break; // while () ... Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt

                        if (millis() - start_time > WAAGE_READY_TIME) // WAAGE_READY_TIME)
                        {                                             //  Abbruch da Wartezeit auf Waage ready zu lange, eventuell Waage defekt
                            lcd.setCursor(3, 1);                      // Setz Curser auf Charakter 4, Zeile 2
                            lcd.print("Waage defekt!");

                            Musik(MELODIE_FEHLER);
                            delay(LCD_TIME); // Anzeige der Fehlermeldung

                            Encoder_count_store = OUT_OF_RANGE; // LCD Grundanzeige wieder herstellen
                            //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; //  case 2 - 5 Abbruch, (POS 03 - 06 - Gipsgewicht fix immer um 160/130 g erhöht)
                        } // end if (millis() - start_time < WAAGE_READY_TIME)

                        gewicht_waagschale = (scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor;               // gemessene Grundgewicht
                        teilgewicht_gips = daten[armposition].gewicht[Encoder_count_neu] / gips_verhaeltnis[armposition]; // berechnetes Gipsgewicht
                        teilgewicht_h2o = daten[armposition].gewicht[Encoder_count_neu] - teilgewicht_gips;               // berechnetes Wassergewicht                                                                                                               // berechnetes Wassergewicht
                        teilgewicht_h2o_waagschale = gewicht_waagschale + teilgewicht_h2o;

                        gesamtgewicht =        // zu messendes Gesamtgewicht =
                            gewicht_waagschale // Eigengewicht der Waagschale
                            +                  // plus
                            teilgewicht_gips   // Gipsgewicht
                            +                  // plus
                            teilgewicht_h2o    // Wassergewicht
                            ;                  // Berechnung des zu messendes Gesamtgewicht

                        //  H2O Abfüllung beginnt ---------------------------------------

                        digitalWrite(relais[armposition + 6], EIN); // Relais H2O-Ventil [ARM Position] einschalten
                        digitalWrite(RELAIS_WP, EIN);               // Relais Wasserpumpe einschalten

                        do // H2O Abfüllung
                        {
                            read_armposition();
                            if (armposition != armposition_alt)
                            {
                                min_counter = 0;                     // Minimalwert für Encoder
                                max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                                //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                                break; // Abbruch wenn Armposition sich geändert hat
                            } // end if (armposition != armposition_alt)

                            // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                            // Dividiert Korrekturfaktor
                            // Abzüglich des Eigengewicht der Waagscghale
                            Gewicht = ((scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor); // Messung Gewicht Gips + Gewicht Waagschale

                            LCD_fortschtitt(gesamtgewicht - gewicht_waagschale, Gewicht - gewicht_waagschale); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                        } while (Gewicht < teilgewicht_h2o_waagschale); // Auf Gipsgewicht prüfen (inklusive Waagschalen Gewicht)

                        digitalWrite(RELAIS_WP, AUS);               // Relais Wasserpumpe ausschalten
                        digitalWrite(relais[armposition + 6], AUS); // Relais H2O-Ventil [ARM Position] ausschalten
                        
                        Musik(MELODIE_TON_1000);  // Kurzer Signalton zum Ende der Wasserabfüllung
                        
                        //  Gips Abfüllung beginnt ---------------------------------------

                        digitalWrite(relais[armposition], EIN);     // Relais Gipsmotor [ARM Position] einschalten
                        digitalWrite(relais[armposition + 3], EIN); // Relais Rüttler [ARM Position] einschalten

                        do // Gips Abfüllung
                        {
                            read_armposition();
                            if (armposition != armposition_alt)
                            {
                                min_counter = 0;                     // Minimalwert für Encoder
                                max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                                //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                                break; // Abbruch wenn Armposition sich geändert hat
                            } // end if (armposition != armposition_alt)

                            // Waage wird nicht auf Funtionsfähigkeit überprüft,
                            // da dies bereits vor der Gipsabfüllung erfolgte

                            // Differenz der Gewichtseinheit minus der Leereinheitdurch,
                            // Dividiert Korrekturfaktor
                            // Abzüglich des Eigengewicht der Waagscghale
                            Gewicht = ((scale.read_average(3) - Leergew_einheiten) / Korrekturfaktor); // gemessenes Gewicht inklusive der Waagschale

                            LCD_fortschtitt(gesamtgewicht - gewicht_waagschale, Gewicht - gewicht_waagschale); // Fortschritt der Abfüllung auf LCD grafisch anzeigen
                        } while (Gewicht < gesamtgewicht); // Auf Gipsgewicht + Wassergewicht prüfen

                        digitalWrite(relais[armposition], AUS);     // Relais Gipsmotor [ARM Position] ausschalten
                        digitalWrite(relais[armposition + 3], AUS); // Relais Rüttler [ARM Position] ausschalten

                        Musik(MELODIE_OK);
                        //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 6, 7, 8, 9
                        Encoder_count_store = OUT_OF_RANGE; // LCD muss wieder beschrieben werden --> Fortschrittsanzeige
                    } //  end  if (becher[armposition])
                    else
                    {
                        lcd.setCursor(0, 1);
                        lcd.print("Kein Gips-Becher");

                        //       lcd.setCursor(3, 1);
                        //       lcd.print("?Gips-Becher?");

                        Musik(MELODIE_FEHLER);
                        delay(LCD_TIME); // Anzeige der Fehlermeldung

                        Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen, LCD muss wieder beschrieben werden
                    } //  end else  if (becher[armposition])
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
                if (press_key(I_O_PIN, SCAN)) //  wenn I/O Taste gedrückt wird
                {
                    if (becher[armposition]) //  Becher der Armposition ist vorhanden
                    {
                        on_off_encoder = false; // Encoder Interrupt ausschalten

                        digitalWrite(relais[armposition], EIN); // Relais Gipsmotor[ARM Position] einschalten

                        do
                        {
                            read_armposition();
                            if (armposition != armposition_alt)
                            {
                                min_counter = 0;                     // Minimalwert für Encoder
                                max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                                //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                                break; // Abbruch wenn Armposition sich geändert hat
                            } // end if (armposition != armposition_alt)

                        } while (!release_key(I_O_PIN, SCAN)); // Relais halten bis I/O Taste losgelassen wird

                        digitalWrite(relais[armposition], AUS); // Relais Gipsmotor [ARM Position] ausschalten
                    } //  end  if (becher[armposition])
                    else
                    {
                        lcd.setCursor(0, 1);
                        lcd.print("Kein Gips-Becher");

                        //       lcd.setCursor(3, 1);
                        //       lcd.print("?Gips-Becher?");

                        Musik(MELODIE_FEHLER);
                        delay(LCD_TIME); // Anzeige der Fehlermeldung

                        Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen, LCD muss wieder beschrieben werden
                    } //  end else  if (becher[armposition])

                    on_off_encoder = true;  // Encoder Interrupt einschalten
                    Encoder_count_neu = 10; //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 10
                } // end  if (!digitalRead(I_O_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
                break;                            //  end case 10, (POS 11 - Gipsentleerung)

            case 11: /*  POS 12 - Wasserentnahme  */
                if (write_to_LCD[11])
                {
                    lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
                    lcd.print("   ");    // POS 01 anzeige löschen

                    lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                    lcd.print("12 H2O  Entnahme");
                    lcd.setCursor(2, 1); // Setz Curser auf Charakter 3, Zeile 2
                    lcd.write(byte(CURSOR));

                    write_to_LCD[11] = false; // damit immer nur einmalig auf den LCD geschrieben wird
                } // end if (erstmalig)

                // Wasserentnahme beginnt ---------------------------------------
                if (press_key(I_O_PIN, SCAN)) //  wenn I/O Taste gedrückt wird
                {
                    on_off_encoder = false; // Encoder Interrupt ausschalten

                    digitalWrite(RELAIS_WP, EIN); // Relais Wasserpumpe einschalten

                    do
                    {
                        read_armposition();
                        if (armposition != armposition_alt)
                        {
                            min_counter = 0;                     // Minimalwert für Encoder
                            max_counter = MAX_CURSOR_POSITIONEN; // Maximalwert für Encoder (0 bis 11, also 12 Positionen)

                            //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 2, 3, 4, 5
                            Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                            break; // Abbruch wenn Armposition sich geändert hat
                        } // end if (armposition != armposition_alt)
                    } while (!release_key(I_O_PIN, SCAN)); // Relais halten bis I/O Taste losgelassen wird

                    digitalWrite(RELAIS_WP, AUS); // Relais Wasserpumpe ausschalten

                    on_off_encoder = true;  // Encoder Interrupt einschalten
                    Encoder_count_neu = 11; //  Encoder_count_neu bleibt unverändert, Verbleib im Manü case 11
                } // end  if (!digitalRead(I_O_PIN))

                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen

                break; //  end case 11, (POS 12 - Wasserentnahme)

            default:
                lcd.setCursor(0, 1);           // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("!!!! Error !!!!!"); // allgemeiner Fehler

                Musik(MELODIE_FEHLER);
                delay(LCD_TIME);                  // Anzeige der Fehlermeldung
                Encoder_count_alt = OUT_OF_RANGE; // Erststartbedingung herstellen
                break;
            } // end switch (curser_position)
        } // end if (Encoder_count_neu != Encoder_count_alt
    } while (true);
} //  end mainprogramm()
////////////////////////////////// Ende mainprogramm  ///////////////////////////////////////////
