// Motor pinleri
const int sol_enable = 11;  // ENABLE A       
const int sag_ileri = 13;   // INPUT 1
const int sag_geri = 12;    // INPUT 2
const int sol_ileri = 10;   // INPUT 3
const int sol_geri = 8;     // INPUT 4
const int sag_enable = 9;   // ENABLE B

// Ultrasonik sensör pinleri
const int trigPin1 = 3; // Sol sensör trig
const int echoPin1 = 2; // Sol sensör echo
const int trigPin2 = 5; // Ön sensör trig
const int echoPin2 = 4; // Ön sensör echo
const int trigPin3 = 6; // Sağ sensör trig
const int echoPin3 = 7; // Sağ sensör echo

const int ldrPin = A0; // LDR sensörü bağlı olduğu pin

long sure1, solaUzaklik, sure2, ileriyeUzaklik, sure3, sagaUzaklik;
int ldrValue;
const int ldrThreshold = 500; // Tünel için ışık eşik değeri (bu değeri ortam ışığına göre ayarlayın)
bool hasWaitedInTunnel = false; // Tünelde bekleme yapıldı mı?

// Motor hız ayarları
int forwardSpeed = 80; // İleri gitme hızı (0-255 arasında)
int backwardSpeed = 80; // Geri gitme hızı (0-255 arasında)
int turnRightSpeed = 80; // Sağa dönüş hızı (0-255 arasında)
int turnLeftSpeed = 80; // Sola dönüş hızı (0-255 arasında)
int backupSpeed = 80; // Geri gitme hızı (0-255 arasında)
int backupTime = 400; // Geri gitme süresi (milisaniye)

void setup() {
  // Sensör pinlerini ayarla
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
  pinMode(trigPin3, OUTPUT); 
  pinMode(echoPin3, INPUT); 
  
  // Motor pinlerini ayarla
  pinMode(sol_enable, OUTPUT);
  pinMode(sag_enable, OUTPUT);
  pinMode(sag_ileri, OUTPUT);
  pinMode(sag_geri, OUTPUT);
  pinMode(sol_ileri, OUTPUT);
  pinMode(sol_geri, OUTPUT);

    solaUzaklik = measureDistance(trigPin1, echoPin1);
    ileriyeUzaklik = measureDistance(trigPin2, echoPin2);
    sagaUzaklik = measureDistance(trigPin3, echoPin3);
    ldrValue = analogRead(ldrPin);
  // Seri haberleşmeyi başlat
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    Serial.println(command);
    if (command == 'F') {
    // Mesafeleri ölç
    solaUzaklik = measureDistance(trigPin1, echoPin1);
    ileriyeUzaklik = measureDistance(trigPin2, echoPin2);
    sagaUzaklik = measureDistance(trigPin3, echoPin3);
    ldrValue = analogRead(ldrPin);

    if (ldrValue < ldrThreshold && !hasWaitedInTunnel) {
        delay(1000);
        stopMotors();
        delay(5000); // 5 saniye bekle
        hasWaitedInTunnel = true; // Bekleme işlemi yapıldı
    } else {
        // Hareket kararları
        if (ileriyeUzaklik < 25) {
            if (sagaUzaklik > solaUzaklik ) {
                // Sağda boşluk varsa sağa dön
                backUp();
                turnRight();
            } else if (solaUzaklik > sagaUzaklik) {
                // Solda boşluk varsa sola dön
                backUp();
                turnLeft();
            } else if (solaUzaklik > 90 && sagaUzaklik > 90) {
                stopMotors();
            } else {
                // Ön, sağ ve sol doluysa geri dön
                moveBackward();
            }
        } else {
            // Ön boşsa ileri git
            moveForward();
        }
    }
  }
  else if (command == 'B'){
    stopMotors();
  }
 }
}
void moveForward() {
  digitalWrite(sag_ileri, HIGH);
  digitalWrite(sag_geri, LOW);
  digitalWrite(sol_ileri, HIGH);
  digitalWrite(sol_geri, LOW);
  analogWrite(sag_enable, forwardSpeed); // motor hızları
  analogWrite(sol_enable, forwardSpeed);
}

void moveBackward() {
  digitalWrite(sag_ileri, LOW);
  digitalWrite(sag_geri, HIGH);
  digitalWrite(sol_ileri, LOW);
  digitalWrite(sol_geri, HIGH);
  analogWrite(sag_enable, backwardSpeed); // motor hızları
  analogWrite(sol_enable, backwardSpeed);
}

void backUp() {
  moveBackward();
  delay(backupTime); // kısa süreli geri gitme
  stopMotors();
}

void turnRight() {
  digitalWrite(sag_ileri, LOW);
  digitalWrite(sag_geri, HIGH);
  digitalWrite(sol_ileri, HIGH);
  digitalWrite(sol_geri, LOW);
  analogWrite(sag_enable, turnRightSpeed); // motor hızları
  analogWrite(sol_enable, turnRightSpeed);
  delay(400); // sağa dönüş süresi
  stopMotors();
}

void turnLeft() {
  digitalWrite(sag_ileri, HIGH);
  digitalWrite(sag_geri, LOW);
  digitalWrite(sol_ileri, LOW);
  digitalWrite(sol_geri, HIGH);
  analogWrite(sag_enable, turnLeftSpeed); // motor hızları
  analogWrite(sol_enable, turnLeftSpeed);
  delay(400); // sola dönüş süresi
  stopMotors();
}

void stopMotors() {
  digitalWrite(sag_ileri, LOW);
  digitalWrite(sag_geri, LOW);
  digitalWrite(sol_ileri, LOW);
  digitalWrite(sol_geri, LOW);
  analogWrite(sag_enable, 0);
  analogWrite(sol_enable, 0);
}

float measureDistance(int trigPin, int echoPin) {
  // Ultrasonik sensör mesafe ölçümü
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // cm'ye çevir

  return distance;
}