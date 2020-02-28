
/**
 * SwitchValvePair has refferals to one float switch and one solenoid valve
 */
class SwitchValvePair {
    bool error = false;
    int inPin;
    int outPin;
    int ID;
  public:
  
    SwitchValvePair() {}
    SwitchValvePair(int in, int out, int id){
      inPin = in;
      outPin = out;
      ID = id;
    }

    int getInPin() {
      return inPin;
    }

    int getOutPin() {
      return outPin;
    }
};

// Number of trays/switches
const int numOfSwitches = 12;
// Array of SVP
SwitchValvePair SVPArray[numOfSwitches];
// First switch at startPin
// Valves and switches should be every other pin
// starting at startPin
const int startPin = 23;

void setup() {
  // Float switch INPUT pins [23-45] ODD
  // Solenoid valse OUTPUT pins [24-46] EVEN
  int inPin;
  int outPin;
  int ID;
  // Write pinModes and add SVP objects to SVPArray
  for(int i = 0; i < numOfSwitches; i ++) {
    inPin = startPin + (2 * i);
    outPin = startPin + (2 * i) + 1; 
    ID = i + 1;
    SVPArray[i] = SwitchValvePair(inPin, outPin, ID);
    pinMode(inPin, INPUT);
    pinMode(outPin, OUTPUT);
    digitalWrite(outPin, HIGH);
  }
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < numOfSwitches; i++) {
    if(digitalRead(SVPArray[i].getInPin()) == HIGH) {
      digitalWrite(SVPArray[i].getOutPin(), LOW);
      while(digitalRead(SVPArray[i].getInPin()) == HIGH) {
        delay(10);
      }
      digitalWrite(SVPArray[i].getOutPin(), HIGH);
    }
  }
}
