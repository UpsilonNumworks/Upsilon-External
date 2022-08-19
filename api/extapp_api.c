#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern void (* const *_api_base)(void);

// We need to redefine DataTime struct to match the one in extapp_api.h file, because we can't include it directly
struct DateTime {
  int tm_sec;
  int tm_min;
  int tm_hour; // 0-23
  int tm_mday; // 1-31
  int tm_mon;  // 1-12
  int tm_year;
  int tm_wday; // 0-6, 0 is Monday
};

// Settings
struct Settings {
  // 0 for degrees, 1 for radians, 2 for gradians
  uint8_t angleUnit;
  // 0 for decimal, 1 for Scientific, 2 for Engineering
  uint8_t displayMode;
  // Raw number of digits, max is 14
  uint8_t numberOfSignificantDigits;
  // 0 for real, 1 for cartesian, 2 for polar
  uint8_t complexFormat;
  // If true, the big font should be used.
  bool largeFont;
};


uint64_t extapp_millis() {
  return ((uint64_t (*)(void))_api_base[0])();
}

void extapp_msleep(uint32_t ms) {
  ((void (*)(uint32_t))_api_base[1])(ms);
}

uint64_t extapp_scanKeyboard() {
  return ((uint64_t (*)(void))_api_base[2])();
}

void extapp_pushRect(int16_t x, int16_t y, uint16_t w, uint16_t h, const uint16_t * pixels) {
  ((void (*)(int16_t, int16_t, uint16_t, uint16_t, const uint16_t *))_api_base[3])(x, y, w, h, pixels);
}

void extapp_pushRectUniform(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color) {
  ((void (*)(int16_t, int16_t, uint16_t, uint16_t, uint16_t))_api_base[4])(x, y, w, h, color);
}

void extapp_pullRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t * pixels) {
  ((void (*)(int16_t, int16_t, uint16_t, uint16_t, uint16_t *))_api_base[5])(x, y, w, h, pixels);
}

int16_t extapp_drawTextLarge(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg, bool fake) {
  return ((int16_t (*)(const char *, int16_t, int16_t, uint16_t, uint16_t, bool))_api_base[6])(text, x, y, fg, bg, fake);
}

int16_t extapp_drawTextSmall(const char *text, int16_t x, int16_t y, uint16_t fg, uint16_t bg, bool fake) {
  return ((int16_t (*)(const char *, int16_t, int16_t, uint16_t, uint16_t, bool))_api_base[7])(text, x, y, fg, bg, fake);
}

bool extapp_waitForVBlank(void) {
  return ((bool (*)(void))_api_base[8])();
}

void extapp_clipboardStore(const char *text) {
  ((void (*)(const char *))_api_base[9])(text);
}

const char * extapp_clipboardText(void) {
  return ((const char * (*)(void))_api_base[10])();
}

int extapp_fileListWithExtension(const char ** filenames, int maxrecords, const char * extension, int storage) {
  return ((int (*)(const char **, int, const char *, int))_api_base[11])(filenames, maxrecords, extension, storage);
}

bool extapp_fileExists(const char * filename, int storage) {
  return ((bool (*)(const char *, int))_api_base[12])(filename, storage);
}

bool extapp_fileErase(const char * filename, int storage) {
  return ((bool (*)(const char *, int))_api_base[13])(filename, storage);
}

const char * extapp_fileRead(const char * filename, size_t *len, int storage) {
  return ((const char * (*)(const char *, size_t *, int))_api_base[14])(filename, len, storage);
}

bool extapp_fileWrite(const char * filename, const char * content, size_t len, int storage) {
  return ((bool (*)(const char *, const char *, size_t, int))_api_base[15])(filename, content, len, storage);
}

void extapp_lockAlpha() {
  ((void (*)(void))_api_base[16])();
}

void extapp_resetKeyboard() {
  ((void (*)(void))_api_base[17])();
}

int extapp_getKey(bool allowSuspend, bool *alphaWasActive) {
  return ((int (*)(bool, bool *))_api_base[18])(allowSuspend, alphaWasActive);
}

bool extapp_isKeydown(int key){
  return ((bool (*)(int))_api_base[19])(key);
}

int extapp_restorebackup(int mode){
  return ((int (*)(int))_api_base[20])(mode);
}

bool extapp_erasesector(void * ptr){
  return ((bool (*)(void *))_api_base[21])(ptr);
}

bool extapp_writememory(unsigned char * dest,const unsigned char * data,size_t length){
  return ((bool (*)(unsigned char *, const unsigned char *, size_t))_api_base[22])(dest,data,length);
}

bool extapp_inExamMode(){
  return ((bool (*)(void ))_api_base[23])();
}

uint8_t extapp_getBrightness(){
  return ((uint8_t (*)(void ))_api_base[24])();
}

void extapp_setBrightness(uint8_t brightness){
  ((void (*)(uint8_t))_api_base[25])(brightness);
}

int extapp_batteryLevel(){
  return ((int (*)(void ))_api_base[26])();
}

float extapp_batteryVoltage(){
  return ((float (*)(void ))_api_base[27])();
}

bool extapp_batteryCharging(){
  return ((bool (*)(void ))_api_base[28])();
}

int extapp_batteryPercentage(){
  return ((int (*)(void ))_api_base[29])();
}

struct DateTime extapp_getDateTime(){
  return ((struct DateTime (*)(void ))_api_base[30])();
}

void extapp_setDateTime(struct DateTime dt){
  ((void (*)(struct DateTime))_api_base[31])(dt);
}

void extapp_setRTCMode(int mode){
  ((void (*)(int))_api_base[32])(mode);
}

int extapp_getRTCMode(){
  return ((int (*)(void ))_api_base[33])();
}

void extapp_getTime(struct DateTime *dt){
  ((void (*)(struct DateTime *))_api_base[34])(dt);
}

uint32_t extapp_random(){
  return ((uint32_t (*)(void ))_api_base[35])();
}

void extapp_reloadTitleBar(){
  ((void (*)(void ))_api_base[36])();
}

const char * extapp_username(){
  return ((const char * (*)(void ))_api_base[37])();
}

const char * extapp_getOS(){
  return ((const char * (*)(void ))_api_base[38])();
}

const char * extapp_getOSVersion(){
  return ((const char * (*)(void ))_api_base[39])();
}

void extapp_getOSCommit(char *commit){
  ((void (*)(char *))_api_base[40])(commit);
}

size_t extapp_storageSize(){
  return ((size_t (*)(void ))_api_base[41])();
}

size_t extapp_storageAvailable(){
  return ((size_t (*)(void ))_api_base[42])();
}

size_t extapp_storageUsed(){
  return ((size_t (*)(void ))_api_base[43])();
}


struct Settings extapp_getSettings(){
  return ((struct Settings (*)(void ))_api_base[44])();
}

void extapp_setSettings(struct Settings settings){
  ((void (*)(struct Settings))_api_base[45])(settings);
}
