///////////////////////////////////////////////////////////////////
////////////////////////////Pin Numbers////////////////////////////
///////////////////////////////////////////////////////////////////

//Ultrasonic 1
#define Trig_1 2
#define Echo_1 3

//Ultrasonic 2
#define Trig_2 10
#define Echo_2 11

//Ultrasonic 3
#define Trig_3 12
#define Echo_3 13

//Front & Rear Motors
#define Front_enA 4
#define Rear_enB 5
#define Front_in1 22
#define Front_in2 23
#define Rear_in3 24
#define Rear_in4 25

//Left & Right Motors
#define Left_enA 6
#define Right_enB 7
#define Left_in1 26
#define Left_in2 27
#define Right_in3 28
#define Right_in4 29

//Raising Motors
#define Raise_enA 8
#define Raise_enB 9
#define Raise_in1 30
#define Raise_in2 31
#define Raise_in3 32
#define Raise_in4 33

#define OFFSET 0      //Offset for the Difference Between Ultrasonic Sensor 1 and Ultrasonic Sensor 2

///////////////////////////////////////////////////////////////////
/////////////////////////Global Variables//////////////////////////
///////////////////////////////////////////////////////////////////

const int Reading_No = 20;
double Readings_US1[Reading_No];
double Readings_US2[Reading_No];
int IndexRead = 0;
double Total_US1 = 0;
double Total_US2 = 0;

double US1, US2, Diff;
double Pos = 0;
double Error = 0;
double Power_Left, Power_Right, Power_Rear;

///////////////////////////////////////////////////////////////////
////////////////////////////Functions//////////////////////////////
///////////////////////////////////////////////////////////////////

void Initialise_Array() {
  for (int ThisReading = 0; ThisReading < Reading_No; ThisReading++) {
    Readings_US1[ThisReading] = 0;
    Readings_US2[ThisReading] = 0;
  }
}

void Smooth() {
  Total_US1 = Total_US1 - Readings_US1[IndexRead];
  Readings_US1[IndexRead] = US1;
  Total_US1 = Total_US1 + Readings_US1[IndexRead];

  Total_US2 = Total_US2 - Readings_US2[IndexRead];
  Readings_US2[IndexRead] = US2;
  Total_US2 = Total_US2 + Readings_US2[IndexRead];
  
  IndexRead = IndexRead + 1;

  if (IndexRead >= Reading_No) {
    IndexRead = 0;
  }

  US1 = Total_US1 / Reading_No;
  US2 = Total_US2 / Reading_No;
}

void Ultrasonic(int Trig, int Echo) {
  double duration;
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  duration = pulseIn(Echo, HIGH);
  Pos = (duration/2) / 29.1;
}

void Distance() {
  Ultrasonic(Trig_1, Echo_1);
  US1 = Pos;
  Ultrasonic(Trig_2, Echo_2);
  US2 = Pos;

  Smooth();
  
  Diff = US1 - US2;
  Pos = (US1 + US2) / 2.0;
}

double Power(int Input_1, int Input_2, double Power) {
  if (Power >= 0) {
    digitalWrite(Input_1, LOW);
    digitalWrite(Input_2, HIGH);
  }
  else {
    digitalWrite(Input_1, HIGH);
    digitalWrite(Input_2, LOW);
  }
  Power = abs(Power);
  return Power;
}

void Drive(double Kp, double Target) {
  do {
    //Drives the robot a desired distance in a straight line
    Distance();
    Diagnostics();
    Error = Kp * (Pos - Target);
    Error = constrain(Error, -255, 225);
    Diff = constrain(Diff, -25, 25);
    Power_Left = Power(Left_in1, Left_in2, Error);
    Power_Right = Power(Right_in3, Right_in4, Error);
    Power_Rear = Power(Rear_in3, Rear_in4, -100 * Diff);
    analogWrite(Left_enA, 0*Power_Left);
    analogWrite(Right_enB, 0*Power_Right);
    analogWrite(Rear_enB, Power_Rear);
  } while(1);
  analogWrite(Left_enA, 0);
  analogWrite(Right_enB, 0);
}

void Raise() {
  analogWrite(Raise_enA, 255);
  analogWrite(Raise_enB, 255);  
  delay(500);
  analogWrite(Raise_enA, 0);
  analogWrite(Raise_enB, 0);
}

void Diagnostics() {
  Serial.print(US1);
  Serial.print(" ");
  Serial.print(US2);
  Serial.print(" ");
  Serial.print(Diff);
  Serial.print(" ");
  Serial.println(Pos);
}

///////////////////////////////////////////////////////////////////
///////////////////////////Main Program////////////////////////////
///////////////////////////////////////////////////////////////////

void setup() {
  //Serial Port Begin
  Serial.begin (9600);
  
  //Define I/O
  pinMode(Trig_1, OUTPUT);
  pinMode(Echo_1, INPUT);
  pinMode(Trig_2, OUTPUT);
  pinMode(Echo_2, INPUT);
  pinMode(Trig_3, OUTPUT);
  pinMode(Echo_3, INPUT);

  //Set Motor Control Pins to Output
  pinMode(Front_enA, OUTPUT);
  pinMode(Front_in1, OUTPUT);
  pinMode(Front_in2, OUTPUT);
  pinMode(Rear_enB, OUTPUT);
  pinMode(Rear_in3, OUTPUT);
  pinMode(Rear_in4, OUTPUT);
  pinMode(Left_enA, OUTPUT);
  pinMode(Left_in1, OUTPUT);
  pinMode(Left_in2, OUTPUT);
  pinMode(Right_enB, OUTPUT);
  pinMode(Right_in3, OUTPUT);
  pinMode(Right_in4, OUTPUT);
  pinMode(Raise_enA, OUTPUT);
  pinMode(Raise_in1, OUTPUT);
  pinMode(Raise_in2, OUTPUT);
  pinMode(Raise_enB, OUTPUT);
  pinMode(Raise_in3, OUTPUT);
  pinMode(Raise_in4, OUTPUT);

  //Set Motor Direction
  digitalWrite(Front_in1, LOW);
  digitalWrite(Front_in2, HIGH);
  digitalWrite(Rear_in3, LOW);
  digitalWrite(Rear_in4, HIGH);
  digitalWrite(Left_in1, LOW);
  digitalWrite(Left_in2, HIGH);
  digitalWrite(Right_in3, LOW);
  digitalWrite(Right_in4, HIGH);
  digitalWrite(Raise_in1, LOW);
  digitalWrite(Raise_in2, HIGH);
  digitalWrite(Raise_in3, LOW);
  digitalWrite(Raise_in4, HIGH);

  Initialise_Array();

  Raise();
  
  //Drive to Collection
  Drive(100, 30);
}

void loop() {
  //##DO NOT USE##
}
