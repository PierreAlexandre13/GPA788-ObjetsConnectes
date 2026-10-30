# 1 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
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
# 29 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
// Classe pour afficher des caractères à l'aide du LCD
# 31 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 2
// Classe pour mesurer la température interne de l'ATmega328P 
# 33 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 2

# 35 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 2



/* ---------------------------------------------------------------

   Créer un objet (global) de type LiquidCrystal.

   Le constructeur utilisé est:

     LiquidCrystal(rs, enable, d4, d5, d6, d7)

     rs (register select): broche 12

     enable: broche 11

     d4: broche 5, d5: broche 4, d6: broche 3, d7: broche 2

   ---------------------------------------------------------------    

   Note: Les numéros de broches sont ceux du côté d'Arduino.   

         Brancher ces broches sur l'afficheur LCD selon le

         diagramme électrique donné sur le protocole de laboratoire.

   --------------------------------------------------------------- */
# 50 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
/* ---------------------------------------------------------------

   Créer un objet (global) de type ChipTemp.

   Le constructeur utilisé est:

     ChipTemp(float g, float d)

     g: gain de l'ajustement, d: décalage de l'ajustement

     

   Normalement le gain et le décalage sont des valeurs obtenues

   par une procédure de calibration. Dans ce laboratoire, on utilisera

   des constantes proposées par Albert van Daln, l'auteur du code

   original de ChipTemp.

   --------------------------------------------------------------- */
# 62 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
const float DECALAGE{316.0}; //332.70; //335.2; // Choisir la bonne...
const float GAIN{1.22}; //1.06154;              // Choisir la bonne...
const uint32_t SERIAL_SPEED{115200};
//ChipTemp chipTemp(GAIN, DECALAGE);
dhtlib_gpa788 MyDht;

// Interruption à toutes les 3 secondes
const uint32_t TIMER_PERIOD{3000000};
const int DHT11_PIN{7};

// On doit utiliser le modificateur volatile ici
// car la variable chk est utilisée dans une fonction
// de gestion d'interruption.
volatile DHTLIB_ErrorCode chk;


/* ---------------------------------------------------------------

   Fonction d'initialisation obligatoire (exécutée 1 seule fois).

   --------------------------------------------------------------- */
# 81 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
void setup() {
  // Pour afficher la température sur le terminal série
  Serial.begin(SERIAL_SPEED);

  // Pour l'afficheur LCD
  // 0) Il s'agir d'un afficheur 16 x 2
  lcd.begin(16, 2);
  // 1) Effacer l'affichage
  lcd.clear();


  // L'objet Timer1 est automatiquement créé dans la bibliothèque TimerOne
  // Règle la période d'interruption
  Timer1.initialize(TIMER_PERIOD);
  // Règle la fonction de gestion d'interruption
  Timer1.attachInterrupt(readTemperature);
  // Démarrer le comptage
  Timer1.start();

  MyDht.Connect();

}

/* ---------------------------------------------------------------

   Fonction principale

   (Exécutée comme une fonction dans une boucle sans fin)

   --------------------------------------------------------------- */
# 108 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
const int16_t NB_MSG_COUNT{1};
int messageCount = 0;
void loop() {
  // Alterner entre le message de bienvenue et la température 
  // interne de l'ATmega328P.
  // messageCount: nombre de fois le contenu a été affiché
  // showingMessage: true -> bienvenue, false -> température
  //static int16_t messageCount{0};
  static bool showingMessage{true};

  // Si le compteur est à zéro alors change le contenu
  lcd.clear();
  showHumTemp(lcd);

  lcd.display();
  waitUntil(TIMER_PERIOD/1000);

}



void welcome(LiquidCrystal &l) {
  // D'abord le terminal série
  Serial.println((reinterpret_cast<const __FlashStringHelper *>(
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                "Bienvenue au GPA788 OC/IoT"
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                ); &__c[0];}))
# 131 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                )));
  // Ensuite l'afficheur LCD
  l.setCursor(0, 0); // Cursuer à la 1ere colonne, 1ere ligne
  l.print("Bienvenue au"); // Afficher le texte...
  l.setCursor(0, 1); // Curseur à la 1ere colonne, 2e ligne
  l.print("GPA788 OC/IoT"); // Afficher le texte...}
}



void showHumTemp(LiquidCrystal &l) {

  //chk = MyDht.read11(DHT11_PIN);

  if(chk == DHTLIB_ErrorCode::DHTLIB_OK){
    // Afficher des informations sur le moniteur série
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 147 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 147 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                "Humidite: "
# 147 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                ); &__c[0];}))
# 147 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                ))); Serial.println(MyDht.getHumidity());
    Serial.print((reinterpret_cast<const __FlashStringHelper *>(
# 148 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 148 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                "Temperature: "
# 148 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                ); &__c[0];}))
# 148 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                ))); Serial.println(MyDht.getTemperature());

    // Ensuite l'afficheur LCD
    l.setCursor(0,0); l.print("Humidite: "); l.print(MyDht.getHumidity(), 2); l.print("%");
    l.setCursor(0,1); l.print("Temp: "); l.print(MyDht.getTemperature(), 2); l.print((char)223); l.print("C");
    }
    else{
        l.setCursor(0,0); l.print("Erreur DHT11");
        l.setCursor(0,1); l.print("Code: "); l.print(static_cast<int>(chk));
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
# 179 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 179 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                "Interruption"
# 179 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
                ); &__c[0];}))
# 179 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
                )));

  
# 181 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
 __asm__ __volatile__ ("sei" ::: "memory")
# 181 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
             ; // permettre les interruptions
  chk = MyDht.read11(DHT11_PIN);
  //count++; 
  messageCount++;
  
# 185 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino" 3
 __asm__ __volatile__ ("cli" ::: "memory")
# 185 "/home/pa/Documents/ETS/objetsConnectes/labo3/test_lcd/test_lcd.ino"
               ; // empêcher les interruptions
}
