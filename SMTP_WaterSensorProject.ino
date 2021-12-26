#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "xxxxxx"
#define WIFI_PASSWORD "xxxxxx"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "xxxxxx"//gonderici mail
#define AUTHOR_PASSWORD "xxxxxx"//gonderici sifresi

/* Recipient's email*/
#define RECIPIENT_EMAIL "xxxxxx"//alici mail

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

#define POWER_PIN  17 // ESP32 pin GIOP17 connected to sensor's VCC pin
#define led 23

//----------eklenecek
const int sensorpin1 = 36;
const int sensorpin2 = 39;
const int sensorpin3 = 34;
const int sensorpin4 = 35;
const int sensorpin5 = 32;

int value1 =0;
int value2 =0;
int value3 =0;
int value4 =0;
int value5 =0;

String degerString1="x";
String degerString2="x";
String degerString3="x";
String degerString4="x";
String degerString5="x";

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF
  pinMode(led, OUTPUT);


}

void loop(){
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value1 = analogRead(sensorpin1); // read the analog value1 from sensor
  value2 = analogRead(sensorpin2); // read the analog value1 from sensor
  value3 = analogRead(sensorpin3); // read the analog value1 from sensor
  value4 = analogRead(sensorpin4); // read the analog value1 from sensor
  value5 = analogRead(sensorpin5); // read the analog value1 from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("The water sensor value1: ");
  Serial.println(value1);
  Serial.println(value2);
  Serial.println(value3);
  Serial.println(value4);
  Serial.println(value5);
  if(value1||value2||value3||value4||value5==0){
    degerString1 = String(value1);
    degerString2 = String(value2);
    degerString3 = String(value3);
    degerString4 = String(value4);
    degerString5 = String(value5);

    digitalWrite(led, HIGH);
    smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("ALICI", RECIPIENT_EMAIL);

  /*Send HTML message*/
  String htmlMsg = 
  "     Sensör 1 değeri: "+degerString1+
  "<br> Sensör 2 değeri: "+degerString2+
  "<br> Sensör 3 değeri: "+degerString3+
  "<br> Sensör 4 değeri: "+degerString4+
  "<br> Sensör 5 değeri: "+degerString5;
  
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;


  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
    
  }else{
    digitalWrite(led, LOW);
  }

  delay(30000);
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");

  }}
