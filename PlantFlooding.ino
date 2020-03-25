// 1 for normal, 0 for test
int const MODE = 1;

/**
 * SwitchValvePair has refferals to one float switch and one solenoid valve
 */
class SwitchValvePair {
    // Make false if error or disabled by user
    bool enabled = true;
    bool error = false;
    String errorName = "none";
    int TimeMinSinceLastFill = 0;
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

    int getTimeMinSinceLastFill() {
      return TimeMinSinceLastFill;
    }

    // Increment amount time since last refill by timeMil
    void incTimeMinSinceLastFill(int timeMin) {
      TimeMinSinceLastFill += timeMin;
    }

    // Refresh TimeMinSinceLastFill back to zero
    void refTimeMinSinceLastFill() {
      TimeMinSinceLastFill = 0;
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

//ERRORS
String const ERR1 = "Switch fired for too long";
String const ERR2 = "Tray did not fill for too long";

// Variable used to keep track of how long a switch has been HIGH
// Milliseconds
int amountTimeHighMil = 0;
// amount of milliseconds a switch has been HIGH to throw an error
int const amountTimeHighForErrorMil = 2000;
// Amount of milliseconds to delay between loops
int const delayTimeMil = 10;
// These three variables are used to keep track of delay since last update of TimeMinSinceLastFill
int totalDelayTimeMil = 0;
int totalDelayTimeSec = 0;
int totalDelayTimeMin = 0;
// If TimeMinSinceLastFill for SVP goes over threshold, flag as an error
int const thresholdTimeMinSinceLastFill = 1;
// Number of times to call checkAndFill() each time loop() is called
int const numTimesCheckAndFill = 10;

void loop() {
  if(MODE == 0) {
    testLoop();
  }
  else{
    for(int i = 0; i < numTimesCheckAndFill; i++) {
      checkAndFillTrays();
    }
    updateTimeSinceLastFill();
  }
}

// Main loop that checks each SVP to see if each tray needs to be filled.
void checkAndFillTrays() {
  dlay(delayTimeMil);
  // Iterate through SVParray
  // If switch is HIGH make correspoiding valve LOW while switch is HIGH
  for(int i = 0; i < numOfSwitches; i++) {
    // If SCP is enabled and the switch is HIGH, fill tray
    if(SVPArray[i].isEnabled() && digitalRead(SVPArray[i].getInPin()) == HIGH) {
      SVPArray[i].refTimeMinSinceLastFill(); // Set timeSinceLastFill back to 0
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
        SVPArray[i].createError(ERR1);
        SVPArray[i].disable();
      }
      // set amountTimeHighMil back to 0
      amountTimeHighMil = 0;
      // Write HIGH to valve
      digitalWrite(SVPArray[i].getOutPin(), HIGH);
    }
  }
}

// Update TimeMinSinceLastFill for each SVP
// Then check each SVP to see if its
// TimeMinSinceLastFill is over the threshold
void updateTimeSinceLastFill() {
  // If totalDelayTimeSec > 60, increment totalDelayTimeMin
  // by number of minuts and set TDTS to itself mod 60
  if(totalDelayTimeSec > 60) {
    totalDelayTimeMin += totalDelayTimeSec / 60;
    totalDelayTimeSec = totalDelayTimeSec % 60;
    for(int i = 0; i < numOfSwitches; i++) {
      SVPArray[i].incTimeMinSinceLastFill(totalDelayTimeMin);
      if(SVPArray[i].getTimeMinSinceLastFill() > thresholdTimeMinSinceLastFill) {
        SVPArray[i].createError(ERR2);
        SVPArray[i].disable();
      }
    }
  }
  // Set totalDelayTimeMin back to zero
  totalDelayTimeMin = 0;
}

// Delay for timeMil
// Increment numDelays
void dlay(int timeMil) {
  delay(timeMil);
  totalDelayTimeMil += timeMil;
  // If totalDelayTimeMil > 1000, increment totalDelayTimeSec
  // by number of seconds and set TDTM to itself mod 1000
  if(totalDelayTimeMil > 1000) {
    totalDelayTimeSec += totalDelayTimeMil / 1000;
    totalDelayTimeMil = totalDelayTimeMil % 1000;
  }
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
  for(int i = 0; i < numTimesCheckAndFill; i++) {
    Serial.print("CheckAndFill #");
    Serial.println(i);
    testCheckAndFillTrays();
  }
  testUpdateTimeMinSinceLastFill();
}

void testCheckAndFillTrays() {
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
//      if(amountTimeHighMil > amountTimeHighForErrorMil) {
//        //Serial.println("Throwing error");
//        SVPArray[i].createError("switchFiredForTooLong");
//        SVPArray[i].disable();
//        Serial.println("Creating error");
//      }
      // set amountTimeHighMil back to 0
      amountTimeHighMil = 0;
      digitalWrite(SVPArray[i].getOutPin(), HIGH);
      Serial.print("Delay Time (mil, sec, min): ");
      Serial.print(totalDelayTimeMil);
      Serial.print(", ");
      Serial.print(totalDelayTimeSec);
      Serial.print(", ");
      Serial.println(totalDelayTimeMin);
    } else {
      Serial.print("ERROR: ");
      Serial.print(SVPArray[i].getID());
      Serial.print(" ");
      Serial.println(SVPArray[i].getError());
      dlay(1000);
      digitalWrite(SVPArray[i].getOutPin(), HIGH);
      Serial.print("Delay Time (mil, sec, min): ");
      Serial.print(totalDelayTimeMil);
      Serial.print(", ");
      Serial.print(totalDelayTimeSec);
      Serial.print(", ");
      Serial.println(totalDelayTimeMin);
    }
  }
}

void testUpdateTimeMinSinceLastFill() {
  Serial.println("Entering testUpdateTimeMinSinceLastFill()...");
  // Update TimeMinSinceLastFill for each SVP
  // If it is higher than threshold, create error and disable
   if(totalDelayTimeSec > 60) {
    Serial.println("totalDelayTimeSec > 60");
    totalDelayTimeMin += totalDelayTimeSec / 60;
    totalDelayTimeSec = totalDelayTimeSec % 60;
    for(int i = 0; i < numOfSwitches; i++) {
      Serial.print("Checking switch ");
      Serial.println(i);
      SVPArray[i].incTimeMinSinceLastFill(totalDelayTimeMin);
      Serial.print("TimeMinSinceLastFill: ");
      Serial.println(SVPArray[i].getTimeMinSinceLastFill());
      if(SVPArray[i].getTimeMinSinceLastFill() > thresholdTimeMinSinceLastFill) {
        Serial.print(SVPArray[i].getTimeMinSinceLastFill());
        Serial.println(" : ERR2 created");
        SVPArray[i].createError(ERR2);
        SVPArray[i].disable();
      }
      delay(500);
    }
  }
  // Set totalDelayTimeMin back to zero
  totalDelayTimeMin = 0;
  Serial.println("Exiting...");
}
