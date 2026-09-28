#include <Arduino.h>
#include <BluetoothSerial.h>
#include <math.h>

// MERIVA SMART MID v2.2 - Core OBD-II Real
// Somente leitura. Nao transmitir comandos de controle para a ECU.
// Defina o MAC do ELM327 para conexao automatica.
static const char *ELM_MAC = "";

BluetoothSerial ELM;

enum Quality : uint8_t { INVALID=0, VALID=1, STALE=2, UNSUPPORTED=3, OFFLINE=4 };
struct Sample { float value=NAN; uint32_t ms=0; Quality q=INVALID; };
struct Telemetry {
  Sample rpm, speed, coolant, battery, load, map, maf, throttle, intake, stft, ltft, fuelLevel;
} t;

struct ElmState {
  bool connected=false;
  bool initialized=false;
  bool protocolReady=false;
  uint8_t initStep=0;
  uint8_t failCount=0;
  uint32_t lastAction=0;
} es;

static const uint32_t CMD_TIMEOUT=1800;
static const uint32_t FRESH_MS=3000;
static const uint32_t RETRY_MS=3000;

void setSample(Sample &s, float v) { s.value=v; s.ms=millis(); s.q=VALID; }

int hx(char c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='A' && c<='F') return c-'A'+10;
  if (c>='a' && c<='f') return c-'a'+10;
  return -1;
}

String compactHex(const String &in) {
  String out; out.reserve(in.length());
  for (size_t i=0;i<in.length();i++) if (hx(in[i])>=0) out += in[i];
  out.toUpperCase();
  return out;
}

bool elmCmd(const char *cmd, String &resp, uint32_t timeout=CMD_TIMEOUT) {
  if (!ELM.hasClient()) return false;
  while (ELM.available()) ELM.read();
  ELM.print(cmd); ELM.print('\r');
  resp="";
  uint32_t start=millis();
  while (millis()-start < timeout) {
    while (ELM.available()) {
      char c=(char)ELM.read(); resp += c;
      if (c=='>') return true;
    }
    delay(1);
  }
  return resp.length()>0;
}

bool pidBytes(uint8_t pid, uint8_t *data, uint8_t n) {
  char cmd[8]; snprintf(cmd,sizeof(cmd),"01%02X",pid);
  String r;
  if (!elmCmd(cmd,r)) return false;
  if (r.indexOf("NO DATA")>=0 || r.indexOf("ERROR")>=0 || r.indexOf("UNABLE")>=0) return false;
  String h=compactHex(r);
  char key[5]; snprintf(key,sizeof(key),"41%02X",pid);
  int p=h.indexOf(key);
  if (p<0) return false;
  int pos=p+4;
  for (uint8_t i=0;i<n;i++) {
    if (pos+1 >= (int)h.length()) return false;
    int a=hx(h[pos]), b=hx(h[pos+1]);
    if (a<0 || b<0) return false;
    data[i]=(uint8_t)((a<<4)|b); pos+=2;
  }
  return true;
}

void invalidateStale() {
  Sample *v[]={&t.rpm,&t.speed,&t.coolant,&t.battery,&t.load,&t.map,&t.maf,&t.throttle,&t.intake,&t.stft,&t.ltft,&t.fuelLevel};
  uint32_t now=millis();
  for (Sample *s:v) if (s->q==VALID && now-s->ms>FRESH_MS) s->q=STALE;
}

void pollPids() {
  if (!es.protocolReady) return;
  uint8_t a[2];
  if (pidBytes(0x0C,a,2)) setSample(t.rpm,((a[0]*256.0f)+a[1])/4.0f);
  if (pidBytes(0x0D,a,1)) setSample(t.speed,a[0]);
  if (pidBytes(0x05,a,1)) setSample(t.coolant,a[0]-40.0f);
  if (pidBytes(0x42,a,2)) setSample(t.battery,((a[0]*256.0f)+a[1])/1000.0f);
  if (pidBytes(0x04,a,1)) setSample(t.load,a[0]*100.0f/255.0f);
  if (pidBytes(0x0B,a,1)) setSample(t.map,a[0]);
  if (pidBytes(0x10,a,2)) setSample(t.maf,((a[0]*256.0f)+a[1])/100.0f);
  if (pidBytes(0x11,a,1)) setSample(t.throttle,a[0]*100.0f/255.0f);
  if (pidBytes(0x0F,a,1)) setSample(t.intake,a[0]-40.0f);
  if (pidBytes(0x06,a,1)) setSample(t.stft,(a[0]-128.0f)*100.0f/128.0f);
  if (pidBytes(0x07,a,1)) setSample(t.ltft,(a[0]-128.0f)*100.0f/128.0f);
  if (pidBytes(0x2F,a,1)) setSample(t.fuelLevel,a[0]*100.0f/255.0f);
  invalidateStale();
}

bool initStep() {
  static const char *cmds[]={"ATZ","ATE0","ATL0","ATS0","ATH0","ATSP0","ATDP"};
  if (es.initStep >= 7) return true;
  String r;
  bool ok=elmCmd(cmds[es.initStep],r,2500);
  Serial.printf("[ELM] %s -> %s\n",cmds[es.initStep],r.c_str());
  return ok;
}

void maintainElm() {
  if (ELM.hasClient()) {
    if (!es.connected) {
      es.connected=true; es.initialized=false; es.protocolReady=false; es.initStep=0; es.failCount=0;
      Serial.println("[OBD] Bluetooth connected");
    }
    if (!es.initialized) {
      if (millis()-es.lastAction<200) return;
      es.lastAction=millis();
      if (initStep()) {
        es.initStep++;
        if (es.initStep>=7) { es.initialized=true; Serial.println("[OBD] init complete"); }
      } else if (++es.failCount>=3) {
        es.initStep=0; es.failCount=0; Serial.println("[OBD] init retry");
      }
      return;
    }
    if (!es.protocolReady) {
      uint8_t a[2];
      if (pidBytes(0x0C,a,2)) { es.protocolReady=true; Serial.println("[OBD] real PID response confirmed"); }
      return;
    }
    pollPids();
    return;
  }

  if (es.connected) {
    es.connected=false; es.initialized=false; es.protocolReady=false; es.initStep=0;
    Serial.println("[OBD] Bluetooth disconnected");
  }

  if (!ELM_MAC[0]) return;
  if (millis()-es.lastAction < RETRY_MS) return;
  es.lastAction=millis();
  Serial.println("[OBD] connecting...");
  ELM.connect(ELM_MAC);
}

void setup() {
  Serial.begin(115200);
  delay(250);
  if (!ELM.begin("MERIVA_SMART_MID", true)) Serial.println("[BT] init failure");
  else Serial.println("[BT] Classic/SPP ready");
  Serial.println("[SYSTEM] Meriva Smart MID v2.2 Core OBD-II Real");
}

void loop() {
  maintainElm();
  static uint32_t last=0;
  if (millis()-last>=2000) {
    last=millis();
    Serial.printf("[TEL] RPM=%.0f/%d SPEED=%.0f/%d COOL=%.1f/%d STFT=%.2f LTFT=%.2f\n",
      t.rpm.value,t.rpm.q,t.speed.value,t.speed.q,t.coolant.value,t.coolant.q,t.stft.value,t.ltft.value);
  }
  delay(5);
}
