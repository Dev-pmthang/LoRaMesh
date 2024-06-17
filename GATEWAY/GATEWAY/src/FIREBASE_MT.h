#ifndef FIREBASE_MT_H
#define FIREBASE_MT_H
#include <WiFi.h>
#include <FirebaseClient.h>

const char *API_KEY = "AIzaSyCG17AC0gxEOe7el3xL6l32DbSBCdUjPPg";
const char *USER_EMAIL = "phamthang900c@gmail.com";
const char *USER_PASSWORD = "MT08112002";

#define DATABASE_URL "lvtn-mt-default-rtdb.asia-southeast1.firebasedatabase.app"

void authHandler();
void printError(int code, const String &msg);
void printResult(AsyncResult &aResult);
void stopWiFiClient();

void FireBase_Init();
void sw_mode();
void sendString(String path, String value);
void pushJSon(String path, object_t json);

DefaultNetwork network(true); // initilize with boolean parameter to enable/disable network reconnection
UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);
NoAuth no_auth;

FirebaseApp app;
WiFiClient basic_client;
ESP_SSLClient ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client, getNetwork(network));
AsyncResult aResult_no_callback;
RealtimeDatabase Database;

object_t datanode1;
object_t datanode2;
object_t datanode3;
object_t error;

bool signupOK = false;
bool mode = true;

void FireBase_Init()
{
  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
  Serial.println("Initializing app...");

  ssl_client.setClient(&basic_client);
  ssl_client.setInsecure();
  ssl_client.setBufferSizes(4096, 1024);

  // initializeApp(aClient, app, getAuth(user_auth));
  initializeApp(aClient, app, getAuth(no_auth), aResult_no_callback);

  authHandler();
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

void sw_mode()
{
  mode = !mode;
  Serial.println("Mode " + String(mode));
  sendString("/MODE/", String(mode));
}


void stopWiFiClient()
{
  static unsigned long StopWiFiClientpreviousMillis;
  if (millis() - StopWiFiClientpreviousMillis > (1000 * 30))
  {
    StopWiFiClientpreviousMillis = millis();
    basic_client.stop();
    Serial.println("stopWiFiClient");
  }
}
void sendString(String path, String value)
{
  bool status = Database.set<String>(aClient, path, value);
  if (status)
    Serial.println("Set string is ok");
  else
    printError(aClient.lastError().code(), aClient.lastError().message());
}
void pushJSon(String path, object_t json)
{
  // Khoi tao bien de luu tru ten
  String name;

  // Push json len database
  name = Database.push<object_t>(aClient, path, json);

  // Kiem tra loi
  if (aClient.lastError().code() == 0)
    Firebase.printf("Push json is ok, name: %s\n", name.c_str());
  else
    printError(aClient.lastError().code(), aClient.lastError().message());
}

void authHandler()
{
  // Blocking authentication handler with timeout
  unsigned long ms = millis();
  while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
  {
    // This JWT token process required for ServiceAuth and CustomAuth authentications
    JWT.loop(app.getAuth());
    printResult(aResult_no_callback);
  }
}

void printResult(AsyncResult &aResult)
{
  if (aResult.isEvent())
  {
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
  }

  if (aResult.isDebug())
  {
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
  }

  if (aResult.isError())
  {
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
  }
}
void printError(int code, const String &msg)
{
  Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}
#endif
