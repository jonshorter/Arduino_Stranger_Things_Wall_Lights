#include <ESP8266mDNS.h>
#include <ESP8266mDNS_Legacy.h>
#include <LEAmDNS.h>
#include <LEAmDNS_lwIPdefs.h>
#include <LEAmDNS_Priv.h>

#include <Arduino.h>
#include <ArduinoTrace.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


/* #define DEFAULT_STRING " ABCD " // for testing light setup
   #define DEFAULT_STRING "ABCDEFGHQPONMLJKIRSTUVWXYZQPONMLJKI" */
#define DEFAULT_STRING " HELP ME   I AM HERE   RUN "

// SSID and Password to your WiFi Access Point
const char* ssid = "ENTER_YOUR_WIFI_AP_HERE";
const char* password = "ENTER_YOUR_WIFI_PW_HERE";

//                    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
String letterIndex = "Z(Y:X)W+VU0TS;R--$IJ9KL8M&NO7PQ!--*H6GF5ED3CB2A";
String colorLetterIndex = "Z(Y:X)W+VU0TS;R$IJ9KL8M&NO7PQ!*H6GF5ED3CB2A";

#define LETTER_DURATION 600 // => 600
#define PAUSE_DURATION 300 // => 300
#define MAX_NORMAL_DURATION 600000 // => 60000 1 minute
#define ORIGINAL_COLOR true
#define DO_NOT_SHOW false

#define DATA_PIN    D2
#define NUM_LEDS    47
#define BRIGHTNESS  (254)
CRGB leds[NUM_LEDS];

#define TITLE_PAGE "Stranger Things wall"
#define NORMAL_LIGHTS "Display lights normally (no messages):"
#define NORMAL_LIGHTS_DURATION "Delay Between Messages (1-20 mins): "
#define TEXT_LETTER_DURATION "Letter duration (60-1200ms): "
#define TEXT_PAUSE_DURATION "Wait between letter duration (60-600ms): "
#define TYPE_MESSAGE "Type message to display: "
#define ORIGINAL_COLORS "Stranger Things Colours (unchecked uses white):"
#define LED_COLOR "LED color"
#define SUBMIT "submit"
#define BRIGHTNESS_TEXT "Brightness (1-254): "

int marker = 0;
int wordMark = 0;
int pauseDuration = PAUSE_DURATION;
int letterDuration = LETTER_DURATION;
String hostString = "stranger.local";
String filtered = DEFAULT_STRING;
String message = DEFAULT_STRING;
bool originalColor = ORIGINAL_COLOR;
bool doNotShow = DO_NOT_SHOW;
int brightness = BRIGHTNESS;
int maxNormalDuration = MAX_NORMAL_DURATION;


uint32_t colorIndex[44] = {
  0x0000FF, // Z
  0xFF00FF, // (
  0x00FFB9, // Y
  0x0000FF, // :
  0xFCFF21, // X
  0xFF0000, // )
  0x00FFB9, // W
  0xFF00FF, // +
  0x00FFB9, // V
  0x0000FF, // U
  0x00FFB9, // 0
  0xFCFF21, // T
  0xFF00B9, // S
  0x00FFB9, // ;
  0xFF00FF, // R
  0xFF00FF, // $
  0x00FFB9, // I
  0xFFF0C4, // J
  0xFCFF21, // 9
  0x0000FF, // K
  0xFF0000, // L
  0x00FFB9, // 8
  0xFCFF21, // M
  0xFF00FF, // &
  0xFF0000, // N
  0xFFF0C4, // O
  0x0000FF, // 7
  0xFF00FF, // P
  0x00FFB9, // Q
  0x0000FF, // !
  0xFCFF21, // *
  0xFF0000, // 6
  0xFCFF21, // H
  0x00FFB9, // G
  0xFF00FF, // F
  0x00FFB9, // 5
  0x0000FF, // E
  0x00FFB9, // 4
  0xFCFF21, // D
  0xFF0000, // 3
  0x00FFB9, // C
  0x0000FF, // B
  0x00FF00, // 2
  0xFF0000  // A
};
ESP8266WebServer server(80); //Server on port 80

String getPage() {
  String page = "<html><head>";
  page += "<meta charset='utf-8'>";

  page += "<title>" + String(TITLE_PAGE) + "</title>";

  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0' />";

  page += "</head><body>";
  page += "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 81.37 26.49'><defs><style>.cls-1{fill:#ed2024;}</style></defs><title>s_f_w</title><g id='Calque_2' data-name='Calque 2'><g id='Calque_1-2' data-name='Calque 1'><path class='cls-1' d='M15.35,8.67a2.72,2.72,0,0,1-.28,1.21,3.34,3.34,0,0,1-.79,1,4.09,4.09,0,0,1-1.23.71,4.81,4.81,0,0,1-1.61.26,6.91,6.91,0,0,1-.81,0c-.25,0-.48-.08-.68-.13l-.54-.13a1.91,1.91,0,0,0-.36-.05.59.59,0,0,0-.34.09l-.22.19L8.21,8.05l.18,0a5.06,5.06,0,0,0,.49,1.32,4,4,0,0,0,.69.92,2.65,2.65,0,0,0,.82.54,2.46,2.46,0,0,0,.91.18,2,2,0,0,0,1.33-.43,1.36,1.36,0,0,0,.49-1.07A1.15,1.15,0,0,0,13,8.93a1.16,1.16,0,0,0-.31-.39,2.11,2.11,0,0,0-.49-.29L11.57,8,10.2,7.45a3.3,3.3,0,0,1-1.26-.83,2.08,2.08,0,0,1-.49-1.48,2.49,2.49,0,0,1,.24-1.09,2.73,2.73,0,0,1,.69-.86,3.15,3.15,0,0,1,1.07-.59,4.36,4.36,0,0,1,1.39-.21A4.45,4.45,0,0,1,13,2.53a3.6,3.6,0,0,0,.89.13.57.57,0,0,0,.33-.08,1.08,1.08,0,0,0,.2-.16l.11,3.18h-.18a3.92,3.92,0,0,0-.37-1,2.59,2.59,0,0,0-.55-.73,2.09,2.09,0,0,0-.68-.44A1.85,1.85,0,0,0,12,3.29a1.47,1.47,0,0,0-1,.31.94.94,0,0,0-.34.73.66.66,0,0,0,.1.37A1.1,1.1,0,0,0,11,5a1.9,1.9,0,0,0,.34.2l.39.17,1.43.56A6.28,6.28,0,0,1,14,6.3a2.9,2.9,0,0,1,.7.54,2.41,2.41,0,0,1,.48.75A3.14,3.14,0,0,1,15.35,8.67Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M22.46,5.35A2.74,2.74,0,0,0,21.71,4a2.06,2.06,0,0,0-1.43-.57v7.21a.66.66,0,0,0,.54.72l.13,0v.38H17.24v-.38l.11,0a.84.84,0,0,0,.4-.24.69.69,0,0,0,.15-.48V3.4a2.14,2.14,0,0,0-.67.11,2.17,2.17,0,0,0-.63.34,2,2,0,0,0-.65.77,5.82,5.82,0,0,0-.31.76l-.16,0,.5-3a.57.57,0,0,0,.21.19,1.37,1.37,0,0,0,.36,0h5a.66.66,0,0,0,.33,0,.92.92,0,0,0,.21-.17l.53,3Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M27.74,11.73v-.38l.13,0a.32.32,0,0,0,.18-.13.38.38,0,0,0,.07-.24,2.59,2.59,0,0,0-.07-.5,7.33,7.33,0,0,0-.25-.87,9.37,9.37,0,0,0-.49-1.18c-.21-.45-.48-.93-.8-1.47l-.72.26v3.41a1.06,1.06,0,0,0,.09.42.56.56,0,0,0,.43.3l.11,0v.38H22.76v-.38l.12,0a.66.66,0,0,0,.53-.72V3.67a.81.81,0,0,0-.14-.51.48.48,0,0,0-.39-.23l-.12,0V2.53h5.46l.42,0,.43,0a1.25,1.25,0,0,1,.81.43,1.26,1.26,0,0,1,.34.9A1.79,1.79,0,0,1,29.79,5a4.19,4.19,0,0,1-1.25,1,7.64,7.64,0,0,1,.57.84,8.66,8.66,0,0,1,.56,1c.18.35.34.72.49,1.1a6.77,6.77,0,0,1,.35,1.11c.08.3.16.56.25.8a.66.66,0,0,0,.47.42l.12,0v.38Zm0-7.46a.93.93,0,0,0-.26-.66,1.06,1.06,0,0,0-.8-.26,1.3,1.3,0,0,0-.49.07.62.62,0,0,0-.26.17.47.47,0,0,0-.11.25,1.5,1.5,0,0,0,0,.29V6.27a2.9,2.9,0,0,0,.87-.38,2.37,2.37,0,0,0,.6-.51,2.25,2.25,0,0,0,.36-.57A1.39,1.39,0,0,0,27.74,4.27Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M36.22,11.73v-.38l.19,0a.29.29,0,0,0,.19-.17,1.3,1.3,0,0,0,.07-.53,2.21,2.21,0,0,0-.25-1.06,2.82,2.82,0,0,0-.66-.83,3.17,3.17,0,0,0-.9-.54,2.65,2.65,0,0,0-1-.19l-.61,2.22c0,.09,0,.19-.05.31a1.69,1.69,0,0,0,0,.3.43.43,0,0,0,.13.37.67.67,0,0,0,.3.13h.13v.38H31.24v-.38l.11,0a.71.71,0,0,0,.49-.4,4.27,4.27,0,0,0,.27-.7l2-6.74c0-.06,0-.12,0-.18a1,1,0,0,0,0-.17A.21.21,0,0,0,34,2.91h-.07V2.53H37.6V2.9h-.09a.23.23,0,0,0-.21.24,1,1,0,0,0,0,.16,1.16,1.16,0,0,0,0,.18l1.82,7.07a2.13,2.13,0,0,0,.18.51.58.58,0,0,0,.38.26l.12,0v.38ZM35,3.75,34.1,7.2a2.32,2.32,0,0,1,.49,0,5.62,5.62,0,0,1,.55.15,3,3,0,0,1,.52.23,1.52,1.52,0,0,1,.41.31Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M45.79,11.73v-.38l.14,0a.65.65,0,0,0,.51-.72v-.28c0-.14,0-.26,0-.37a1.91,1.91,0,0,0-.06-.33,2.14,2.14,0,0,0-.16-.35,4.34,4.34,0,0,0-.28-.42l-3.85-5v6.76a.66.66,0,0,0,.51.72l.13,0v.38H40.27v-.38l.13,0a.67.67,0,0,0,.53-.72V3.67a.88.88,0,0,0-.14-.51.51.51,0,0,0-.39-.23l-.13,0V2.53h4.41v.38l-.15,0a.16.16,0,0,0-.17.18.22.22,0,0,0,0,.14.31.31,0,0,0,.09.12l1.94,2.36V3.67a.82.82,0,0,0-.14-.52.49.49,0,0,0-.39-.22l-.12,0V2.53h2.45v.38l-.13,0a.48.48,0,0,0-.39.23.88.88,0,0,0-.14.51v6.94a.69.69,0,0,0,.15.48.66.66,0,0,0,.38.24l.13,0v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M54,11.73q-.55,0-1,0a3.84,3.84,0,0,1-.91-.14,3.94,3.94,0,0,1-.83-.31,4.35,4.35,0,0,1-.82-.57A4.64,4.64,0,0,1,49.2,9a5.78,5.78,0,0,1-.42-2.27A4.23,4.23,0,0,1,49.12,5a4.31,4.31,0,0,1,.93-1.38,4.36,4.36,0,0,1,1.41-.92,4.73,4.73,0,0,1,1.77-.34,4.16,4.16,0,0,1,.75.07c.21,0,.41.08.58.13l.48.13a1.14,1.14,0,0,0,.38.07.45.45,0,0,0,.3-.1l.2-.21L56.2,5.6l-.15,0a3.15,3.15,0,0,0-1-1.63,2.36,2.36,0,0,0-1.52-.54A2,2,0,0,0,52,4.24a3.76,3.76,0,0,0-.63,2.38,7.69,7.69,0,0,0,.18,1.74A4.78,4.78,0,0,0,52,9.67a2.72,2.72,0,0,0,.73.83,1.44,1.44,0,0,0,.86.29,1,1,0,0,0,.48-.09.56.56,0,0,0,.24-.24.8.8,0,0,0,.09-.31c0-.11,0-.22,0-.34V7.36a.81.81,0,0,0-.16-.5.68.68,0,0,0-.39-.24l-.14,0V6.22h3.68v.36h-.09a.53.53,0,0,0-.4.24,1,1,0,0,0-.11.49v3.29a.75.75,0,0,0,.16.5.69.69,0,0,0,.36.22l.13,0v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M64.15,12a.41.41,0,0,0-.28-.21,1.64,1.64,0,0,0-.4,0H57.92v-.38l.13,0a.67.67,0,0,0,.53-.72V3.67a.81.81,0,0,0-.14-.51.48.48,0,0,0-.39-.23l-.13,0V2.53h5.37a2.26,2.26,0,0,0,.42,0A.46.46,0,0,0,64,2.27l.48,3.08-.17,0a2.77,2.77,0,0,0-.38-.91,2.92,2.92,0,0,0-.59-.66,2.4,2.4,0,0,0-.7-.38,2.46,2.46,0,0,0-.75-.13c-.39,0-.64.07-.74.21a.93.93,0,0,0-.16.58v1h.6A1.48,1.48,0,0,0,62.08,5a.7.7,0,0,0,.31-.2,1.09,1.09,0,0,0,.17-.32c0-.13.07-.27.1-.42h.16l.37,3.22-.18,0a2.09,2.09,0,0,0-.56-1.08,1.37,1.37,0,0,0-.91-.35H61V10a1.71,1.71,0,0,0,.07.52.71.71,0,0,0,.2.29.73.73,0,0,0,.34.12l.48,0a2.2,2.2,0,0,0,.78-.15,2.9,2.9,0,0,0,.78-.47,3.8,3.8,0,0,0,.71-.77,5.37,5.37,0,0,0,.59-1.09l.15,0Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M70.38,11.73v-.38l.13,0a.29.29,0,0,0,.18-.13.38.38,0,0,0,.07-.24,2.59,2.59,0,0,0-.07-.5,7,7,0,0,0-.24-.87A11.15,11.15,0,0,0,70,8.41a15.18,15.18,0,0,0-.81-1.47l-.71.26v3.41a1.25,1.25,0,0,0,.08.42.59.59,0,0,0,.44.3l.11,0v.38H65.4v-.38l.13,0a.67.67,0,0,0,.53-.72V3.67a.88.88,0,0,0-.14-.51.51.51,0,0,0-.39-.23l-.13,0V2.53h5.46l.43,0,.42,0a1.31,1.31,0,0,1,1.15,1.33A1.83,1.83,0,0,1,72.44,5a4.37,4.37,0,0,1-1.26,1,9.31,9.31,0,0,1,.58.84c.19.31.38.64.56,1s.34.72.49,1.1a7.9,7.9,0,0,1,.35,1.11,5,5,0,0,0,.25.8.64.64,0,0,0,.46.42l.13,0v.38Zm0-7.46a.92.92,0,0,0-.25-.66,1.1,1.1,0,0,0-.81-.26,1.22,1.22,0,0,0-.48.07.6.6,0,0,0-.27.17.47.47,0,0,0-.11.25c0,.1,0,.2,0,.29V6.27a3.14,3.14,0,0,0,.87-.38,2.78,2.78,0,0,0,.6-.51,2.25,2.25,0,0,0,.36-.57A1.58,1.58,0,0,0,70.38,4.27Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M7,22.15a2.74,2.74,0,0,0-.75-1.38A2.09,2.09,0,0,0,4.8,20.2v7.21a.66.66,0,0,0,.55.72l.12,0v.38H1.76v-.38l.12,0a.81.81,0,0,0,.39-.24.69.69,0,0,0,.15-.48V20.2a2.08,2.08,0,0,0-.66.11,2.21,2.21,0,0,0-.64.34,2,2,0,0,0-.65.77,4.54,4.54,0,0,0-.3.76l-.17,0,.5-3a.79.79,0,0,0,.21.19,1.41,1.41,0,0,0,.37,0h5a.66.66,0,0,0,.33,0,.72.72,0,0,0,.2-.17l.53,3Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M11.87,28.53v-.38l.11,0a1,1,0,0,0,.4-.24.69.69,0,0,0,.15-.48v-4.8H10.32v4.8a.67.67,0,0,0,.53.72l.13,0v.38H7.28v-.38l.13,0a.67.67,0,0,0,.53-.72V20.47A.83.83,0,0,0,7.81,20a.6.6,0,0,0-.39-.24l-.14,0v-.38H11v.38l-.14,0c-.35.07-.52.31-.52.73V21.8h2.21V20.47a.65.65,0,0,0-.53-.73l-.13,0v-.38h3.7v.38l-.14,0c-.35.07-.52.31-.52.73v6.94a.65.65,0,0,0,.52.72l.14,0v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M16,28.53v-.38l.14,0a.65.65,0,0,0,.52-.72V20.47a.88.88,0,0,0-.13-.51.48.48,0,0,0-.39-.23l-.14,0v-.38h3.7v.38l-.13,0a.51.51,0,0,0-.39.23.88.88,0,0,0-.14.51v6.94a.69.69,0,0,0,.15.48.73.73,0,0,0,.38.24l.13,0v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M25.73,28.53v-.38l.14,0a.65.65,0,0,0,.52-.72v-.28c0-.14,0-.26,0-.37a1.3,1.3,0,0,0-.07-.33,1.48,1.48,0,0,0-.15-.35,4.68,4.68,0,0,0-.29-.42l-3.85-5v6.76a.66.66,0,0,0,.52.72l.13,0v.38H20.22v-.38l.12,0a.66.66,0,0,0,.53-.72V20.47a.81.81,0,0,0-.14-.51.48.48,0,0,0-.39-.23l-.12,0v-.38h4.41v.38l-.16,0a.17.17,0,0,0-.17.18.3.3,0,0,0,.05.14l.09.12,1.95,2.36V20.47a.89.89,0,0,0-.14-.52.49.49,0,0,0-.39-.22l-.13,0v-.38h2.45v.38l-.12,0a.51.51,0,0,0-.4.23.88.88,0,0,0-.14.51v6.94a.69.69,0,0,0,.15.48.72.72,0,0,0,.39.24l.12,0v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M34,28.53c-.37,0-.72,0-1,0a3.84,3.84,0,0,1-.9-.14,3.94,3.94,0,0,1-.83-.31,4.35,4.35,0,0,1-.82-.57,4.48,4.48,0,0,1-1.22-1.69,5.69,5.69,0,0,1-.42-2.26,4.4,4.4,0,0,1,.33-1.7A4.51,4.51,0,0,1,30,20.45a4.15,4.15,0,0,1,1.41-.92,4.69,4.69,0,0,1,1.77-.34,3.26,3.26,0,0,1,.74.07c.22,0,.41.08.59.13l.48.13a1.4,1.4,0,0,0,.37.07.5.5,0,0,0,.31-.1l.2-.21.28,3.12-.16,0a3.09,3.09,0,0,0-1-1.63,2.34,2.34,0,0,0-1.52-.54,2,2,0,0,0-1.61.78,3.76,3.76,0,0,0-.63,2.38,7.13,7.13,0,0,0,.19,1.74A5.11,5.11,0,0,0,32,26.47a2.75,2.75,0,0,0,.74.83,1.42,1.42,0,0,0,.86.29A1,1,0,0,0,34,27.5a.53.53,0,0,0,.25-.24.8.8,0,0,0,.09-.31c0-.11,0-.22,0-.34V24.16a.75.75,0,0,0-.16-.5.63.63,0,0,0-.38-.24l-.14,0V23h3.68v.36h-.1a.51.51,0,0,0-.39.24.94.94,0,0,0-.11.49v3.29a.68.68,0,0,0,.52.72l.12,0v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M45.19,25.47a2.72,2.72,0,0,1-.28,1.21,3.19,3.19,0,0,1-.79,1,4.09,4.09,0,0,1-1.23.71,4.79,4.79,0,0,1-1.6.26,7.08,7.08,0,0,1-.82-.05c-.25,0-.48-.08-.68-.13l-.53-.13a2.11,2.11,0,0,0-.37-.05.54.54,0,0,0-.33.09,1.45,1.45,0,0,0-.23.19l-.28-3.74.18,0a5.2,5.2,0,0,0,.49,1.32,4,4,0,0,0,.69.92,2.58,2.58,0,0,0,.83.54,2.38,2.38,0,0,0,.91.18,2,2,0,0,0,1.32-.43,1.37,1.37,0,0,0,.5-1.07,1.15,1.15,0,0,0-.12-.55,1.16,1.16,0,0,0-.31-.39,2.11,2.11,0,0,0-.49-.29c-.19-.09-.4-.18-.64-.27L40,24.25a3.3,3.3,0,0,1-1.26-.83,2.08,2.08,0,0,1-.49-1.48,2.49,2.49,0,0,1,.24-1.09,2.89,2.89,0,0,1,.69-.87,3.32,3.32,0,0,1,1.07-.58,4.36,4.36,0,0,1,1.39-.21,4.59,4.59,0,0,1,1.15.14,3.46,3.46,0,0,0,.88.13.57.57,0,0,0,.33-.08,1.08,1.08,0,0,0,.2-.16l.11,3.18h-.18a3.92,3.92,0,0,0-.36-1,2.82,2.82,0,0,0-.56-.73,1.94,1.94,0,0,0-.68-.44,1.85,1.85,0,0,0-.71-.15,1.47,1.47,0,0,0-1,.31,1,1,0,0,0-.34.73.66.66,0,0,0,.1.37,1.32,1.32,0,0,0,.25.28,1.9,1.9,0,0,0,.34.2l.39.17,1.43.56a6.28,6.28,0,0,1,.83.39,2.9,2.9,0,0,1,.7.54,2.41,2.41,0,0,1,.48.75A3.14,3.14,0,0,1,45.19,25.47Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M59.57,19.74a.43.43,0,0,0-.25.13.63.63,0,0,0-.16.23,1.18,1.18,0,0,0-.1.3c0,.11-.05.21-.06.31l-1.26,6.51c0,.06,0,.15-.07.28a1.75,1.75,0,0,0,0,.25.62.62,0,0,0,0,.27.38.38,0,0,0,.27.13v.38H55v-.38h.11a.17.17,0,0,0,.15-.11.83.83,0,0,0,0-.35.69.69,0,0,0,0-.13l-.81-6.31-1,6.1c0,.1,0,.2-.05.32s0,.19,0,.24a.23.23,0,0,0,.2.24h.08v.38H50.88v-.38H51a.17.17,0,0,0,.15-.09.54.54,0,0,0,0-.19v-.15a1,1,0,0,1,0-.16l-1.14-7a2,2,0,0,0-.14-.51.56.56,0,0,0-.39-.26l-.1,0v-.38h3.46v.38l-.13,0a.48.48,0,0,0-.33.21.68.68,0,0,0-.09.35c0,.07,0,.14,0,.21a1.36,1.36,0,0,0,0,.2l.6,5,.76-5.17a1.49,1.49,0,0,1,0-.17c0-.1,0-.15,0-.16a1.07,1.07,0,0,0,0-.32c0-.1-.14-.16-.32-.17l-.05,0v-.38h3.26v.38l-.1,0a.33.33,0,0,0-.31.37s0,.08,0,.17,0,.17,0,.24L57,25.83l1-5.19c0-.1,0-.19,0-.29s0-.2,0-.3a.3.3,0,0,0-.09-.21.39.39,0,0,0-.25-.11l-.08,0v-.38h2.11v.38Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M64,28.53v-.38l.18,0a.29.29,0,0,0,.19-.17,1.3,1.3,0,0,0,.07-.53,2.21,2.21,0,0,0-.25-1.06,2.82,2.82,0,0,0-.66-.83,3.17,3.17,0,0,0-.9-.54,2.65,2.65,0,0,0-1-.19L61.1,27q0,.13-.06.3a1.89,1.89,0,0,0,0,.31.46.46,0,0,0,.13.37.66.66,0,0,0,.31.13h.12v.38H59.05v-.38l.11,0a.71.71,0,0,0,.49-.4,3.79,3.79,0,0,0,.27-.7l2-6.74a1.16,1.16,0,0,0,0-.18,1,1,0,0,0,0-.17.21.21,0,0,0-.19-.23H61.7v-.37h3.71v.37h-.09a.23.23,0,0,0-.21.24,1,1,0,0,0,0,.16,1.16,1.16,0,0,0,0,.18L67,27.36a3.63,3.63,0,0,0,.18.51.58.58,0,0,0,.38.26l.13,0v.38Zm-1.22-8L61.91,24a2.32,2.32,0,0,1,.49.05,5.62,5.62,0,0,1,.55.15,3.54,3.54,0,0,1,.53.23,1.63,1.63,0,0,1,.4.31Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M73.71,28.76a.85.85,0,0,0-.64-.23h-5v-.38h.13a.51.51,0,0,0,.4-.23.89.89,0,0,0,.13-.5V20.47c0-.45-.18-.7-.55-.74l-.11,0v-.38h3.7v.38l-.14,0a.6.6,0,0,0-.39.24.83.83,0,0,0-.13.49v6.4a.91.91,0,0,0,.19.67.81.81,0,0,0,.58.18,1.36,1.36,0,0,0,.65-.19,3.1,3.1,0,0,0,.71-.55,4.86,4.86,0,0,0,.66-.91,5.56,5.56,0,0,0,.53-1.23l.17,0Z' transform='translate(0 -2.27)'/><path class='cls-1' d='M80.47,28.76a.83.83,0,0,0-.64-.23h-5v-.38H75a.53.53,0,0,0,.41-.23,1,1,0,0,0,.12-.5V20.47c0-.45-.18-.7-.54-.74l-.12,0v-.38h3.7v.38l-.14,0A.6.6,0,0,0,78,20a.83.83,0,0,0-.13.49v6.4a.91.91,0,0,0,.19.67.82.82,0,0,0,.58.18,1.36,1.36,0,0,0,.65-.19A2.9,2.9,0,0,0,80,27a4.86,4.86,0,0,0,.66-.91,5.56,5.56,0,0,0,.53-1.23l.17,0Z' transform='translate(0 -2.27)'/></g></g></svg>";


  page += "<form action='/' method='POST'>";
  page += "<br><label for='donotshow'>" + String(NORMAL_LIGHTS) + "</label> <input type='checkbox' name='donotshow' id='donotshow' " + String((doNotShow) ? "checked" : "") + " style='float: right;'><br>";
  TRACE();
  DUMP(doNotShow);

  page += "<p>" + String(TEXT_LETTER_DURATION) + String(letterDuration) + " (ms)</p>";
  page += "<input type='range' min='60' max='" + String(LETTER_DURATION * 2) + "' value='" + String(letterDuration) + "' name='letterduration' class='slider'>";


  page += "<p>" + String(TEXT_PAUSE_DURATION)  + String(pauseDuration) + " (ms)</p>";
  page += "<input type='range' min='0' max='" + String(PAUSE_DURATION * 2) + "' value='" + String(pauseDuration) + "' name='pauseduration' class='slider'><br><br>";


  page += "<p>" + String(NORMAL_LIGHTS_DURATION)  + String(maxNormalDuration / 60000) + " (mins)</p>";
  page += "<input type='range' min='60000' max='" + String(MAX_NORMAL_DURATION * 2) + "' value='" + String(maxNormalDuration) + "' name='maxnormalduration' class='slider'><br><br>";

  page += "<hr>";

  page += "<p>" + String(TYPE_MESSAGE) + "</p>";
  page += "<textarea name='message' rows=\"5\" cols=\"50\">" + String(message)  + "</textarea>";

  page += "<p>" + String(BRIGHTNESS_TEXT) + " : " + String(brightness) + "</p>";
  page += "<input type='range' min='1' max='254' value='" + String(brightness) + "' name='brightness' class='slider'><br><br>";

  page += "<label for='originalcolor'>" + String(ORIGINAL_COLORS) + "</label> <input type='checkbox' name='originalcolor' id='originalcolor' " + String((originalColor) ? "checked" : "") + " style='float: right;'><br><br>";

  page += "<br><br><button type='submit'>" + String(SUBMIT) + "</button>";
  page += "</body>";

  page += "<style>";
  page += "body { background-color: #1b1b1b; font-family: Serif; Color: #FF0000; font-size: 18px; margin: 0; padding: 30px 10px 0; }";
  page += "svg { max-height: 100px; margin: 0 auto; display: block; }";
  page += "textarea { background: transparent; width: 100%; padding: 7px; border: 1px solid red; color: White; FastLED.show() ; font-size: 13px; border-radius: 0px; }";
  page += "button { background: transparent; width: 100%; padding: 7px; border: 0px none; background: red; font-weight: bold; font-family: inherit; color: #1b1b1b; text-transform: uppercase; font-size: 13px; border-radius: 0px; }";
  page += ".slider { width: 100%; margin: 0; }";
  page += "</style>";

  page += "<script>";
  page += "";
  page += "</script>";

  page += "</html>";
  return page;
}
String filterMessage(String message) {  // permit only alphabetical characters
  String filtered = "";

  for (int i = 0; i < message.length(); i++) {
    if ((message[i] >= 'a' && message[i] <= 'z') || (message[i] >= 'A' && message[i] <= 'Z') || message[i] == ' ') {
      filtered += message[i];
    }
  }
  DUMP(filtered);
  return filtered;
}

void setup(void) // main config
{
  Serial.begin(115200);
  WiFi.hostname(hostString);
  Serial.print("Hostname: ");
  Serial.println(WiFi.hostname());

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  if (!MDNS.begin(hostString)) { // setup mDNS
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  } else {
    Serial.println("mDNS responder started");
  }

  server.on("/", handleRoot); // Call the 'handleRoot' function when a client requests URI "/" (or just goes to the IP)

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

  //  Add service to MDNS - SD
  MDNS.addService("http", "tcp", 80);

  // following .setCorrection( TypicalLEDStrip ) is optional
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  randomSeed(analogRead(0));

}

void loop(void) {
  server.handleClient(); //Handle client requests
  turnOnAll();
  if (doNotShow == 0) { // if client has requested lights stay on all of the time
    flickerLeds(30);
    fill_solid( leds, NUM_LEDS, CRGB::Black);
    writeWord(message, letterDuration, pauseDuration);
    turnOnAll();
    flickerLeds(30);
    fill_solid( leds, NUM_LEDS, CRGB::Black);
    turnOnAll();
    for (marker = 0; marker <= (maxNormalDuration / 1000) - 1; marker++) {
      server.handleClient(); //Handle client requests
      DUMP(maxNormalDuration / 100);
      DUMP(marker);
      delay(maxNormalDuration / 600);
    }

    FastLED.show();
  }
}

void handleRoot() { // triggered when user hits submit on web page
  Serial.println("===Request received==");

  doNotShow = server.hasArg("donotshow");
  Serial.println("do not show : " + String(doNotShow));


  if (server.hasArg("message")) {
    message = filterMessage(server.arg("message"));
  }

  originalColor = server.hasArg("originalcolor");
  Serial.println("original color : " + String(originalColor));


  if (server.hasArg("letterduration")) {
    letterDuration = atoi(server.arg("letterduration").c_str());
  }

  if (server.hasArg("pauseduration")) {
    pauseDuration = atoi(server.arg("pauseduration").c_str());
  }

  if (server.hasArg("maxnormalduration")) {
    maxNormalDuration = atoi(server.arg("maxnormalduration").c_str());
  }

  if (server.hasArg("brightness")) {
    brightness = atoi(server.arg("brightness").c_str());
    FastLED.setBrightness(brightness);
    Serial.println("brightness : " + String(brightness));
  }

  server.send(200, "text/html", getPage() ); // now variables have been parsed from web and stored, refresh the web interface with these
  marker = 1000; // reset for loop integer in case submit was hit whilst within the for loop
  wordMark = 0;  // reset for loop integer in case submit was hit whilst within the for loop
}

void turnOnAll() // enable all valid leds on maximum brightness
{
  int stringLen = letterIndex.length() + 1;
  char char_array[stringLen];
  letterIndex.toCharArray(char_array, stringLen);
  for (int i = 0; i < stringLen - 1; i++)
  {
    if (String(char_array[i]) != "-" )
    {
      displayLetter(String(char_array[i]), 0);
      leds[i].maximizeBrightness();
    }

  }
  FastLED.show();
}


void writeWord(String theword, int letterDuration, int letterSpacing) // spell out the word
{
  DUMP(theword);
  int stringLen = theword.length() + 1;
  DUMP(stringLen);
  char char_array[stringLen];
  theword.toCharArray(char_array, stringLen);
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  for (wordMark = 0; wordMark < stringLen - 1; wordMark++)
  {
    displayLetter(String(char_array[wordMark]), letterDuration);
    Serial.println(String(char_array[wordMark]));
    delay(letterSpacing);
    server.handleClient();
  }

}

void displayLetter(String theLetter, int letterDuration) // set the colour of the letter to be displayed
{
  int lightIndex = letterIndex.indexOf(theLetter);
  int colorIndexValue = colorLetterIndex.indexOf(theLetter);
  uint32_t colorValue = colorIndex[colorIndexValue];

  if (originalColor == 1 ) {
    lightLED(lightIndex, colorValue, letterDuration);
  }
  else {
    lightLED(lightIndex,  CRGB::White , letterDuration); // considering check box on web for white only
  }
}

void lightLED(int ledIndex, uint32_t colorValue, int duration) // illuminate the configured led for the specified period
{
  // Turn the LED on, then pause
  leds[ledIndex] = colorValue;
  FastLED.show();
  if (duration != 0) {
    delay(duration);
    // Now turn the LED off, then pause
    leds[ledIndex] = CRGB::Black;
    FastLED.show();
    delay(pauseDuration);
  }
}

void flickerLeds(int numTimes) // calls the flicker subroutine
{
  if (doNotShow == 0 ) {
    for ( int i = 0; i < numTimes; i++)
    {
      flicker();
    }
  }
}

void flicker() { // cool flicker subroutine
  int random_bright = random(0, 254);
  int random_delay = random(10, 100);
  int randomFullLight = random(0, 2);
  int stringLen = letterIndex.length() + 1;
  char char_array[stringLen];
  letterIndex.toCharArray(char_array, stringLen);

  for (int i = 0; i < stringLen - 1; i++)
  {
    if ( String(char_array[i]) != "-"  ) {
      if ( randomFullLight == 0 )
      {
        leds[i].maximizeBrightness();
      }
      leds[i].fadeLightBy(random_bright);
    }
  }
  FastLED.show();
  delay(random_delay);
}
