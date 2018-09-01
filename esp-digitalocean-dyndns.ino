#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "RestClient.h"

ESP8266WiFiMulti WiFiMulti;

/*
  $ echo | openssl s_client -connect api.digitalocean.com:443 |& openssl x509 -fingerprint -noout
  SHA1 Fingerprint=9A:8E:2F:33:95:CA:72:4F:47:26:54:52:A4:E5:00:70:F0:BD:ED:85
*/
RestClient client = RestClient("api.digitalocean.com", 443, "9A 8E 2F 33 95 CA 72 4F 47 26 54 52 A4 E5 00 70 F0 BD ED 85");
RestClient clientPubIP = RestClient("ipv4bot.whatismyipaddress.com", 80);

/*
 * Variables Definitions
 */
 
/*
 * change "sample.com" for your domain name
 * change "1234567890" for your record ID on digital ocean
 */
String record = "house";
String urlPath = "/v2/domains/sample.com/records/1234567890";

String response;
String putData, putDataPrefix;

/*
 * Change TOKEN_ID for your token on Digital Ocean API
 */

String Header_auth = "Authorization: Bearer TOKEN_ID";
String Header_content_type = "application/json";

void setup() {
  Serial.begin(115200);
  delay(10);

/*
 * Change SSID_Network for you SSID and Password
 */ 
  WiFiMulti.addAP("ssid_network", "ssid_password");

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    String responsePubIP = "";
    int statusCodePubIP = clientPubIP.get("/", &responsePubIP);
    Serial.print("Status code from PubIP Server: ");
    Serial.println(statusCodePubIP);
    Serial.print("Response body from PubIP Server: ");
    Serial.println(responsePubIP);
    Serial.println("#############");


  /*      
   * "You can use std::string::c_str() function, which returns a pointer to a const char buffer"
   * https://stackoverflow.com/questions/31614364/arduino-joining-string-and-char
   */
     
    putDataPrefix = "{\"name\": \""+ record +"\", \"data\": \"" + responsePubIP + "\"}";
    const char *putData = putDataPrefix.c_str(); 
/*
    Serial.print("Public IP: ");
    Serial.println(responsePubIP);
*/
    Serial.print("PUT Data: ");
    Serial.println(putData);

    /*
     * Update DNS on DigitalOcean Networking API
     */
     
    response = "";

    const char *CharHeader_auth = Header_auth.c_str();
    const char *CharHeader_content_type = Header_content_type.c_str();
 
    client.setHeader(CharHeader_auth);
    client.setContentType(CharHeader_content_type);
    client.setSSL(1);

    
   /*
    * Library to do PUT request
    * https://github.com/DaKaZ/esp8266-restclient
    * 
    * 
    * 
    * Check if code bellow are in 
    * esp8266-restclient/RestClient.cpp
    * 
    * 207     if(body != NULL){
    * 208         request += "\r\n";
    * 209         request += String(body);
    * 210         request += "\r\n\r\n";
    * 211     }
    */

    int statusCode = client.put("/v2/domains/sample.com/records/1234567890", putData, &response);
    //int statusCode = client.put("/v2/domains/sample.com/records/1234567890", putData);

    Serial.print("Status code from server: ");
    Serial.println(statusCode);
    Serial.print("Response body from server: ");
    Serial.println(response);

    /*
     * Update each 10 minutes
     */
    delay(600000);
  }
}
