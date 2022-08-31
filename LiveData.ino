#include "Nextion.h"
#include "Aranet4.h"

// We will take up to 300 measurments form the aranet4.
#define MEASUREMENT_COUNT 300


/*
void tTempPopCallback(void *ptr);
NexText tTemp = NexText(0, 3, "tTemp");
*/


// Mac address for the aranet4
String address = "ea:fb:f1:4a:35:c0";

class MyAranet4Callbacks: public Aranet4Callbacks {
    uint32_t onPinRequested() {
        Serial.println("PIN Requested. Enter PIN in serial console.");
        while(Serial.available() == 0)
            vTaskDelay(500 / portTICK_PERIOD_MS);
        return  Serial.readString().toInt();
    }
};

Aranet4 ar4(new MyAranet4Callbacks());

/*
void tTempPopCallback(void *ptr) {
    dbSerialPrintln("tTempPopCallback");
    tTemp.setText("50");
}
*/

void setup() {
    Serial.begin(115200);
    Serial.println("Init");
    Aranet4::init();
    //tTemp.attachPop(tTempPopCallback);
}

void loop() {
    long sleep = 1000 * 60;

    Serial.println("Connecting...");
    if (ar4.connect(address) == AR4_OK) {
      // Storing the data.
      AranetDataCompact data[MEASUREMENT_COUNT];
      AranetData data1 = ar4.getCurrentReadings();
      
      // count = total measurments in Aranet4
      uint16_t count = ar4.getTotalReadings();
      Serial.printf("Total logs in available: %i\n", count);
      
      // getting the most recent 300 measurments
      int start = count - MEASUREMENT_COUNT + 1;
      int recived = ar4.getHistory(start, MEASUREMENT_COUNT, data);
      
      // Printing the 300 measurments to the serial monitor.
      for(int i = 0; i < recived; i++) {
        Serial.printf("#%i\t %i ppm\t %.1f C\t %.1f hPa\t %i %% \n", i + 1, data[i].co2, data[i].temperature/20.0, data[i].pressure/10.0, data[i].humidity);
      }

      sleep = (data1.interval - data1.ago) * 1000;

      float temp = ("%.1f", data[299].temperature/20.0);

      //Printing the latest data to the serial monitor.
      Serial.printf("The latest meausurment: %i ppm\t %.1f C\t %i %% \n", data1.co2, data1.temperature/20.0, data1.humidity);    
    
    }
    else {
        Serial.println("Failed to conenct.");
    }
        
    ar4.disconnect();
    
    Serial.printf("Waiting %is for next measurement\n", sleep / 1000);
    delay(sleep);

  
}
