// Battery Level Indicator (BLI) 
// The purpose of this program is to monitor the power bank battery level
// and send alert when battery is low using a Raspberry Pi Pico
// In Serial Monitor, send 0 or 1 to toggle Debug Mode.

// BLI: Read the power bank battery level, when battery level reaches below 39% (1 bar LED), print 1 in Serial Monitor.

const int analogPinBLI =  26; //PB0;
const float REFERENCE_VOLTAGE = 3.3;
const int NUM_READINGS = 10; // take 10 readings to calculate average
int currentReading = 0; // initial reading
double averageVoltage = 0.0; // calculate avg for BLI only
double batteryLevel = 0.0;
double sum = 0.0;
int batteryAlert = 0; // Alert feature: Send 1 or 0 
int isDebug = 1; // Use int instead of bool
const int LOW_BATTERY_THRESHOLD = 39;
const double EMPTY_VOLTAGE_ADC = 620.0;
const double FULL_VOLTAGE_ADC = 800.0;
const double EMPTY_VOLTAGE = 22.07;
const double FULL_VOLTAGE = 27.72;
int ADCValueBLI = 0;

// Function to calculate BATTERY voltage BLI based on ADC reading and calibration values
double calculateVoltageBLI(int ADCValueBLI) {
  return EMPTY_VOLTAGE + ((ADCValueBLI - EMPTY_VOLTAGE_ADC) / (FULL_VOLTAGE_ADC - EMPTY_VOLTAGE_ADC) * (FULL_VOLTAGE - EMPTY_VOLTAGE));
}


double calculateVoltageBLIOnly(int ADCValueBLI) {
  return REFERENCE_VOLTAGE * (ADCValueBLI / 1023.0);
}

void setup() {
  Serial.begin(9600);
 // analogReadResolution(12); // set 12-bit ADC reading for pico
  Serial.println("Setup Complete");
}

void loop() {
  // Read ADC values for BLI and CSD
  ADCValueBLI = analogRead(analogPinBLI);

  // Calculate voltage for BLI
  double voltageBLI = calculateVoltageBLI(ADCValueBLI);
  double voltageBLIOnly = calculateVoltageBLIOnly(ADCValueBLI);

  // Loop to accumulate voltage readings for BLI averaging
  if (currentReading < NUM_READINGS) {
    sum += voltageBLI;
    currentReading++;
  } else {
    // Calculate average voltage BLI
    averageVoltage = sum / NUM_READINGS;

    // Calculate battery level
    batteryLevel = (averageVoltage - EMPTY_VOLTAGE) * (100.0 / (FULL_VOLTAGE - EMPTY_VOLTAGE));

    // Reset counter for next averaging cycle
    currentReading = 0;
    sum = 0;

    // BLI alert feature
    // Check if avg voltage BLI is lower than threshold 
    if (batteryLevel < LOW_BATTERY_THRESHOLD) {
      batteryAlert = 1;
    } else {
      batteryAlert = 0;
    }
  }



  // Handle debug mode user input
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    //String incomingChar = Serial.readString();
    Serial.print("Received: ");
    Serial.println(incomingChar);

    if (incomingChar == '1') {
      isDebug = 1;
      Serial.println("Debug mode ON");
    } else if (incomingChar == '0') {
      isDebug = 0;
      Serial.println("Debug mode OFF");
    }
  }

// calibration equation plotted from excel
//y = 1.1321x - 2.8716 
double averageVoltageCORR = (1.1321*averageVoltage) - 2.8716;


  // Print data
  if (isDebug) {
    Serial.print("voltageBLIOnly: ");
    Serial.print(voltageBLIOnly);
    Serial.print(" ADC Value BLI: ");
    Serial.print(ADCValueBLI);
    Serial.print("  Avg Voltage BLI: ");
    Serial.print(averageVoltageCORR, 2); 
    Serial.print("V  Battery Level: ");
    Serial.print(batteryLevel, 2);
    Serial.print("%  BLI Alert: ");
    Serial.print(batteryAlert);

  } else {
    Serial.print("1,");
    Serial.print(voltageBLIOnly);
    Serial.println(",");
    Serial.print("2,");
    Serial.print(ADCValueBLI); 
    Serial.println(",");
    Serial.print("3,");
    Serial.print(averageVoltageCORR, 2); 
    Serial.println(",");
    Serial.print("4,");
    Serial.print(batteryLevel, 2);
    Serial.println(",");
    Serial.print("5,");
    Serial.print(batteryAlert);
    Serial.println(",");

  }

  delay(100); // Add a delay between readings  
}
