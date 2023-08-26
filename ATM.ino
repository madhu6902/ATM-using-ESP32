#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>
#include <WiFi.h>
#include "HTTPClient.h"

const char* ssid = "XXXXXXX";
const char* password = "XXXXX";

#define BOT_ID "5961295166:AAHxj8qc4vJvIlVMB2ZDIQ_FA82778rOSXI"  
#define CHAT_ID "1490481782"  //replace with your telegram user ID

int bal = 15000;
String GOOGLE_SCRIPT_ID  = "AKfycbyY_jjUUWibUYPq4tEOuscAeaVSEKeHz1_76vtKPrfX8XoZj91AAfJIv2OkaYorBn20";

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_ID, client);

int bot_del = 1000;
unsigned long last_t_Bot;

const int ledPin = 2;
bool led_St = LOW;

String User_name;
String Password;

// Handling the new messages
void handleNewMessages(int New_messages) {
  Serial.println("Handling New Message");
  Serial.println(String(New_messages));

  for (int i=0; i<New_messages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the message received
    String user_text = bot.messages[i].text;
    Serial.println(user_text);

    String your_name = bot.messages[i].from_name;
    if (user_text == "/start") {
      String welcome = "Welcome, " + your_name + ".\n";
      welcome += "Send /username to request usename \n";
      welcome += "Send /password to request password \n";
      welcome += "Send /VERIFIED to request for transation once username and password is verified \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if(user_text == "/username")
    {
      bot.sendMessage(chat_id, "Enter username", "");
    }
    if(user_text == "/Tejas")
    {
      User_name = 
      bot.sendMessage(chat_id,"Username is entered", "");
    }

    if(user_text == "/password")
    {
      bot.sendMessage(chat_id, "Enter password","");
    }   
    if(user_text == "/12345678")
    {
      bot.sendMessage(chat_id, "VERIFIED","");
    }
    if (user_text == "/VERIFIED") {
      String welcome = "Welcome, " + your_name + ".\n";
      welcome += "Send /credit to request amount to be credited \n";
      welcome += "Send /debit to request amount to be debited \n";
      welcome += "Send /chk_bal to request for the balance amount \n";
      bot.sendMessage(chat_id, welcome, "");
    }
  
    if (user_text == "/debit") {
      led_St = HIGH;
      digitalWrite(ledPin, led_St);
      Serial.println("Enter denominations: ");
      delay(3000);
      while(Serial.available() == 0){
      }
      int count = Serial.read();
      Serial.write(count);
      debit_amt(count);
    }
    
    if (user_text == "/credit") {
      led_St = HIGH;
      digitalWrite(ledPin, led_St);
      Serial.println("Enter denominations: ");
      delay(3000);
      while(Serial.available() == 0){
      }
      int count = Serial.read();
      Serial.write(count);
      credit_amt(count);
    }

    if(user_text == "/chk_bal")
    {
      led_St = HIGH;
      digitalWrite(ledPin, led_St);
      Serial.println("The balance amount is:");
      Serial.println(bal);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, led_St);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > last_t_Bot + bot_del)  {
    int New_messages = bot.getUpdates(bot.last_message_received + 1);

    while(New_messages) {
      Serial.println("\nGot Response!");
      handleNewMessages(New_messages);
      New_messages = bot.getUpdates(bot.last_message_received + 1);
    }
    last_t_Bot = millis();
  }
}
bool username_flag = 0;
bool password_flag = 0;
void User_name_verify()
{
    if(User_name == Payload)
    {
        username_flag = 1;
    }
}

void Password_verify()
{
    if(Password == Payload)
    {
        pasword_flag = 1;
    }
}

void verification()
{
  if(username_flag==1 && password_flag==1)
  {
    Serial.println("Verified!!");
  }
}
void debit_amt(int num){
  int amt = num * 100;
  bal += amt;
  Serial.print("The amount has been debited.");
  sheet_update(amt, 0, bal);
}

void credit_amt(int amount){
  int amt = amount * 100;
  if(amt > bal){
    Serial.println("Insufficient Balance.");
    exit(0);
  }
  else{
  bal -= amount;
  Serial.print("\nThe amount has been credited.");
  }
  sheet_update(0, amt, bal);
}

void bal_check(){
  Serial.println("\nYour current balance is.. Rs.");
  Serial.println(bal);
}

void sheet_update(int amt1, int amt2, int amt3) {
if (WiFi.status() == WL_CONNECTED) {
String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"Debit=" + String(amt1) + "&Credit=" + String(amt2) + "&Total_Balance=" + String(amt3) + "&Date=" ;
Serial.print ("POST data to spreadsheet:");
Serial.println (urlFinal);
HTTPClient http;
http.begin (urlFinal.c_str());
http.setFollowRedirects (HTTPC_STRICT_FOLLOW_REDIRECTS);
int httpCode = http.GET();
Serial.print ("HTTP Status Code: ");
Serial.println (httpCode);
http.end();
}
delay(1000);
}
