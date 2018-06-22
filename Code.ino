#include <pt.h>
#include <Http.h>
#include <FlowMeter.h>
#include <ArduinoJson.h>

int RX_PIN = 11;
int TX_PIN = 10;
int RST_PIN = 12;
static struct pt pt1;
FlowMeter Meter = FlowMeter(2);
const unsigned long period = 1000;
HTTP http(9600, RX_PIN, TX_PIN, RST_PIN);
char *dtostrf(double val, signed char width, unsigned char prec, char *s);

void MeterISR() {
  // let our flow meter count the pulses
  Meter.count();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting!");  
  attachInterrupt(INT0, MeterISR, RISING);
  Meter.reset();
  PT_INIT(&pt1);
}

void loop() {
    delay(period);

    Meter.tick(period);

    if(Meter.getCurrentFlowrate() > 10){
        Serial.println(Meter.getCurrentFlowrate());
         http.configureBearer("movistar.es");
          http.connect();
          char response[256];
       
          char str_temp[6];
          char buf [256];  
          dtostrf(Meter.getCurrentFlowrate(), 4, 2, str_temp);
          sprintf(buf,"{\"data\":\ %s }", str_temp);
          
          Result result = http.post("www.rannet.com.br/postrannet", buf, response);
      
          Serial.println(response);
      
          http.disconnect();
    }
     
}
