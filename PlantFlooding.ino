// 1 for normal, 0 for test
int const MODE = 0;

/**
 * SwitchValvePair has refferals to one float switch and one solenoid valve
 */
class SwitchValvePair {
    // Make false if error or disabled by user
    bool enabled = true;
    bool error = false;
    String errorName = "none";
    int timeMilSinceLastFill = 0;
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

    int getID() {
      return ID;
    }

    // If this SVP has error return true
    bool hasError() {
      return error;
    }

    // If this SVP is enabled return true
    bool isEnabled() {
      return enabled;
    }

    // Create a new error for this SVP, set error's name
    int createError(String errName) {
      errorName = errName;
      error = true;
    }

    // Disable switch valve pair
    void disable() {
      enabled = false;
    }

    // retunrs error's name
    String getError() {
      return errorName;
    }

    // Increment amount time since last refill by timeMil
    void incTimeMilSinceLastFill(int timeMil) {
      timeMilSinceLastFill += timeMil;
    }

    // Refresh timeMilSinceLastFill back to zero
    void refTimeMilSinceLastFill() {
      timeMilSinceLastFill = 0;
    }
};

// Number of trays/switches
const int numOfSwitches = 4;
// Array of SVP
SwitchValvePair SVPArray[numOfSwitches];
// First switch at startPin
// Valves and switches should be every other pin
// starting at startPin
const int startPin = 23;

void setup() {
  if(MODE == 0) {
    testSetup();
  }
  else {
    // Float switch INPUT pins [23-45] ODD
    // Solenoid valse OUTPUT pins [24-46] EVEN
    int inPin;
    int outPin;
    int ID;
    // Write pinModes and add SVP objects to SVPArray
    for(int i = 0; i < numOfSwitches; i++) {
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
}

// Variable used to keep track of how long a switch has been HIGH
// Milliseconds
int amountTimeHighMil = 0;
// amount of milliseconds a switch has been HIGH to throw an error
int const amountTimeHighForErrorMil = 2000;
// Amount of milliseconds to delay between loops
int const delayTimeMil = 10;
// Total millisec of delay since last update of TimeMilSinceLastFill
int totalDelayTimeMil = 0;
// If timeMilSinceLastFill for SVP goes over threshold, flag as an error
int const thesholdTimeMilSinceLastFill = 1000;
void loop() {
  if(MODE == 0) {
    testLoop();
  }
  else{
    dlay(delayTimeMil);
    // Iterate through SVParray
    // If switch is HIGH make correspoiding valve LOW while switch is HIGH
    for(int i = 0; i < numOfSwitches; i++) {
      if(SVPArray[i].isEnabled() && digitalRead(SVPArray[i].getInPin()) == HIGH) {
        digitalWrite(SVPArray[i].getOutPin(), LOW);
        // While switch is high fill tray
        // If amountTimeHighMil becomes too large stop fill
        while(amountTimeHighMil <= amountTimeHighForErrorMil
              && digitalRead(SVPArray[i].getInPin()) == HIGH) {
          amountTimeHighMil += delayTimeMil;
          dlay(delayTimeMil);
        }
        // check if switch was fired for too long, if it was create an error
        // and disable the switch
        if(amountTimeHighMil > amountTimeHighForErrorMil) {
          SVPArray[i].createError("switchFiredForTooLong");
          SVPArray[i].disable();
        }
        // set amountTimeHighMil back to 0
        amountTimeHighMil = 0;
        // Write HIGH to valve
        digitalWrite(SVPArray[i].getOutPin(), HIGH);
      }
    }
  }
}

//void updateTimeMilSinceLastFill() {
//  for(int i = 0; i < numOfSwitches; i++) {
//    
//  }
//}

// Delay for timeMil
// Increment numDelays
void dlay(int timeMil) {
  delay(timeMil);
  totalDelayTimeMil += timeMil;
}


////////////////////////////////// FOR TESTING ///////////////////////////////////////////
void testSetup() {
  // Float switch INPUT pins [23-45] ODD
  // Solenoid valse OUTPUT pins [24-46] EVEN
  int inPin;
  int outPin;
  int ID;
  // Write pinModes and add SVP objects to SVPArray
  for(int i = 0; i < numOfSwitches; i++) {
    inPin = startPin + (2 * i);
    outPin = startPin + (2 * i) + 1; 
    ID = i + 1;
    SVPArray[i] = SwitchValvePair(inPin, outPin, ID);
  }
  Serial.begin(9600);
  
}
void testLoop() {
  dlay(delayTimeMil);
  // Iterate through SVParray
  // If switch is HIGH make correspoiding valve LOW while switch is HIGH
  for(int i = 0; i < numOfSwitches; i++) {
    if(SVPArray[i].isEnabled()) {
      //Serial.println("fired");
      Serial.println(SVPArray[i].getID());
      // While switch is high fill tray
      Serial.println("Filling");
      while(amountTimeHighMil <= amountTimeHighForErrorMil) {
        amountTimeHighMil += delayTimeMil;
        dlay(delayTimeMil);
      }
      Serial.println("Done");
      // check if switch was fired for too long, if it was create an error
      if(amountTimeHighMil > amountTimeHighForErrorMil) {
        //Serial.println("Throwing error");
        SVPArray[i].createError("switchFiredForTooLong");
        SVPArray[i].disable();
        Serial.println("Creating error");
      }
      // set amountTimeHighMil back to 0
      amountTimeHighMil = 0;
      digitalWrite(SVPArray[i].getOutPin(), HIGH);
      Serial.print("Delay Time: ");
      Serial.println(totalDelayTimeMil);
    } else {
      Serial.print("ERROR: ");
      Serial.print(SVPArray[i].getID());
      Serial.print(" ");
      Serial.println(SVPArray[i].getError());
      dlay(1000);
      Serial.print("Delay Time: ");
      Serial.println(totalDelayTimeMil);
    }
  }
}
