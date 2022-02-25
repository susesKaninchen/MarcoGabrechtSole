#define DEBUG
#include "DashbordM.h"

// dadad
const int sensorHPin = 35;  // Analog input pin that the potentiometer is attached to
const int sensorVPin = 34;  // Analog input pin that the potentiometer is attached to
const int sensorGPin = 37;  // Analog input pin that the potentiometer is attached to
const int zuluftHPin = 25; // Analog output pin that the LED is attached to
const int verbinderVPin = 22; // Analog output pin that the LED is attached to
const int verbinderHPin = 32; // Analog output pin that the LED is attached to
const int zuluftVPin = 21; // Analog output pin that the LED is attached to

int sensorValueH = 0;        // value read from the pot
int sensorValueV = 0;        // value read from the pot
int sensorValueG = 0;        // value read from the pot
int diff = 0;
int base = 0;
int filter = 0;
int threshold = 15;
int state = 0;
double baseline = 0;
unsigned long startHigh = 0;
unsigned long startLow = 0;
unsigned long laststartHigh = 0;
unsigned long laststartLow = 0;
int stepDetection = 40;
String classification = "";
String dataMessure = "";
bool changedClassification = false;

void messure() {
  //sensorValueG = analogRead(sensorGPin);
  sensorValueH = analogRead(sensorHPin)-sensorValueG;
  sensorValueV = analogRead(sensorVPin)-diff-sensorValueG;
  baseline = baseline*0.990 + sensorValueH*0.010;
}

void calibrate() {
  //Open Valves
  digitalWrite(zuluftHPin, HIGH);
  digitalWrite(zuluftVPin, HIGH);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  digitalWrite(zuluftHPin, LOW);
  digitalWrite(zuluftVPin, LOW);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  
  int MeasurementsToAverage = 20;                // Anzahl der in den Mettelwert aufgenommenen Messungen 
  sensorValueH = 0;
  sensorValueV = 0;
  //sensorValueG = 0;
  for(int i = 0; i < MeasurementsToAverage; ++i)
  {
    sensorValueH += analogRead(sensorHPin);
    sensorValueV += analogRead(sensorVPin);
    //sensorValueG += analogRead(sensorGPin);
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  sensorValueH /= MeasurementsToAverage;
  sensorValueV /= MeasurementsToAverage;
  sensorValueG /= MeasurementsToAverage;
  diff = sensorValueV - sensorValueH;
  base = sensorValueH;
  baseline = base;
  startHigh = millis();
  startLow = millis();
  laststartHigh = millis();
  laststartLow = millis();
}



MyPage w = MyPage(80,500,500);
int counn = 0;
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){
      //w.updateWebPage();
      // Send Eventsorce to all?
      #ifdef DEBUG
        Serial.println("Websocket client connection received");
      #endif
    } else if(type == WS_EVT_DISCONNECT){
      #ifdef DEBUG
        Serial.println("Client disconnected");
      #endif
    } else if(type == WS_EVT_DATA) {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        w.handleWS(data);
      }
      #ifdef DEBUG
        Serial.println("Got data");
      #endif
    }
  }
  WebObject *wobbbb = new WebObject();
  WebObject *ta = new WebObject();
  WebObject *wob = new WebObject();
  WebObject *logE = new WebObject();
  DatapointsInt *pp = new DatapointsInt();
  int cc = 0;
  bool messureMent = false;
  bool messureMent2 = false;
  
void setup() {
  messure();
  pinMode(zuluftHPin, OUTPUT);//ZuluftHacke
  pinMode(verbinderVPin, OUTPUT);//VerbinderV
  pinMode(verbinderHPin, OUTPUT);//VerbinderH
  pinMode(zuluftVPin, OUTPUT);//ZuluftV
  digitalWrite(zuluftHPin, LOW);
  digitalWrite(verbinderVPin, LOW);
  digitalWrite(verbinderHPin, LOW);
  digitalWrite(zuluftVPin, LOW);
  vTaskDelay(50 / portTICK_PERIOD_MS);
  messure();// Remove inductive nois
  vTaskDelay(1 / portTICK_PERIOD_MS);
  calibrate();

  
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  IPAddress apIP = IPAddress(192,1,1,1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  #ifdef ESP32
    WiFi.softAP("TestPage", ""); //Create WiFi hotspot
  #else
    WiFi.softAP("TestPage", ""); //Create WiFi hotspot
  #endif
  w.init();
  wob->setCard("Starte");
  w.addElement(wob);
  //ta->setTextArea("Data");
  //w.addElement(ta);
  //WebObject *wobButton = new WebObject();
  //wobButton->setButton("Add Button", addBtn);
  //w.addElement(wobButton);
  /*WebObject *wobButtonUpdate = new WebObject();
  wobButtonUpdate->setButton("Messure Pressure", toggleMes);
  w.addElement(wobButtonUpdate);
  WebObject *wobButtonU = new WebObject();
  wobButtonU->setButton("Messure Flanks", toggleMes2);
  w.addElement(wobButtonU);*/
  WebObject *slider = new WebObject();
  slider->setSlider("Max Pressure", 1200, 600,1500, testt);
  w.addElement(slider);
  logE->setLog("");
  w.addElement(logE);
   WebObject *pf = new WebObject();
  pf->setButton("Pump Front", [](int t) {state = 0;});
  w.addElement(pf);
  WebObject *pb = new WebObject();
  pb->setButton("Pump Back", [](int t) {state = 1;});
  w.addElement(pb);
   WebObject *eo = new WebObject();
  eo->setButton("Even Out", [](int t) {state = 2;});
  w.addElement(eo);
   WebObject *pp = new WebObject();
  pp->setButton("PAUSE", [](int t) {state = 3;});
  w.addElement(pp);
   WebObject *op = new WebObject();
  op->setButton("OPEN", [](int t) {state = 4;});
  w.addElement(op);
    WebObject *ca = new WebObject();
  ca->setButton("OPEN", [](int t) {state = 6;});
  w.addElement(ca);
  //WebObject *joy = new WebObject();
  //joy->setJoy(testJ);
  //w.addElement(joy);
  //WebObject *wobbb = new WebObject();
  //wobbb->setCard("Hallo dies ist ein Text. BlaBl");
  //w.addElement(wobbb);
  //-> global
  wobbbb->setGraph();
  w.addElement(wobbbb);
  //wobbbb->setText("{labels: [8],datasets: [{label: 'Pressure Front',backgroundColor: 'rgb(255, 99, 132)',borderColor: 'rgb(255, 99, 132)',data: [45],},{label: 'Pressure Back',backgroundColor: 'rgb(0, 99, 132)',borderColor: 'rgb(0, 99, 132)',data: [0],}]}");
  
  Serial.println("setup Fertig");
}

void toggleMes(int i){
  messureMent = !messureMent;
}
void toggleMes2(int i){
  messureMent2 = !messureMent2;
  ta->setText(dataMessure);
  //dataMessure = "";
  //Serial.println(dataMessure);
}

void testJ(int i, int i2, int i3, int i4) {
  Serial.print("X: ");
  Serial.println(i);
  Serial.print("X: ");
  Serial.println(i2);
  Serial.print("Speed: ");
  Serial.println(i3);
  Serial.print("Degree: ");
  Serial.println(i4);
}

void testt(int i) {
  state = i;
}

void moveMeRight(int i) {
  w.moveElement(i, i++);
}

void addBtn(int i) {
  WebObject *wobButtonUpdate = new WebObject("btn", String(counn++), 50, 0, 180, updateWp);
  w.addElement(wobButtonUpdate);
}

void updateWp(int i) {
  w.removeElement(i);
}

// the loop function runs over and over again forever
void loop() {
  messure();
  if (messureMent){
    if(cc%10 == 0) {
    //Serial.print(sensorValueH);
    //Serial.print("\t");
    //Serial.println(sensorValueV);
    }
  }
  if (state == 0) {// PUMP_FRONT
    if (sensorValueH<base-threshold) {
      digitalWrite(zuluftHPin, HIGH);//ZuluftHacke
    }else {
      digitalWrite(zuluftHPin, LOW);//ZuluftHacke
    }
    if (sensorValueV<base-threshold) {
      digitalWrite(zuluftVPin, HIGH);//ZuluftHacke
    }else {
      digitalWrite(zuluftVPin, LOW);//ZuluftHacke
    }
    if (sensorValueV<sensorValueH - threshold) {// von leiner Kammer in große Kammer
      digitalWrite(verbinderVPin, HIGH);
      digitalWrite(verbinderHPin, HIGH);
    }else {
      digitalWrite(verbinderVPin, LOW);
      digitalWrite(verbinderHPin, LOW);
    }
  } else if (state == 1) {// PUMP_BACK
    if (sensorValueH<base-threshold) {
      digitalWrite(zuluftHPin, HIGH);//ZuluftHacke
    }else {
      digitalWrite(zuluftHPin, LOW);//ZuluftHacke
    }
    if (sensorValueV<base-threshold) {
      digitalWrite(zuluftVPin, HIGH);//ZuluftHacke
    }else {
      digitalWrite(zuluftVPin, LOW);//ZuluftHacke
    }
    if (sensorValueH<sensorValueV - threshold) {// von leiner Kammer in große Kammer
      digitalWrite(verbinderVPin, HIGH);
      digitalWrite(verbinderHPin, HIGH);
    }else {
      digitalWrite(verbinderVPin, LOW);
      digitalWrite(verbinderHPin, LOW);
    }
  } else if (state == 2) {// EVEN_OUT
     digitalWrite(verbinderVPin, HIGH);
     digitalWrite(verbinderHPin, HIGH);
     if (cc%100 == 0) {
      state = 3;
    }
  } else if (state == 3) {// PAUSE
    digitalWrite(zuluftHPin, LOW);
    digitalWrite(verbinderVPin, LOW);
    digitalWrite(verbinderHPin, LOW);
    digitalWrite(zuluftVPin, LOW);
  } else if (state == 4) {// OPEN
    digitalWrite(zuluftHPin, HIGH);
    digitalWrite(verbinderVPin, LOW);
    digitalWrite(verbinderHPin, LOW);
    digitalWrite(zuluftVPin, HIGH);
  } else if (state == 5) {// EVEN_OUT
    
  } else if (state == 6) {// CALLIBRATE
    calibrate();
    state = 3;
  } else if (state == 7) {// KeineAhung
    
  } else {// Go To Value FRONT
    if (sensorValueH<base-threshold) {
        digitalWrite(zuluftHPin, HIGH);//ZuluftHacke
      }else {
        digitalWrite(zuluftHPin, LOW);//ZuluftHacke
      }
      if (sensorValueV<base-threshold) {
        digitalWrite(zuluftVPin, HIGH);//ZuluftHacke
      }else {
        digitalWrite(zuluftVPin, LOW);//ZuluftHacke
      }
    if (sensorValueV < state) {
      if (sensorValueV<sensorValueH - threshold) {// von leiner Kammer in große Kammer
        digitalWrite(verbinderVPin, HIGH);
        digitalWrite(verbinderHPin, HIGH);
      }else {
        digitalWrite(verbinderVPin, LOW);
        digitalWrite(verbinderHPin, LOW);
      }
    } else {
      digitalWrite(zuluftHPin, LOW);
      digitalWrite(verbinderVPin, LOW);
      digitalWrite(verbinderHPin, LOW);
      digitalWrite(zuluftVPin, LOW);
    }
  }
  //Stepdetection
  if (startHigh > startLow) {// last step high
    if (baseline - stepDetection > sensorValueH) {
      //if (messureMent2){
      //  Serial.print(millis()-startLow);
      //  Serial.print("\t");
      //}
      laststartLow = startLow;
      startLow = millis();
    }
  }else {// last step low
    if (baseline + stepDetection < sensorValueH) {
      if (messureMent2){
        dataMessure += String(millis()-startHigh) + String(",");
        //Serial.println(millis()-startHigh);
      }
      laststartHigh = startHigh;
      startHigh = millis();
    }
  }
  if (millis() - startHigh > 2101) {
    classification = "Stehen";
  }else if (startHigh - laststartHigh > 1028) {
    classification = "Gehen";
  }else {
    classification = "Rennen";
  }
  if (cc%100 == 0) {
    pp->addData(sensorValueH, sensorValueV, (int) millis());
    //ta->setText("" + String(millis()-startHigh));
  }
  if (wob->text != classification) {
    changedClassification = true;
  }else {
    changedClassification = false;
  }
  if (cc%1000 == 0 || changedClassification) {
    wob->setText(classification);
    wobbbb->setText(pp->toString());
    logE->setText(classification + ": " + String(startHigh - laststartHigh));
    w.updateWebPage();
  }
  cc++;
  vTaskDelay(1 / portTICK_PERIOD_MS);
}
