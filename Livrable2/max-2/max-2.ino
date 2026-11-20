/*
 * test_lcd.ino
 * Utiliser la bibliothèque LiquidCrystal pour faire afficher
 * des messages/données sur l'afficheur LCD.
 * 
 * L'afficheur LCD pourrait être intégré dans le projet IoT
 * de ce cours.
 * 
 * Note : (Août 2024) Le message "Visual Studio Code extension for 
 *        Arduino (deprecated)" apparaît lors du chargement de l'extension.
 *        Cependant, elle continue de fonctionner correctement.
 * 
 * 
 * GPA788 - ETS
 * T. Wong
 * 08-2019
 * 08-2020
 * 08/2024 (Arduino extension is now deprecated)
 * 
 * Ce programme est inspéré de l'exemple "Hello World!" de la page:
 * https://www.arduino.cc/en/Tutorial/HelloWorld. La classe LiquidCrystal()
 * et ses fonctions membres sont listées sur la page: 
 * https://www.arduino.cc/en/Reference/LiquidCrystal
 * Le branchement électrique est présenté d'une façon plus évidente dans le
 * protocole de laboratoire du cours. Finalement, la technique de lecture
 * de la température interne de l'ATmega328P est donnée dans les notes
 * de cours.
 */
// Classe pour mesurer la température interne de l'ATmega328P 
#include "calculateur_leq.h"

#include <Wire.h>                      // Pour la communication I2C               
#include <util/atomic.h>               // Pour la section critique


const uint32_t SERIAL_BAUD_RATE{115200};    // Terminal serie
const uint8_t PIN{A0};                      // Broche du Capteur sonore
const uint32_t TS = 62;                     // Péruide d'échantillionnage (ms)
const uint16_t NB_SAMPLE = 32;              // 32 x 62 ms ~ 2 secondes
const uint16_t NB_LI = 2;                 // 150 x 2 secondes = 5 minutes (*)
uint32_t countMillis;                       // Compter les minutes (pour debug seulement)
uint32_t timerTs;



Calculateur_Leq leq(TS, NB_SAMPLE, NB_LI);



const uint8_t ADR_NOEUD{0x44};        // Adresse I2C de ce noeud
const uint8_t NB_REGISTRES{11};        // Nombre de registres de ce noeud

/* La carte des registres */
union CarteRegistres {
  // Cette structure: Utilisée par le noeud pour lire et écrire
  //                  des données.
  struct {
    // Taux d'échantillonnage (1 octet)
    volatile uint8_t Ts;
    // Nombre d'échantillons (2 octets)
    volatile int16_t nb_echantillons;
    // Température interne du processeur ATMEGA en celsius
    // (4 octets)
    volatile float Leq;
    volatile float temps;
  } champs;
  // Ce tableau: Utilisé par le coordonnateur pour lire et écrire
  //             des données.
  uint8_t regs[NB_REGISTRES];
};

union CarteRegistres cr;               // Une carte des registres
//float temperature;                     // Variable intermédiaire pour mémoriser la température
uint8_t adrReg;                        // Adresse du registre reçue du coordonnateur
enum class CMD { Stop = 0, Go};        // Commandes venant du coordonnateur
volatile CMD cmd;                      // Go -> échantilloner, Stop -> arrêter
const uint8_t MIN_Ts = 1;              // Période d'échantillonnage min (sec)
const uint8_t MAX_Ts = 200;            // Période d'échantillonnage max (sec)
float tempo;

void setup() {
  // Pour afficher la température sur le terminal série
  Serial.begin(SERIAL_BAUD_RATE);

  // Initialiser les champs de la carte des registres
  cr.champs.Ts = MIN_Ts;
  cr.champs.nb_echantillons = 0;
  cr.champs.Leq = 0;
  cr.champs.temps = 0;
  // Initialiser les variables de contrôle de la
  // communication I2C
  cmd = CMD::Stop;
  adrReg = -1;
  // Réglage de la bibliothèque Wire pour le I2C
  Wire.begin(ADR_NOEUD);
  // Fonction pour traiter la réception de données venant du coordonnateur
  Wire.onReceive(i2c_receiveEvent);
  // Fonction pour traiter une requête de données venant du coordonnateur
  Wire.onRequest(i2c_requestEvent);


  // Indiquer que le noeud est prêt
  Serial.print(F("Noeud à l'adresse 0x")); Serial.print(ADR_NOEUD, HEX);
  Serial.println(F(" prêt à recevoir des commandes"));

  Serial.print(F("Ts = ")); Serial.print(leq.GetTs()); Serial.print(F("ms\t"));
  Serial.print(F("ti = ")); Serial.print(leq.GetTs() * leq.GetVrmSamples() / 1000.0);
  Serial.print(F("s\t"));
  Serial.print(F("tp = ")); Serial.print((leq.GetTs() * leq.GetVrmSamples() / 1000.0) * leq.GetLiSamples() / 60.0);
  Serial.println(F("min"));
  Serial.println("Leq (dB SPL)\tMinutes écoulées");

  // Pour l'ADC du microcontrôleur...
  analogReference(EXTERNAL);                // utiliser VREF externe pour l'ADC
  pinMode(PIN, INPUT);                      // capteur sonore à la broche PIN
  // Pour debug seulement
  countMillis = millis();  
  timerTs = 0;
 
}



void loop() {
  
  // Échantillonner la température interne si la commande est Go
  if (cmd == CMD::Go) {
    leq.Accumulate();
    // L'objet leq "sait" à quel moment il faut calculer Vrms, Li et Leq
    if (leq.Compute() ) {
      Serial.print(leq.GetLeq(), 3); Serial.print(F("\t\t\t"));
      Serial.println((1.0 * millis() - countMillis) / 60000);
      tempo = (1.0 * millis() - countMillis) / 60000;
      countMillis = millis();
    }
    // Section critique: empêcher les interruptions lors de l'assignation
    // de la valeur de la température à la variable dans la carte des registres.
    // Recommandation: réaliser la tâche la plus rapidement que possible dans
    //                 la section critique.
    
    //if(millis() - timerTs > cr.champs.Ts * 1000){ 
    if(millis() - timerTs > leq.GetTs() * leq.GetVrmSamples() * leq.GetLiSamples()){ // m_ts*m_nb_sample*m_nb_li
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Assigner la température lue dans cr.champs.temperature
        cr.champs.Leq = leq.GetLeq();
        cr.champs.temps = tempo;
        // Augmenter le compte du nombre d'échantillons
        cr.champs.nb_echantillons++;
      }
      timerTs = millis();
      Serial.print(F("#")); Serial.print(cr.champs.nb_echantillons); Serial.print(F("  "));
      Serial.print(cr.champs.Leq); Serial.print(F("  ")); Serial.println(cr.champs.temps);
    }


  }

}





void waitUntil(uint32_t w) {
  uint32_t t =  millis();
  // Attendre w millisecondes
  while (millis() < t + w) {}
}




/* ------------------------------------------------------------------
   i2c_receiveFunc(int n)
   Cette fonction est exécutée à la réception des données venant
   du coordonnateur. Le paramètre n indique le nombre d'octets reçus.
   ------------------------------------------------------------------
   Note: Normalement on ne doit pas afficher des messages utilisant
         le port série - il y a risque de conflit entre les inter-
         ruptions. Donc, après débogage, n'oubliez pas de mettre les
         Serial print en commentaires ;-).
   ------------------------------------------------------------------ */
void i2c_receiveEvent(int n) {
  // Traiter les commandes ou le numéro d'un registre (1 octet)
  if (n == 1) {
    // Un seul octet reçu. C'est probablement une commande.
    uint8_t data = Wire.read();
    switch (data) {
      case 0xA1:
        cmd = CMD::Stop;
        Serial.println(F("commande 'Arrêter' reçue"));
        break;
    case 0xA2:
        cmd = CMD::Go;
        cr.champs.nb_echantillons = 0;
        Serial.println(F("Commande 'Démarrer' reçue"));
        break;
    default:
        // Sinon, c'est probablement un numéro de registre
        if ((data >= 0) && (data < NB_REGISTRES)) {
          adrReg = data;
        }
        else
          adrReg = -1; // Il y sans doute une erreur!
    }
  } 
  else if (n == 2) {
    // Deux octets reçus. C'est probablement pour changer le
    // taux d'échantillonnage.
    uint8_t data1 = Wire.read();
    uint8_t data2 = Wire.read();
    if ((data1 == 0xA0) && (data2 >= MIN_Ts) && (data2 <= MAX_Ts)) {
      Serial.println(F("Commande 'Changer Ts' reçue")); 
      cr.champs.Ts = data2;
      Serial.print(F("La nouvelle valeur est: ")); 
      Serial.print(cr.champs.Ts); Serial.println(F(" secondes"));
    }
    // set nb_samples
    if ((data1 == 0xA3)) {
      Serial.print(F("Commande 'SET nb_sample' reçue : ")); 
      leq.setVrmSamples(data2);
      Serial.print(leq.GetVrmSamples()); 
      Serial.print(", nouveau ti = "); 
      Serial.println(leq.GetTs() * leq.GetVrmSamples() / 1000.0); 
    }
    // set nb_li
    if ((data1 == 0xA4)) {
      Serial.print(F("Commande 'SET nb_li' reçue : ")); 
      leq.setLiSamples(data2);
      Serial.print(leq.GetLiSamples()); 
      Serial.print(", nouveau tp = "); 
      Serial.println((leq.GetTs() * leq.GetVrmSamples() / 1000.0) * leq.GetLiSamples() / 60.0); 
    }

  }
  else {
    // Ignorer la réception n > 2 octets.
    Serial.println(F("Erreur: ce noeud n'accepte\
    pas de communication/commande à trois octets"));
  }
}

/* ------------------------------------------------------------------
   i2c_requestEvent()
   Cette fonction est exécutée à la réception d'une requête de
   données venant du coordonnateur.
   ------------------------------------------------------------------
   Note: Normalement on ne doit pas afficher des messages utilisant
         le port série - il y a risque de conflit entre les inter-
         ruptions. Donc, après débogage, n'oubliez pas de mettre les
         Serial print en commentaires ;-).
   ------------------------------------------------------------------ */
void i2c_requestEvent(){
  // Le coordonnateur veut la valeur d'un registre. L'adresse du
  // registre a été reçue précédemment.
  if ((adrReg >= 0) && (adrReg < NB_REGISTRES)){
    Serial.print("");
    // Envoyer le contenu du registre au coordonnateur
    Wire.write(cr.regs[adrReg]);
  }
}
