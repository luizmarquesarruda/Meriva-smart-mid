#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <BluetoothSerial.h>
#include <math.h>

// MERIVA SMART MID v2.0 / PCB REV A / ESP32-WROOM-32E / 3.3V
// J1/J2 TID remain 1:1. TID active driving is disabled until electrical/protocol validation.
#define PIN_IGN_SENSE 35
#define PIN_BUZZER 26
#define PIN_SD_CS 5
#define PIN_SD_SCK 18
#define PIN_SD_MISO 19
#define PIN_SD_MOSI 23
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22
#define PIN_TID_SCL 32
#define PIN_TID_SDA 33
#define PIN_TID_MRQ 27
#define PIN_GNSS_RX 16
#define PIN_GNSS_TX 17
#define PIN_GNSS_PPS 4
#define SHUTDOWN_DELAY_MS 30000UL
#define OBD_PERIOD_MS 1500UL
#define DNA_SAVE_PERIOD_MS 60000UL

const char *TRIP_FILE="/MERIVA_MID/TRIPS/trips.csv";
const char *EVENT_FILE="/MERIVA_MID/EVENTS/events.csv";
const char *DNA_FILE="/MERIVA_MID/AI/dna.csv";
BluetoothSerial ELM; HardwareSerial GNSS(2);
enum State{RUNNING,SHUTTING_DOWN}; State state=RUNNING;
struct Telemetry{float rpm=NAN,speed=NAN,coolant=NAN,battery=NAN,load=NAN,map=NAN,maf=NAN,throttle=NAN,intake=NAN,stft=NAN,ltft=NAN,fuelLevel=NAN;bool valid=false;} t;
struct GPSData{bool fix=false,valid=false;double lat=NAN,lon=NAN;float speed=NAN;} gps;
struct Trip{bool active=false;uint32_t startEpoch=0,seconds=0;double distance=0,fuel=0;float maxSpeed=0;} trip;
struct DNA{uint32_t samples=0;double km=0,fuel=0;float consumption=NAN,avgSpeed=NAN,coolant=NAN,battery=NAN,stft=NAN,ltft=NAN;} dna;
uint32_t ignOff=0,lastObd=0,lastDNA=0;

uint8_t bcd2dec(uint8_t x){return(x>>4)*10+(x&15);}
bool rtcRead(uint16_t &y,uint8_t &mo,uint8_t &d,uint8_t &h,uint8_t &mi,uint8_t &s){Wire.beginTransmission(0x52);Wire.write(0);if(Wire.endTransmission(false)!=0)return false;if(Wire.requestFrom((uint8_t)0x52,(uint8_t)7)!=7)return false;uint8_t r[7];for(int i=0;i<7;i++)r[i]=Wire.read();s=bcd2dec(r[0]&127);mi=bcd2dec(r[1]&127);h=bcd2dec(r[2]&63);d=bcd2dec(r[4]&63);mo=bcd2dec(r[5]&31);y=2000+bcd2dec(r[6]);return true;}
uint32_t epochApprox(uint16_t y,uint8_t m,uint8_t d,uint8_t h,uint8_t mi,uint8_t s){int yy=y;yy-=m<=2;int era=(yy>=0?yy:yy-399)/400;unsigned yoe=(unsigned)(yy-era*400),doy=(153*(m+(m>2?-3:9))+2)/5+d-1,doe=yoe*365+yoe/4-yoe/100+doy;int64_t days=(int64_t)era*146097+doe-719468;return(uint32_t)(days*86400LL+h*3600UL+mi*60UL+s);}
void beep(uint8_t n){while(n--){digitalWrite(PIN_BUZZER,HIGH);delay(80);digitalWrite(PIN_BUZZER,LOW);if(n)delay(80);}}
void storageInit(){if(!SD.begin(PIN_SD_CS,SPI)){Serial.println("[SD] FAIL");return;}SD.mkdir("/MERIVA_MID");SD.mkdir("/MERIVA_MID/CONFIG");SD.mkdir("/MERIVA_MID/TRIPS");SD.mkdir("/MERIVA_MID/RAW");SD.mkdir("/MERIVA_MID/EVENTS");SD.mkdir("/MERIVA_MID/MAINTENANCE");SD.mkdir("/MERIVA_MID/AI");if(!SD.exists(TRIP_FILE)){File f=SD.open(TRIP_FILE,FILE_WRITE);if(f){f.println("epoch,duration_s,distance_km,max_speed_kmh,avg_speed_kmh,fuel_l,km_l");f.close();}}if(!SD.exists(EVENT_FILE)){File f=SD.open(EVENT_FILE,FILE_WRITE);if(f){f.println("epoch,event,detail");f.close();}}if(!SD.exists(DNA_FILE)){File f=SD.open(DNA_FILE,FILE_WRITE);if(f){f.println("epoch,km,fuel_l,km_l,avg_speed,samples,coolant,battery,stft,ltft");f.close();}}}
void eventLog(const char *e,const char *d){File f=SD.open(EVENT_FILE,FILE_APPEND);if(!f)return;uint16_t y;uint8_t m,dd,h,mi,s;uint32_t ep=0;if(rtcRead(y,m,dd,h,mi,s))ep=epochApprox(y,m,dd,h,mi,s);f.printf("%lu,%s,%s\n",(unsigned long)ep,e,d);f.close();}
double coord(const char *v,char hemi){if(!v||!*v)return NAN;double x=atof(v);int deg=(int)(x/100.0);double out=deg+(x-deg*100.0)/60.0;if(hemi=='S'||hemi=='W')out=-out;return out;}
void parseRMC(char *line){if(strncmp(line,"$GPRMC",6)&&strncmp(line,"$GNRMC",6))return;char *f[13]={};uint8_t n=0;char *p=line;while(p&&n<13){f[n++]=p;p=strchr(p,',');if(p)*p++=0;}if(n<9)return;gps.fix=f[2][0]=='A';if(gps.fix){gps.lat=coord(f[3],f[4][0]);gps.lon=coord(f[5],f[6][0]);gps.speed=atof(f[7])*1.852f;gps.valid=true;}else gps.valid=false;}
void gnssUpdate(){static char line[128];static uint8_t i=0;while(GNSS.available()){char c=GNSS.read();if(c=='\n'){line[i]=0;parseRMC(line);i=0;}else if(c!='\r'){if(i<sizeof(line)-1)line[i++]=c;else i=0;}}}
bool hexByte(const String&s,int p,uint8_t&v){if(p+2>(int)s.length())return false;auto hx=[](char c){if(c>='0'&&c<='9')return c-'0';if(c>='A'&&c<='F')return c-'A'+10;if(c>='a'&&c<='f')return c-'a'+10;return -1;};int a=hx(s[p]),b=hx(s[p+1]);if(a<0||b<0)return false;v=(a<<4)|b;return true;}
bool elmCmd(const String&cmd,String&r,uint32_t timeout=1200){if(!ELM.hasClient())return false;while(ELM.available())ELM.read();ELM.print(cmd);ELM.print('\r');r="";uint32_t st=millis();while(millis()-st<timeout){while(ELM.available()){char c=ELM.read();r+=c;if(c=='>')return true;}delay(1);}return r.length()>0;}
bool pidBytes(uint8_t pid,uint8_t*out,uint8_t n){char cmd[8];snprintf(cmd,sizeof(cmd),"01%02X",pid);String r;if(!elmCmd(cmd,r))return false;r.replace("\r"," ");r.replace("\n"," ");char key[8];snprintf(key,sizeof(key),"41 %02X",pid);int p=r.indexOf(key);if(p>=0)p+=strlen(key);else{char k2[8];snprintf(k2,sizeof(k2),"41%02X",pid);p=r.indexOf(k2);if(p<0)return false;p+=4;}while(p<(int)r.length()&&r[p]==' ')p++;for(uint8_t i=0;i<n;i++){if(!hexByte(r,p,out[i]))return false;p+=2;while(p<(int)r.length()&&r[p]==' ')p++;}return true;}
void obdUpdate(){if(!ELM.hasClient())return;uint8_t a[2];if(pidBytes(0x0C,a,2))t.rpm=((a[0]*256.0f)+a[1])/4.0f;if(pidBytes(0x0D,a,1))t.speed=a[0];if(pidBytes(0x05,a,1))t.coolant=a[0]-40.0f;if(pidBytes(0x42,a,2))t.battery=((a[0]*256.0f)+a[1])/1000.0f;if(pidBytes(0x04,a,1))t.load=a[0]*100.0f/255.0f;if(pidBytes(0x0B,a,1))t.map=a[0];if(pidBytes(0x10,a,2))t.maf=((a[0]*256.0f)+a[1])/100.0f;if(pidBytes(0x11,a,1))t.throttle=a[0]*100.0f/255.0f;if(pidBytes(0x0F,a,1))t.intake=a[0]-40.0f;if(pidBytes(0x06,a,1))t.stft=(a[0]-128.0f)*100.0f/128.0f;if(pidBytes(0x07,a,1))t.ltft=(a[0]-128.0f)*100.0f/128.0f;if(pidBytes(0x2F,a,1))t.fuelLevel=a[0]*100.0f/255.0f;t.valid=true;}
void tripStart(){trip={};trip.active=true;uint16_t y;uint8_t m,d,h,mi,s;if(rtcRead(y,m,d,h,mi,s))trip.startEpoch=epochApprox(y,m,d,h,mi,s);eventLog("TRIP_START","ignition_on");}
void tripUpdate(uint32_t dt){if(!trip.active)return;trip.seconds+=dt/1000UL;if(!isnan(t.speed)){if(t.speed>trip.maxSpeed)trip.maxSpeed=t.speed;if(t.speed>1.0)trip.distance+=t.speed*(dt/3600000.0);}}
float avgSpeed(){return trip.seconds?trip.distance/(trip.seconds/3600.0):0;}
void dnaUpdate(){if(!trip.active||trip.distance<0.01)return;dna.samples++;dna.km+=trip.distance;if(!isnan(t.coolant))dna.coolant=isnan(dna.coolant)?t.coolant:dna.coolant*.95f+t.coolant*.05f;if(!isnan(t.battery))dna.battery=isnan(dna.battery)?t.battery:dna.battery*.95f+t.battery*.05f;if(!isnan(t.stft))dna.stft=isnan(dna.stft)?t.stft:dna.stft*.95f+t.stft*.05f;if(!isnan(t.ltft))dna.ltft=isnan(dna.ltft)?t.ltft:dna.ltft*.95f+t.ltft*.05f;dna.avgSpeed=(dna.avgSpeed*(dna.samples-1)+avgSpeed())/dna.samples;}
void dnaSave(){File f=SD.open(DNA_FILE,FILE_APPEND);if(!f)return;uint16_t y;uint8_t m,d,h,mi,s;uint32_t ep=0;if(rtcRead(y,m,d,h,mi,s))ep=epochApprox(y,m,d,h,mi,s);f.printf("%lu,%.3f,%.3f,%.3f,%.2f,%lu,%.2f,%.2f,%.2f,%.2f\n",(unsigned long)ep,dna.km,dna.fuel,dna.consumption,dna.avgSpeed,(unsigned long)dna.samples,dna.coolant,dna.battery,dna.stft,dna.ltft);f.close();}
void tripSave(){if(!trip.active)return;dnaUpdate();File f=SD.open(TRIP_FILE,FILE_APPEND);if(f){f.printf("%lu,%lu,%.3f,%.1f,%.2f,%.3f,%.3f\n",(unsigned long)trip.startEpoch,(unsigned long)trip.seconds,trip.distance,trip.maxSpeed,avgSpeed(),trip.fuel,trip.fuel>0.001?trip.distance/trip.fuel:NAN);f.close();}else eventLog("SD_ERROR","trip_file");dnaSave();trip.active=false;eventLog("TRIP_END","saved");}
void setup(){Serial.begin(115200);pinMode(PIN_IGN_SENSE,INPUT);pinMode(PIN_BUZZER,OUTPUT);digitalWrite(PIN_BUZZER,LOW);Wire.begin(PIN_I2C_SDA,PIN_I2C_SCL,100000);Wire.beginTransmission(0x52);Serial.println(Wire.endTransmission()==0?"[RTC] RV-3028 OK":"[RTC] RV-3028 not found");SPI.begin(PIN_SD_SCK,PIN_SD_MISO,PIN_SD_MOSI,PIN_SD_CS);storageInit();GNSS.begin(9600,SERIAL_8N1,PIN_GNSS_RX,PIN_GNSS_TX);ELM.begin("MERIVA_MID");beep(1);Serial.println("[SYSTEM] Meriva Smart MID v2.0 Rev A");if(digitalRead(PIN_IGN_SENSE)==HIGH)tripStart();}
void loop(){bool ign=digitalRead(PIN_IGN_SENSE)==HIGH;uint32_t now=millis();if(now-lastObd>=OBD_PERIOD_MS){uint32_t dt=now-lastObd;lastObd=now;if(state==RUNNING||state==SHUTTING_DOWN){gnssUpdate();obdUpdate();tripUpdate(dt);}}if(now-lastDNA>=DNA_SAVE_PERIOD_MS){lastDNA=now;if(state==RUNNING&&trip.active)dnaSave();}if(state==RUNNING){if(!ign){ignOff=millis();state=SHUTTING_DOWN;eventLog("IGN_OFF","timer_30s");}}else{if(ign){state=RUNNING;eventLog("IGN_ON","shutdown_cancelled");}else if(millis()-ignOff>=SHUTDOWN_DELAY_MS){tripSave();esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_IGN_SENSE,HIGH);Wire.end();SPI.end();Serial.flush();delay(50);esp_deep_sleep_start();}}delay(2);}
