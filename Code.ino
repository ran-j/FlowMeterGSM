#include <pt.h>
#include <Http.h>
#include <FlowMeter.h>
#include <ArduinoJson.h>

int tes = 0;
int RX_PIN = 11;
int TX_PIN = 10;
int RST_PIN = 12;
static struct pt pt1;
FlowMeter Meter = FlowMeter(2);
const unsigned long period = 1000;
HTTP http(9600, RX_PIN, TX_PIN, RST_PIN);

void MeterISR() {
  // let our flow meter count the pulses
  Meter.count();
}

void Post(){
    http.configureBearer("movistar.es");
    http.connect();
    char response[256];

    char myJson[80];
    char jsonInicio[16]="{\"data\":";
     
    char meioJson[5] = "\"";
    char finalJ[5] = "\"\}";
    
    sprintf(myJson,"%s %s %s %s",jsonInicio,meioJson,Meter.getCurrentFlowrate(),finalJ);
 
    Result result = http.post("www.rannet.com.br/test", myJson, response);

    Serial.println(response);

    http.disconnect();
}

static int protothread1(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    Post();
  }
  PT_END(pt);
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
        protothread1(&pt1, 200);     
    }
     
}
