#include "hauptprogramm.h"

///////////////////////////////////// Anfang Begrüßugng auf dem LCD  ///////////////////////////////////////////
//  Begrüßung auf dem LCD Display
//  und Entscheidung ob nicht offizielle Service Routine oder das Hauptprogramm gestartet wird

void greeting()
{
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
            } // end else if (delta_time < WAIT_TIME_3)
            else if (millis() - start_time < WAIT_TIME_4) // Anzeige der dritten 2 Sekunden
            {
                lcd.setCursor(0, 0); // Setz Curser auf Charakter 1, Zeile 1
                lcd.print("    Richard     ");
                lcd.setCursor(0, 1); // Setz Curser auf Charakter 1, Zeile 2
                lcd.print("    LEOPOLD     ");
            } // end else if (delta_time < WAIT_TIME_4)
        } // end end if (delta_time < WAIT_TIME_2)
    } while (millis() - start_time < WAIT_TIME_4);
} //  end greeting()
///////////////////////////////////// Ende Begrüßugng auf dem LCD  ///////////////////////////////////////////

///////////////////////////////////// Begin cursor_start  ///////////////////////////////////////////
//  Gibt die Position des übergebenen Charakters zurück,
//  damit das editieren mit dem bestehenden Charakter beginnen kann
int edit_cursor_start(char buchstabe)
{
    for (int i = 0; i <= anzahl_texteingabe; i++)
        if (texteingabe[i] == buchstabe)
            return i; // Position des Charakters im Array texteingabe zurückgeben

    return 0; // wenn der Character nicht gefunden wurde, erster Char der der Liste (A) zurückgeben

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

