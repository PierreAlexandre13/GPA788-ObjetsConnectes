# 1 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
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
# 29 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
// Classe pour mesurer la température interne de l'ATmega328P 
# 31 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 2

# 33 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 2
# 34 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 2



# 36 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
const uint32_t SERIAL_BAUD_RATE{115200}; // Terminal serie
const uint8_t PIN{A0}; // Broche du Capteur sonore
const uint32_t TS = 62; // Péruide d'échantillionnage (ms)
const uint16_t NB_SAMPLE = 32; // 32 x 62 ms ~ 2 secondes
const uint16_t NB_LI = 2; // 150 x 2 secondes = 5 minutes (*)
uint32_t countMillis; // Compter les minutes (pour debug seulement)
uint32_t timerTs;



Calculateur_Leq leq(TS, NB_SAMPLE, NB_LI);



const uint8_t ADR_NOEUD{0x44}; // Adresse I2C de ce noeud
const uint8_t NB_REGISTRES{11}; // Nombre de registres de ce noeud

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

union CarteRegistres cr; // Une carte des registres
//float temperature;                     // Variable intermédiaire pour mémoriser la température
uint8_t adrReg; // Adresse du registre reçue du coordonnateur
enum class CMD { Stop = 0, Go}; // Commandes venant du coordonnateur
volatile CMD cmd; // Go -> échantilloner, Stop -> arrêter
const uint8_t MIN_Ts = 1; // Période d'échantillonnage min (sec)
const uint8_t MAX_Ts = 200; // Période d'échantillonnage max (sec)
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
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 103 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 103 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              "Noeud à l'adresse 0x"
# 103 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              ); &__c[0];}))
# 103 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              ))); Serial.print(ADR_NOEUD, 16);
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 104 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 104 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                " prêt à recevoir des commandes"
# 104 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                ); &__c[0];}))
# 104 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                )));

  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              "Ts = "
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              ); &__c[0];}))
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              ))); Serial.print(leq.GetTs()); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                                   (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                                   "ms\t"
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                                   ); &__c[0];}))
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                                   )));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 107 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 107 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              "ti = "
# 107 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              ); &__c[0];}))
# 107 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              ))); Serial.print(leq.GetTs() * leq.GetVrmSamples() / 1000.0);
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 108 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 108 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              "s\t"
# 108 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              ); &__c[0];}))
# 108 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              )));
  Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 109 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 109 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              "tp = "
# 109 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
              ); &__c[0];}))
# 109 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
              ))); Serial.print((leq.GetTs() * leq.GetVrmSamples() / 1000.0) * leq.GetLiSamples() / 60.0);
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 110 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 110 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                "min"
# 110 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                ); &__c[0];}))
# 110 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                )));
  Serial.println("Leq (dB SPL)\tMinutes écoulées");

  // Pour l'ADC du microcontrôleur...
  analogReference(0); // utiliser VREF externe pour l'ADC
  pinMode(PIN, 0x0); // capteur sonore à la broche PIN
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
      Serial.print(leq.GetLeq(), 3); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                 "\t\t\t"
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                 ); &__c[0];}))
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                 )));
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
      
# 143 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
     for ( uint8_t sreg_save __attribute__((__cleanup__(__iRestore))) = (*(volatile uint8_t *)((0x3F) + 0x20)), __ToDo = __iCliRetVal(); __ToDo ; __ToDo = 0 ) 
# 143 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                       {
        // Assigner la température lue dans cr.champs.temperature
        cr.champs.Leq = leq.GetLeq();
        cr.champs.temps = tempo;
        // Augmenter le compte du nombre d'échantillons
        cr.champs.nb_echantillons++;
      }
      timerTs = millis();
      Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  "#"
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  ); &__c[0];}))
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  ))); Serial.print(cr.champs.nb_echantillons); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                                                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                                                 "  "
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                                                 ); &__c[0];}))
# 151 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                                                 )));
      Serial.print(cr.champs.Leq); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 152 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                               (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 152 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                               "  "
# 152 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                               ); &__c[0];}))
# 152 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                               ))); Serial.println(cr.champs.temps);
    }


  }

}





void waitUntil(uint32_t w) {
  uint32_t t = millis();
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
# 183 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
void i2c_receiveEvent(int n) {
  // Traiter les commandes ou le numéro d'un registre (1 octet)
  if (n == 1) {
    // Un seul octet reçu. C'est probablement une commande.
    uint8_t data = Wire.read();
    switch (data) {
      case 0xA1:
        cmd = CMD::Stop;
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 191 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                      (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 191 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                      "commande 'Arrêter' reçue"
# 191 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                      ); &__c[0];}))
# 191 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                      )));
        break;
    case 0xA2:
        cmd = CMD::Go;
        cr.champs.nb_echantillons = 0;
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 196 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                      (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 196 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                      "Commande 'Démarrer' reçue"
# 196 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                      ); &__c[0];}))
# 196 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                      )));
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
      Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 213 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                    (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 213 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                    "Commande 'Changer Ts' reçue"
# 213 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                    ); &__c[0];}))
# 213 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                    )));
      cr.champs.Ts = data2;
      Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 215 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 215 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  "La nouvelle valeur est: "
# 215 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  ); &__c[0];}))
# 215 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  )));
      Serial.print(cr.champs.Ts); Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 216 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 216 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                " secondes"
# 216 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                                                ); &__c[0];}))
# 216 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                                                )));
    }
    // set nb_samples
    if ((data1 == 0xA3)) {
      Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 220 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 220 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  "Commande 'SET nb_sample' reçue : "
# 220 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  ); &__c[0];}))
# 220 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  )));
      leq.setVrmSamples(data2);
      Serial.print(leq.GetVrmSamples());
      Serial.print(", nouveau ti = ");
      Serial.println(leq.GetTs() * leq.GetVrmSamples() / 1000.0);
    }
    // set nb_li
    if ((data1 == 0xA4)) {
      Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 228 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 228 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  "Commande 'SET nb_li' reçue : "
# 228 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  ); &__c[0];}))
# 228 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  )));
      leq.setLiSamples(data2);
      Serial.print(leq.GetLiSamples());
      Serial.print(", nouveau tp = ");
      Serial.println((leq.GetTs() * leq.GetVrmSamples() / 1000.0) * leq.GetLiSamples() / 60.0);
    }

  }
  else {
    // Ignorer la réception n > 2 octets.
    Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 238 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 238 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  "Erreur: ce noeud n'accepte    pas de communication/commande à trois octets"
# 238 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino" 3
                  ); &__c[0];}))
# 238 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
                  ))
                                                   );
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
# 253 "/home/pa/Documents/ETS/objetsConnectes/labo7/max-2/max-2.ino"
void i2c_requestEvent(){
  // Le coordonnateur veut la valeur d'un registre. L'adresse du
  // registre a été reçue précédemment.
  if ((adrReg >= 0) && (adrReg < NB_REGISTRES)){
    Serial.print("");
    // Envoyer le contenu du registre au coordonnateur
    Wire.write(cr.regs[adrReg]);
  }
}
