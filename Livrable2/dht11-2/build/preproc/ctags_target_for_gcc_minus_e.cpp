# 1 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
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
# 29 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
// Classe pour mesurer la température interne de l'ATmega328P 
# 31 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 2

# 33 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 2

# 35 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 2
# 36 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 2



# 38 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
const uint32_t SERIAL_SPEED{115200};
dhtlib_gpa788 MyDht;

// Interruption à toutes les 3 secondes
const uint32_t TIMER_PERIOD{3000000};
const int DHT11_PIN{7};

// On doit utiliser le modificateur volatile ici
// car la variable chk est utilisée dans une fonction
// de gestion d'interruption.
volatile DHTLIB_ErrorCode chk;


const uint8_t ADR_NOEUD{0x45}; // Adresse I2C de ce noeud
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
    volatile float temperature;
    volatile float humidite;
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


void setup() {
  // Pour afficher la température sur le terminal série
  Serial.begin(SERIAL_SPEED);

  // Initialiser les champs de la carte des registres
  cr.champs.Ts = MIN_Ts;
  cr.champs.nb_echantillons = 0;
  cr.champs.temperature = static_cast<float>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);
  cr.champs.humidite = static_cast<float>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);

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
# 105 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
              (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 105 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
              "Noeud à l'adresse 0x"
# 105 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
              ); &__c[0];}))
# 105 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
              ))); Serial.print(ADR_NOEUD, 16);
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                " prêt à recevoir des commandes"
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                ); &__c[0];}))
# 106 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                )));



  // Timer1.initialize(TIMER_PERIOD);
  // Timer1.attachInterrupt(readTemperature);
  // Timer1.start();

  MyDht.Connect();

}



void loop() {

  // Échantillonner la température interne si la commande est Go
  if (cmd == CMD::Go) {
    //temperature = ct.celsius();
    chk = MyDht.read11(DHT11_PIN);
    // Section critique: empêcher les interruptions lors de l'assignation
    // de la valeur de la température à la variable dans la carte des registres.
    // Recommandation: réaliser la tâche la plus rapidement que possible dans
    //                 la section critique.
    
# 130 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
   for ( uint8_t sreg_save __attribute__((__cleanup__(__iRestore))) = (*(volatile uint8_t *)((0x3F) + 0x20)), __ToDo = __iCliRetVal(); __ToDo ; __ToDo = 0 ) 
# 130 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                     {
      // Assigner la température lue dans cr.champs.temperature
      cr.champs.temperature = MyDht.getTemperature();
      cr.champs.humidite = MyDht.getHumidity();
      // Augmenter le compte du nombre d'échantillons
      cr.champs.nb_echantillons++;
    }
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                "#"
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                ); &__c[0];}))
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                ))); Serial.print(cr.champs.nb_echantillons); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                                                                               (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                                                               "  "
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                                                                               ); &__c[0];}))
# 137 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                                                               )));
    Serial.print(cr.champs.temperature); Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 138 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                                                     (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 138 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                                     "  "
# 138 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                                                     ); &__c[0];}))
# 138 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                                     ))); Serial.println(cr.champs.humidite);

    // Attendre la prochaine période d'échantillonnage
    waitUntil(cr.champs.Ts * 1000);

  }

}


void showHumTemp() {

  //chk = MyDht.read11(DHT11_PIN);

  if(chk == DHTLIB_ErrorCode::DHTLIB_OK){
    // Afficher des informations sur le moniteur série
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 154 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 154 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                "Humidite: "
# 154 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                ); &__c[0];}))
# 154 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                ))); Serial.println(MyDht.getHumidity());
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 155 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 155 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                "Temperature: "
# 155 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                ); &__c[0];}))
# 155 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                ))); Serial.println(MyDht.getTemperature());
    }
    else{
        Serial.print("Erreur Capteur temperature : "); Serial.println(static_cast<int>(chk));
    }
}


void waitUntil(uint32_t w) {
  uint32_t t = millis();
  // Attendre w millisecondes
  while (millis() < t + w) {}
}


void readTemperature()
{
// -------------------------------------------------------------------------------
// Interruption du compteur reçue. Il est temps de faire la lecture du capteur
// DHT11
// -------------------------------------------------------------------------------
// Normalement, les interruptions sont bloquées lors du traitement des
// interruptions. On doit les permettre ici car la fonction membre read11()
// utilise delay() pour initier la communication série avec le capteur.
// -------------------------------------------------------------------------------
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 180 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 180 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                "Interruption"
# 180 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                ); &__c[0];}))
# 180 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                )));

  
# 182 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
 __asm__ __volatile__ ("sei" ::: "memory")
# 182 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
             ; // permettre les interruptions
  chk = MyDht.read11(DHT11_PIN);
  //count++; 
  
# 185 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
 __asm__ __volatile__ ("cli" ::: "memory")
# 185 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
               ; // empêcher les interruptions
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
# 199 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
void i2c_receiveEvent(int n) {
  // Traiter les commandes ou le numéro d'un registre (1 octet)
  if (n == 1) {
    // Un seul octet reçu. C'est probablement une commande.
    uint8_t data = Wire.read();
    switch (data) {
      case 0xA1:
        cmd = CMD::Stop;
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 207 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                      (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 207 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                      "commande 'Arrêter' reçue"
# 207 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                      ); &__c[0];}))
# 207 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                      )));
        break;
    case 0xA2:
        cmd = CMD::Go;
        cr.champs.nb_echantillons = 0;
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 212 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                      (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 212 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                      "Commande 'Démarrer' reçue"
# 212 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                      ); &__c[0];}))
# 212 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
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
# 229 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                    (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 229 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                    "Commande 'Changer Ts' reçue"
# 229 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                    ); &__c[0];}))
# 229 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                    )));
      cr.champs.Ts = data2;
      Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 231 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 231 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                  "La nouvelle valeur est: "
# 231 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                  ); &__c[0];}))
# 231 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                  )));
      Serial.print(cr.champs.Ts); Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 232 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                                                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 232 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                                " secondes"
# 232 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                                                ); &__c[0];}))
# 232 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                                                )));
    }
  }
  else {
    // Ignorer la réception n > 2 octets.
    Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 237 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                  (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 237 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
                  "Erreur: ce noeud n'accepte    pas de communication/commande à trois octets"
# 237 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino" 3
                  ); &__c[0];}))
# 237 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
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
# 252 "/home/pa/Documents/ETS/objetsConnectes/labo7/dht11-2/dht11-2.ino"
void i2c_requestEvent(){
  // Le coordonnateur veut la valeur d'un registre. L'adresse du
  // registre a été reçue précédemment.
  if ((adrReg >= 0) && (adrReg < NB_REGISTRES)){
    Serial.print("");
    // Envoyer le contenu du registre au coordonnateur
    Wire.write(cr.regs[adrReg]);
  }
}
