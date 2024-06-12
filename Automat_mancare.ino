#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <Keypad.h>

// Pinii senzorului ultrasonic
int TRIGPIN = 12;
int ECHOPIN = 13;
int LED = A3;
long DURATA, DISTANTA;
int LIMITA = 18;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo Servo1;

ThreeWire myWire(A1, A2, A0); // DAT, CLK, RST - pinii lor
RtcDS1302<ThreeWire> Rtc(myWire);

const int servoPin = 3;
const int butonPin = 2;
int stareButon = HIGH;
bool rotireServo1 = false; // Variabilă pentru a reține dacă servomotorul s-a rotit prima dată astăzi
bool rotireServo2 = false; // Variabilă pentru a reține dacă servomotorul s-a rotit a doua oară astăzi

int ultimaZi = -1;

const byte linii = 4;
const byte coloane = 4;
char keymap[linii][coloane] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'F', '0', 'E', 'D'}
};
byte piniLinii[linii] = {11,10,9,8};
byte piniColoane[coloane] = {7,6,5,4};
Keypad customKeypad = Keypad(makeKeymap(keymap), piniLinii, piniColoane, linii, coloane);

// Orele implicite de acțiune ale servomotorului
int defaultHour1 = 13;
int defaultMinute1 = 20;
int defaultHour2 = 20;
int defaultMinute2 = 30;

// Orele actualizate de acțiune ale servomotorului
int updatedHour1 = defaultHour1;
int updatedMinute1 = defaultMinute1;
int updatedHour2 = defaultHour2;
int updatedMinute2 = defaultMinute2;

// Array pentru a defini emoji zâmbitor
byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

int totalRotiri = 0; // Numără totalul rotațiilor servomotorului într-o zi

void setup() {
  Serial.begin(9600);

  // Configurarea senzorului ultrasonic
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(LED, OUTPUT);
  
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Crează un caracter personalizat (emoji zâmbitor)
  lcd.createChar(0, smiley);
  
  Servo1.attach(servoPin);
  pinMode(butonPin, INPUT_PULLUP);
  Servo1.write(0);

  Rtc.Begin();

  RtcDateTime currentTime = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(currentTime);
}

void loop() {
  // Cod pentru senzorul ultrasonic
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  DURATA = pulseIn(ECHOPIN, HIGH);
  DISTANTA = 0.017 * DURATA;
  if (DISTANTA <= LIMITA) {
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }
  Serial.print("Distanța în cm este: ");
  Serial.println(DISTANTA);
  
  RtcDateTime now = Rtc.GetDateTime();

  // Resetare zilnică
  if (now.Day() != ultimaZi) {
    updatedHour1 = defaultHour1;
    updatedMinute1 = defaultMinute1;
    updatedHour2 = defaultHour2;
    updatedMinute2 = defaultMinute2;
    rotireServo1 = false;
    rotireServo2 = false;
    ultimaZi = now.Day();
    totalRotiri = 0; // Resetează numărul total de rotații

  }

  // Verifică prima oră programată
  if (now.Hour() == updatedHour1 && now.Minute() == updatedMinute1 && !rotireServo1) {
    moveServo();
    rotireServo1 = true;
    totalRotiri++;
  }

  // Verifică a doua oră programată
  if (now.Hour() == updatedHour2 && now.Minute() == updatedMinute2 && !rotireServo2) {
    moveServo();
    rotireServo2 = true;
    totalRotiri++;
  }

  // Verifică apăsarea butonului
  int stareCurentaButon = digitalRead(butonPin);
  if (stareCurentaButon == LOW && stareButon == HIGH) {
    if (totalRotiri < 2) {
      moveServo();
      updateSchedule(now); // Actualizăm programările cu ora curentă
    } else {
      lcd.clear();
      lcd.print("Tom a mancat!");
      lcd.setCursor(0, 1);
      lcd.print("Revino maine!");
      delay(3000);
      displayDateTime(now);
    }
  }
  stareButon = stareCurentaButon;

  // Afișează data și ora
  displayDateTime(now);

  char key = customKeypad.getKey();
  if (key == 'D') {
    if (totalRotiri >= 2) {
      lcd.clear();
      lcd.print("Tom a mancat ");
      lcd.write(byte(0)); // Scrie emoji zâmbitor
      lcd.setCursor(0, 1);
      lcd.print("Revino maine!");
      delay(3000); // Afișează mesajul pentru 3 secunde
      displayDateTime(now); // Reafisează data și ora
    } else if (totalRotiri == 1) {
      // Servomotorul s-a rotit o dată, permite modificarea doar a Ora2
      lcd.clear();
      setScheduledTime(2);
      lcd.clear();
      displayDateTime(now);
    } else {
      setScheduledTime(1); // Setează prima oră programată
      setScheduledTime(2); // Setează a doua oră programată
      lcd.clear();
      displayDateTime(now); // Reafisează data și ora
    }
  } else if (key == 'C') {
    showScheduledFeedingTimes();
    delay(3000); // Așteaptă 3 secunde (3000 milisecunde) înainte de a reveni la afișarea timpului curent
    displayDateTime(now); // Reafisează data și ora
  } else if (key == 'A') {
    // Verifică dacă servomotorul s-a rotit deja o dată
    if (!rotireServo1) {
      // Dacă nu s-a rotit, permite modificarea Ora1
      setScheduledTime(1);
    } else {
      // Dacă s-a rotit, afișează un mesaj și nu permite modificarea
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nu se mai poate!");
      delay(3000); // Afișează mesajul pentru 3 secunde
    }
    lcd.clear();
    displayDateTime(now); // Reafisează data și ora
  } else if (key == 'B') {
    // Verifică dacă servomotorul s-a rotit deja de 2 ori
    if (totalRotiri < 2) {
      // Dacă nu s-a rotit de 2 ori, permite modificarea Ora2
      setScheduledTime(2);
    } else {
      // Dacă s-a rotit de 2 ori, afișează un mesaj și nu permite modificarea
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nu se mai poate!");
      delay(3000); // Afișează mesajul pentru 3 secunde
    }
    lcd.clear();
    displayDateTime(now); // Reafisează data și ora
  }

  delay(500);
}

void updateSchedule(RtcDateTime currentTime) {
  // Verificăm dacă servomotorul s-a rotit deja o dată sau deloc
  if (!rotireServo1) {
    updatedHour1 = currentTime.Hour();
    updatedMinute1 = currentTime.Minute();
    rotireServo1 = true; // Marcăm că servomotorul s-a rotit prima dată
    totalRotiri++;
  } else if (!rotireServo2) {
    updatedHour2 = currentTime.Hour();
    updatedMinute2 = currentTime.Minute();
    rotireServo2 = true; // Marcăm că servomotorul s-a rotit a doua oară
    totalRotiri++;
  }
}

void moveServo() {
  int angle = 180; // Setează unghiul dorit pentru servomotor
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Se misca la ora:");
  lcd.setCursor(0, 1);
  printTime(Rtc.GetDateTime().Hour());
  lcd.print(":");
  printTime(Rtc.GetDateTime().Minute());
  lcd.print(":");
  printTime(Rtc.GetDateTime().Second());
  Serial.println("Moving at scheduled time.");

  Servo1.write(angle); // Rotește servomotorul la unghiul calculat

  delay(1000); // Așteaptă 1 secundă
  Servo1.write(0); // Revine la poziția inițială a servomotorului
  delay(1000); // Așteaptă o secundă suplimentară
}

void printTime(int timeValue) {
  if (timeValue < 10) {
    lcd.print("0");
  }
  lcd.print(timeValue);
}

void setScheduledTime(int scheduleNumber) {
  int hour = 0;
  int minute = 0;
  int numarCifre = 0;
  bool oraCompleta = false;
  RtcDateTime now = Rtc.GetDateTime(); // Obține ora curentă

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(scheduleNumber == 1 ? "Introdu ora1:" : "Introdu ora2:");
  lcd.setCursor(0, 1);

  while (true) {
    char key = customKeypad.getKey();
    if (key != NO_KEY && key >= '0' && key <= '9') {
      int value = key - '0';

      // Logica pentru validarea orei și minutului
      if (numarCifre < 2) { // Construim ora
        if ((numarCifre == 0 && value > 2) || (numarCifre == 1 && hour == 2 && value > 3)) {
          continue; // Sărim peste valorile care nu sunt valide pentru ore
        }
        hour = hour * 10 + value;
      } else if (numarCifre == 2 || numarCifre == 3) { // Construim minutul
        if (numarCifre == 2 && value > 5) {
          continue; // Sărim peste valorile care nu sunt valide pentru primul digit al minutelor
        }
        minute = minute * 10 + value;
      }

      // Afișează cifra introdusă
      lcd.print(key);
      
      // Incrementăm numărul de cifre introduse
      numarCifre++;

      if (numarCifre == 2 && !oraCompleta) {
        lcd.setCursor(2, 1); // Muta cursorul după ora
        lcd.print(":");
        oraCompleta = true;
      }

      if (numarCifre == 4) {
        // Verificăm validitatea orei și minutului introduse
        if (scheduleNumber == 1 && (hour > 23 || minute > 59 || hour < now.Hour() || (hour == now.Hour() && minute < now.Minute()))) {
          lcd.clear();
          lcd.print("Ora invalida!");
          delay(2000);
          returnToInput(&hour, &minute, &numarCifre, &oraCompleta, "Introdu ora1:");
        } else if (scheduleNumber == 2 && (hour > 23 || minute > 59 || hour < updatedHour1 || (hour == updatedHour1 && minute <= updatedMinute1))) {
          lcd.clear();
          lcd.print("Ora invalida!");
          delay(2000);
          returnToInput(&hour, &minute, &numarCifre, &oraCompleta, "Introdu ora2:");
        } else {
          break; // Ora și minutul sunt valide
        }
      }
    }
  }

  // Actualizăm ora programată
  if (scheduleNumber == 1) {
    updatedHour1 = hour;
    updatedMinute1 = minute;
  } else {
    updatedHour2 = hour;
    updatedMinute2 = minute;
  }
}

void returnToInput(int *hour, int *minute, int *numarCifre, bool *oraCompleta, const char* msg) {
  *hour = 0;
  *minute = 0;
  *numarCifre = 0;
  *oraCompleta = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  lcd.setCursor(0, 1);
}

void showScheduledFeedingTimes() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ora1:");
  printTime(updatedHour1);
  lcd.print(":");
  printTime(updatedMinute1);

  lcd.setCursor(0, 1);
  lcd.print("Ora2:");
  printTime(updatedHour2);
  lcd.print(":");
  printTime(updatedMinute2);
}

void displayDateTime(RtcDateTime dateTime) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data:");
  lcd.print(dateTime.Day());
  lcd.print("/");
  lcd.print(dateTime.Month());
  lcd.print("/");
  lcd.print(dateTime.Year());
  lcd.setCursor(0, 1);
  lcd.print("Ora:");
  printTime(dateTime.Hour());
  lcd.print(":");
  printTime(dateTime.Minute());
  lcd.print(":");
  printTime(dateTime.Second());
}
