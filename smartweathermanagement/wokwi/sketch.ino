// ============================================================================
// PROJET SMART WEATHER MANAGEMENT - MODULE 1 : GESTION DES EMPLOYÉS
// SEMAINE 10 : SCÉNARIO ARDUINO - STATION MÉTÉO INTELLIGENTE
// ÉTUDIANTE : SALMA RIDENE
//
// SCÉNARIO : Une station météo autonome (Arduino Uno) équipée d'un capteur
//            DHT22 (température + humidité) et d'un capteur de pluie (simulé
//            par un potentiomètre) envoie des relevés toutes les 3 secondes
//            via la liaison série à l'application Qt.
//            L'application Qt :
//              1) Reçoit et parse les relevés
//              2) Les enregistre dans la table RELEVE_METEO (Oracle/SQLite)
//              3) Si un seuil est franchi (T>35°C, H<20%, pluie>700/1023),
//                 crée une alerte dans la table ALERTE et envoie "ALERT_ON"
//              4) L'Arduino active le buzzer + LED rouge et affiche "ALERTE!"
//                 sur l'écran LCD
//
// COMPOSANTS (réutilisables) :
//   - Arduino Uno
//   - DHT22 (capteur temp + humidité)
//   - Potentiomètre (simule capteur pluie)
//   - LED verte (état normal)
//   - LED rouge (alerte)
//   - Buzzer actif
//   - LCD 1602 I2C (16x2 caractères)
//
// FORMAT SÉRIE (9600 baud) :
//   Envoi vers Qt  : "T:28.50,H:65.30,R:120,GEL:0\n"
//   Reçu de Qt     : "ALERT_ON\n" | "ALERT_OFF\n" | "STATUS\n"
// ============================================================================

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Broches ────────────────────────────────────────────────────────────────
#define DHTPIN      2       // DHT22 → pin 2
#define DHTTYPE     DHT22
#define PIN_PLUIE   A0      // Potentiomètre pluie → A0
#define PIN_LED_V   8       // LED verte
#define PIN_LED_R   9       // LED rouge
#define PIN_BUZZER  10      // Buzzer actif

// ── Objets ─────────────────────────────────────────────────────────────────
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // adresse I2C 0x27, 16 colonnes, 2 lignes

// ── Variables globales ─────────────────────────────────────────────────────
bool   alerteActive  = false;
bool   gelDetecte    = false;
unsigned long dernierEnvoi = 0;
const unsigned long INTERVALLE_MS = 3000;   // envoi toutes les 3 s

String serialBuffer = "";   // tampon pour commandes série

// ── Caractère personnalisé : goutte de pluie ───────────────────────────────
byte charPluie[8] = {
  B00100, B00100, B01110, B11111,
  B11111, B01110, B00100, B00000
};

// ===========================================================================
void setup() {
  Serial.begin(9600);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, charPluie);

  pinMode(PIN_LED_V,   OUTPUT);
  pinMode(PIN_LED_R,   OUTPUT);
  pinMode(PIN_BUZZER,  OUTPUT);

  // État initial : normal
  digitalWrite(PIN_LED_V,  HIGH);
  digitalWrite(PIN_LED_R,  LOW);
  digitalWrite(PIN_BUZZER, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Station Meteo INM");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation...");
  delay(2000);
  lcd.clear();

  Serial.println("READY:StationMeteoINM_v1.0");
}

// ===========================================================================
void loop() {
  // ── 1. Lire les commandes serie entrantes ─────────────────────────────────
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      serialBuffer.trim();
      traiterCommande(serialBuffer);
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }

  // ── 2. Envoi périodique des relevés ───────────────────────────────────────
  unsigned long maintenant = millis();
  if (maintenant - dernierEnvoi >= INTERVALLE_MS) {
    dernierEnvoi = maintenant;
    envoyerReleve();
  }
}

// ===========================================================================
// Lire les capteurs et envoyer un relevé formaté via Serial
// ===========================================================================
void envoyerReleve() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  int   pluie = analogRead(PIN_PLUIE);   // 0–1023

  // DHT22 peut parfois renvoyer NaN (en simulation Wokwi c'est fiable)
  if (isnan(temp)) temp = 0.0;
  if (isnan(hum))  hum  = 0.0;

  // Détection gel : T < 3°C ET H > 85%
  gelDetecte = (temp < 3.0 && hum > 85.0);

  // Afficher sur LCD
  afficherLCD(temp, hum, pluie);

  // Envoyer vers Qt (format parseable)
  Serial.print("T:");   Serial.print(temp,  2);
  Serial.print(",H:");  Serial.print(hum,   2);
  Serial.print(",R:");  Serial.print(pluie);
  Serial.print(",GEL:"); Serial.println(gelDetecte ? 1 : 0);
}

// ===========================================================================
// Afficher temp, humidité, pluie sur l'écran LCD 16x2
// ===========================================================================
void afficherLCD(float temp, float hum, int pluie) {
  lcd.clear();

  if (alerteActive) {
    lcd.setCursor(0, 0);
    lcd.print("!! ALERTE INM !!");
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print("C H:");
    lcd.print((int)hum);
    lcd.print("%");
  } else {
    // Ligne 0 : température + humidité
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print("C  H:");
    lcd.print((int)hum);
    lcd.print("%");

    // Ligne 1 : pluie (barre proportionnelle)
    lcd.setCursor(0, 1);
    lcd.write(0);                    // caractère goutte
    lcd.print(" Pluie:");
    int barres = map(pluie, 0, 1023, 0, 7);
    for (int i = 0; i < barres; i++) lcd.print("|");
    for (int i = barres; i < 7; i++) lcd.print(".");

    if (gelDetecte) {
      lcd.setCursor(14, 1);
      lcd.print("GEL");
    }
  }
}

// ===========================================================================
// Traiter une commande reçue depuis Qt
// ===========================================================================
void traiterCommande(const String &cmd) {
  if (cmd == "ALERT_ON") {
    alerteActive = true;
    digitalWrite(PIN_LED_V,  LOW);
    digitalWrite(PIN_LED_R,  HIGH);
    // Buzzer bip courts répétés (non-bloquant : on fait 3 bips rapides)
    for (int i = 0; i < 3; i++) {
      digitalWrite(PIN_BUZZER, HIGH);
      delay(150);
      digitalWrite(PIN_BUZZER, LOW);
      delay(100);
    }
    Serial.println("ACK:ALERT_ON");

  } else if (cmd == "ALERT_OFF") {
    alerteActive = false;
    digitalWrite(PIN_LED_R,  LOW);
    digitalWrite(PIN_LED_V,  HIGH);
    digitalWrite(PIN_BUZZER, LOW);
    Serial.println("ACK:ALERT_OFF");

  } else if (cmd == "STATUS") {
    // Répondre avec l'état courant
    Serial.print("STATUS:alert=");
    Serial.print(alerteActive ? "1" : "0");
    Serial.print(",gel=");
    Serial.println(gelDetecte ? "1" : "0");

  } else if (cmd.startsWith("SEUIL_T:")) {
    // Optionnel : changer le seuil température à la volée
    // ex: "SEUIL_T:38.0"
    float v = cmd.substring(8).toFloat();
    Serial.print("ACK:SEUIL_T=");
    Serial.println(v);
  }
}

