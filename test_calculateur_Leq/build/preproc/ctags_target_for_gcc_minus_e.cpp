# 1 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
   /*

 * test_calculateur_Leq.ino

 * 

 * Programme de test servant à tester l'implantation de la classe

 * Calculateur_Leq. La quantité Leq d'un signal est le niveau d'énergie 

 * sonore équivalent pour durée déterminée. 

 *

 * Pour ce programme la fréquence d'échantillonnage est

 * réglée à fs = 16 Hz (62.5 ms) et la valeur Li sera calculée

 * à 1 Hz (1 seconde). Finalement, Leq sera calculé pour une

 * durée de 5 minutes.

 * 

 * Note : (Août 2024) Le message "Visual Studio Code extension for 

 *         Arduino (deprecated)" apparaît lors du chargement de l'extension.

 *         Cependant, elle continue de fonctionner correctement.

 * 

 * Convention:

 *  Variables -> camelCase

 *  Classes, fonctions -> PascalCase

 *  Constantes, types utilisateurs -> SNAKE_CASE

 * 

 * 

 * GPA788 - ETS

 * T. Wong

 * 09-2018

 * 08-2020

 * 08-2024

 */
# 30 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
// Pour pouvoir utiliser un objet de type Calculateur_Leq
# 32 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 2

/* -------------------------------------------------------------

   Constantes et variables globales

   -------------------------------------------------------------- */
# 36 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
const uint32_t SERIAL_BAUD_RATE{115200}; // Terminal serie
const uint8_t PIN{A0}; // Broche du Capteur sonore
const uint32_t TS = 62; // Péruide d'échantillionnage (ms)
const uint16_t NB_SAMPLE = 32; // 32 x 62 ms ~ 2 secondes
const uint16_t NB_LI = 2; // 150 x 2 secondes = 5 minutes (*)
uint32_t countMillis; // Compter les minutes (pour debug seulement)
// (*) Évidemment vous pouvez réduire la période d'échantillonnage durant la phase
//     de déboggage (;-))

/* -------------------------------------------------------------

   Créer un objet Calculateur_Leq en utilisant des paramètres

   spécifiques.

   ------------------------------------------------------------- */
# 49 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
Calculateur_Leq leq(TS, NB_SAMPLE, NB_LI);

/* -------------------------------------------------------------

   Initialisation de l'ADC du microcontrôleur et affciher des

   messages sur le terminal série.

   ------------------------------------------------------------- */
# 55 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
// Macro pour extraire le nom de ce fichier (facultatif)
# 57 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 2


# 58 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
void setup()
{
  // Initialiser le terminal série
  Serial.begin(SERIAL_BAUD_RATE);

  // Afficher les paramètres de fonctionnement du programme sur le terminal série
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              "<"
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              ); &__c[0];}))
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              ))); Serial.print((strrchr("/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino", '\\') ? strrchr("/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino", '\\') + 1 : "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino")); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                                                                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                                                                ">"
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                                                                ); &__c[0];}))
# 64 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                                                                )));
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 65 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 65 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                " Démonstration de calcul Leq"
# 65 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                ); &__c[0];}))
# 65 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                )));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              "Ts = "
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              ); &__c[0];}))
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              ))); Serial.print(leq.GetTs()); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                                                                   (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                                                                   "ms\t"
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                                                                   ); &__c[0];}))
# 66 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                                                                   )));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 67 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 67 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              "ti = "
# 67 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              ); &__c[0];}))
# 67 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              ))); Serial.print(leq.GetTs() * leq.GetVrmSamples() / 1000.0);
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 68 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 68 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              "s\t"
# 68 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              ); &__c[0];}))
# 68 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              )));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 69 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 69 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              "tp = "
# 69 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
              ); &__c[0];}))
# 69 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
              ))); Serial.print((leq.GetTs() * leq.GetVrmSamples() / 1000.0) * leq.GetLiSamples() / 60.0);
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 70 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 70 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                "min"
# 70 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                ); &__c[0];}))
# 70 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                )));
  Serial.println("Leq (dB SPL)\tMinutes écoulées");

  // Pour l'ADC du microcontrôleur...
  analogReference(0); // utiliser VREF externe pour l'ADC
  pinMode(PIN, 0x0); // capteur sonore à la broche PIN
  // Pour debug seulement
  countMillis = millis(); // compter le nb. de ms écoulées
}

/* -------------------------------------------------------------

   À chaque exécution de loop(), on exécute les fonctions

   membres Accumulate et Compute() du Calculateur_Leq.



   La temporisation s'effectue dans ces fonctions membres

   simplifiant ainsi leur utilisation.

   ------------------------------------------------------------- */
# 87 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
void loop()
{
  // L'objet leq "sait" à quel moment il doit accumuler les valeurs
  // du signal sonore.
  leq.Accumulate();
  // L'objet leq "sait" à quel moment il faut calculer Vrms, Li et Leq
  if (leq.Compute() ) {
    Serial.print(leq.GetLeq(), 3); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 94 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                                               (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 94 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                                               "\t\t\t"
# 94 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino" 3
                                               ); &__c[0];}))
# 94 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
                                               )));
    Serial.println((1.0 * millis() - countMillis) / 60000);
    countMillis = millis();
  }
}

/* ---------------------------------------------------------------

   Fonction pour créer un delai de w millisecondes

   

   La fonction delay() est utilisée dans bien des tutoriels pour

   créer un delai temporel mais elle bloque les interruption. On

   peut créer notre propre délai qui ne bloquent pas et utiliser 

   une unité de temps à notre guise.

   --------------------------------------------------------------- */
# 108 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/test_calculateur_Leq.ino"
void waitUntil(uint32_t w) {
  uint32_t t{millis()};
  // Attendre w millisecondes
  while (millis() < t + w) {}
}
