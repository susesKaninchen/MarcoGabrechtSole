#include "Arduino.h"
#define webserver_h
//#######################   Includes
#ifdef ESP32      //#####   ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include "SPIFFS.h"
#elif defined(ESP8266)//#   ESP8266
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif            // ####   ESP32/ESP8266
#include <ArduinoJson.h>
#include "ESPAsyncWebServer.h"//Webserver
#include "FS.h"   //Include File System Headers

extern void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

extern "C" {
  typedef void (*parameterizedCallbackFunction)(int);
  typedef void (*pCallbackFunction)(int, int, int, int);
}

class DatapointsInt {
  public:
    int dPoints[20];
    int dPoints2[20];
    int dLabls[20];
    int counter = 0;
    String labelData = "";
    String toString() {
      String lbls = "";
      for (int i = 0; i<counter; i++) {
        if (i == counter -1) {
          lbls.concat(String(dLabls[i]));
        } else {
          lbls.concat(String(dLabls[i]) + ",");
        }
      }
      String dats = "";
      String dats2 = "";
      for (int i = 0; i<counter; i++) {
        if (i == counter -1) {
          dats.concat(String(dPoints[i]));
          dats2.concat(String(dPoints2[i]));
        } else {
          dats.concat(String(dPoints[i]) + ",");
          dats2.concat(String(dPoints2[i]) + ",");
        }
      }
      counter = 0; 
      return "{'labels':[" + lbls + "],'datasets':[{'label':'Pressure Front','backgroundColor':'rgb(255,99,132)','borderColor':'rgb(255,99,132)','data':[" + dats2 + "]},{'label':'Pressure Back','backgroundColor':'rgb(0,99,132)','borderColor':'rgb(0,99,132)','data\':[" + dats + "]}]}";
    }
    int addData(int dp, int dl) {
      if (counter == 500) {
        return -1;
      }
      dPoints[counter] = dp;
      dLabls[counter] = dl;
      counter++;
      return 500-counter;
    }
    int addData(int dp, int dp2, int dl) {
      if (counter == 500) {
        return -1;
      }
      dPoints[counter] = dp;
      dPoints2[counter] = dp2;
      dLabls[counter] = dl;
      counter++;
      return 500-counter;
    }
};

class WebObject {
  public:
    bool updatet = true;
    bool updateText = false;
    String gcolumn = "1";
    String grow = "1";
    int index = 0;
    String type = "card";
    String text = "Text";
    int value = 0;
    int min = 0;
    int max = 100;
    parameterizedCallbackFunction function;
    pCallbackFunction pFunction;
    void setUpdateText(bool b) {
      updateText = b;
    }
    String toString() {
      if (type == "slider") {
        return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"min\":\"" + String(min) + "\",\"max\":\"" + String(max) + "\",\"value\":\"" + String(value) + "\",\"gr\":\"" + grow + "\",\"gc\":\"" + gcolumn + "\"}";
      } else if (type == "btn") {
        return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
      } else if (type == "card") {
        return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
      } else if (type == "ta") {
        return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
      } else if (type == "log") {
        return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
      } else if (type == "graf") {
        return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
      }
      return "{\"action\":\"draw\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"min\":\"" + String(min) + "\",\"max\":\"" + String(max) + "\",\"value\":\"" + String(value) + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
    }
    String textUpdateToString() {
      if (type == "slider") {
        return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\"}";
      } else if (type == "btn") {
        return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\"}";
      } else if (type == "card") {
        return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\"}";
      } else if (type == "ta") {
        return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\"}";
      } else if (type == "log") {
        return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\"}";
      } else if (type == "graf") {
        return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\"}";
      }
      return "{\"action\":\"set\",\"i\":\"" + String(index) + "\",\"t\":\"" + text + "\",\"ty\":\"" + type + "\",\"min\":\"" + String(min) + "\",\"max\":\"" + String(max) + "\",\"value\":\"" + String(value) + "\",\"gr\":\"" + String(grow) + "\",\"gc\":\"" + String(gcolumn) + "\"}";
    }
    void setText(String t) {
      if (type == "log") {
        if (updateText == true) {
          text += t;
        } else {
          text = t;
          setUpdateText(true);
        }
      }else {
        text = t;
        setUpdateText(true);
      }
    }
    void setIndex(int i) {
      index = i;
    }
    void setButton(String te, parameterizedCallbackFunction f){
      function = f;
      type = "btn";
      text= te;
    }
    void setCard(String te){
      type = "card";
      text = te;
    }
    void setLog(String te){
      type = "log";
      text = te;
    }
    void setTextArea(String te){
      type = "ta";
      text = te;
    }
    void setJoy(pCallbackFunction f){
      type = "joy";
      gcolumn = "2";
      grow = "2";
      pFunction = f;
    }
    void setGraph(){
      type = "graf";
      gcolumn = "3";
      grow = "2";
    }
    void setSlider(String te, int val, int mi, int ma, parameterizedCallbackFunction f){
      function = f;
      gcolumn = "3";
      type = "slider";
      text= te;
      value = val;
      min = mi;
      max = ma;
    }
    WebObject(){
    }
    WebObject(String ty, String te, int val, int mi, int ma, parameterizedCallbackFunction f){
      function = f;
      type = ty;
      text = te;
      value = val;
      min = mi;
      max = ma;
    }
};
/*
class Card: public WebObject {
  public:
    String name;
    String type;
    String text;
    Card(String n, String t);
    void update(String t);
};

class Button: public WebObject {
  public:
    String name;
    String type;
    String text;
    Button(String n, String t, parameterizedCallbackFunction f);
    void update(String t);
};

class Slider: public WebObject {
  public:
    String name;
    String type;
    String text;
    int min;
    int max;
    int value;
    Slider(String n, String t, parameterizedCallbackFunction f){
      function = f;
    }
    void update(String t);
};*/

class MyPage {
  private:
    //IPAddress apIP;
    AsyncWebServer *server;
    AsyncWebSocket *ws;
    DynamicJsonDocument *doc;
    DynamicJsonDocument *docSend;
    DynamicJsonDocument *tmp;
    JsonArray dataSend;
    AsyncWebSocketClient * wsclient;
    AsyncEventSource *events;
    int elementCounter;
    WebObject *elements[20];
    int updateIntervall;
    int remoteIntervall;
    long timestampLastUpdate;
    int removedItems = 0;
  public:
    uint16_t port;
    MyPage(uint16_t portS, int updateTime, int remoteUpdateTime) {
      port = portS;
      updateIntervall = updateTime;
      remoteIntervall = remoteUpdateTime;
    }
    bool init();
    bool handleWS(uint8_t *data);
    bool removeElement(int ind){
      if (elementCounter <= ind) {
        return false;
      }
      delete elements[ind];
      for (int iD = ind+1; iD < elementCounter; iD++) {
        elements[iD-1] = elements[iD];
        elements[iD-1]->setIndex(iD-1);
        elements[iD-1]->updatet = true;
      }
      removedItems++;
      elementCounter--;
      updateWebPage();
      return true;
    }
    bool addElement(WebObject *we) {
      if (elementCounter>=20) {
        return false;
      }
      we->setIndex(elementCounter);
      elements[elementCounter] = we;
      elementCounter++;
      updateWebPage();
      return true;
    }
    bool moveElement(int star, int target){
      if (star>=elementCounter) {
        return false;
      }
      if (target>=elementCounter) {
        target = elementCounter-1;
      }
      WebObject *weTemp = elements[target];
      elements[target] = elements[star];
      elements[star] = weTemp;
      elements[star]->setIndex(star);
      elements[star]->updatet = true;
      elements[target]->setIndex(target);
      elements[target]->updatet = true;
      updateWebPage();
      return true;
    }

    void updateAll() {
      for (int iD = 0; iD < elementCounter; iD++) {
        elements[iD]->updatet = true;
      }
      updateWebPage();
    }
    
    void updateWebPage(){
      char output[1024];
      //SendObjectes
      for (int c = 0; c<elementCounter; c++){
        if (elements[c]->updatet) {
          (*tmp)[String(c)] = elements[c]->toString();
          Serial.println((*tmp).memoryUsage());
          if ((*docSend).memoryUsage() + (*tmp).memoryUsage() > 1000) {
            serializeJson((*docSend), output);
            events->send(output);
            (*docSend).clear();
          }
          (*tmp).clear();
          (*docSend)[String(c)] = elements[c]->toString();
          elements[c]->updatet = false;
        } else if (elements[c]->updateText) {
          (*tmp)[String(c)] = elements[c]->textUpdateToString();
          if ((*docSend).memoryUsage() + (*tmp).memoryUsage() > 1000) {
            serializeJson((*docSend), output);
            events->send(output);
            (*docSend).clear();
          }
          (*tmp).clear();
          (*docSend)[String(c)] = elements[c]->textUpdateToString();
          elements[c]->setUpdateText(false);
        }
      }
      //Send RemoveObjects
      int tRmIt = removedItems;
      removedItems == 0;
      for (int c = elementCounter; c<elementCounter + tRmIt; c++){
        String tempElementString = "{\"i\":\"" + String(c) + "\", \"ty\":\"n\"}";
        (*tmp)[String(c)] = tempElementString;
        if ((*docSend).memoryUsage() + (*tmp).memoryUsage() > 1000) {
          serializeJson((*docSend), output);
          events->send(output);
          (*docSend).clear();
        }
        (*tmp).clear();
        (*docSend)[String(c)] = tempElementString;
      }
      //(*docSend)[String(elementCounter)] = "{\"type\":\"updateintervall\", \"value\":\"" + String(remoteIntervall) + "\"}";
      //char* output[docSend->memoryUsage()];
      if(serializeJson((*docSend), output) > 4){
        events->send(output);
        docSend->clear();
      }
    }
    
    void setRemoteSendIntervall(int intervall);
};

bool MyPage::init(){
    server = new AsyncWebServer(port);
    ws = new AsyncWebSocket("/ws");
    doc = new DynamicJsonDocument(1024);
    docSend = new DynamicJsonDocument(1024);
    tmp = new DynamicJsonDocument(1024);
    events = new AsyncEventSource("/events");
    bool errorIndicator = false;
    //Initialize File System
    if (SPIFFS.begin()){
      #ifdef DEBUG
      Serial.println("SPIFFS Initialize....ok");
      #endif
    } else {
      #ifdef DEBUG
      Serial.println("SPIFFS Initialization...failed");
      #endif
      errorIndicator = false;
    }
    // Webpages
    server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    // Events
    server->addHandler(events);
    // Websoket
    ws->onEvent(onWsEvent);
    server->addHandler(ws);
    server->begin();
    return true;
  }

bool MyPage::handleWS(uint8_t *data){
  Serial.println((char*)data);
  DeserializationError error = deserializeJson(*doc, (char*)data);
    if (error) {
      #ifdef DEBUG
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      #endif
      doc->clear();
      return false;
    } else {
      JsonObject root = doc->as<JsonObject>();
      for (JsonPair kv : root) {
        //Sonderbefehle
        if (kv.value().as<JsonObject>()["index"].as<int>() == -1) {
          updateAll();
        }else if (kv.value().as<JsonObject>()["index"].as<int>() == -2) {
          removeElement(kv.value().as<JsonObject>()["value"].as<int>());
        }else if (kv.value().as<JsonObject>()["type"] == "joy") {
          Serial.println(kv.value().as<JsonObject>()["value"].as<String>());
        } else {
          for (int indexOb = 0; indexOb < elementCounter; indexOb++) {
            if (kv.value().as<JsonObject>()["index"].as<int>() == indexOb){
              elements[indexOb]->value = kv.value().as<JsonObject>()["value"].as<int>();
              elements[indexOb]->function(elements[indexOb]->value);
              //Serial.print("kv.Value: ");
              //Serial.println(kv.value().as<JsonObject>()["value"].as<int>());
            }
          }
        }
        //if (strcmp(kv.value().as<JsonObject>()["type"].as<char*>(), "Button") == 0){
        //  Serial.println("Jay ein Button");
        //}
        //Serial.println(kv.value().as<JsonObject>()["value"].as<int>());
        //Serial.println(kv.key().c_str());
        //Serial.println(kv.value().as<JsonObject>()["type"].as<char*>());
      }
    }
    return true;
  }
