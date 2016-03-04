/*********************************************************************
 This is an example for our Feather WIFI modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <adafruit_feather.h>
#include <adafruit_mqtt.h>
#include <adafruit_aio.h>

/* This sketch connect to the Adafruit IO sever io.adafruit.com
 * and update a defined PHOTOCELL_FEED every 5 second. 
 * It also follow ONOF_FEED to receive update from the AIO website
 *
 * To run this demo
 * 1. Change SSID/PASS
 * 2. Decide whether you want to use TLS/SSL or not (USE_TLS)
 * 3. Change CLIENTID, TOPIC, PUBLISH_MESSAGE, WILL_MESSAGE if you want
 * 4. Compile and run
 * 5. Use MQTT desktop client to connect to same sever and subscribe to the defined topic
 * to monitor the published message.
 */

#define WLAN_SSID         "yourSSID"
#define WLAN_PASS         "yourPass"

#define AIO_USERNAME      "...your AIO username (see https://accounts.adafruit.com)..."
#define AIO_KEY           "...your AIO key..."

// AdafruitAIO will auto append "username/feeds/" prefix to your feed
#define PHOTOCELL_FEED     "photocell"
#define ONOFF_FEED         "onoff"

// Connect using TLS/SSL or not
#define USE_TLS           1

// Uncomment to set your own ClientID, otherwise a random ClientID is used
//#define CLIENTID          "Adafruit Feather"

AdafruitAIO       aio(AIO_USERNAME, AIO_KEY);

AdafruitAIOFeed   photocell(&aio, PHOTOCELL_FEED);
AdafruitAIOFeed   onoff(&aio, ONOFF_FEED);

int value = 0;

/**************************************************************************/
/*!
    @brief  The setup function runs once when the board comes out of reset
*/
/**************************************************************************/
void setup()
{
  Serial.begin(115200);

  // Wait for the USB serial port to connect. Needed for native USB port only
  while (!Serial) delay(1);

  Serial.println("AIO Test Example\r\n");

  // Print all software versions
  Feather.printVersions();

  while ( !connectAP() )
  {
    delay(500); // delay between each attempt
  }

  // Connected: Print network info
  Feather.printNetwork();

  // Tell the MQTT client to auto print error codes and halt on errors
  aio.err_actions(true, true);

  // Set ClientID if defined
  #ifdef CLIENTID
  aio.clientID(CLIENTID);
  #endif

  Serial.print("Connecting to io.adafruit.com ... ");
  if ( USE_TLS )
  {
    aio.connectSSL(); // Will halted if an error occurs
  }else
  {
    aio.connect(); // Will halted if an error occurs
  }
  Serial.println("OK");

  // Set message handler for onoff feed
  onoff.setMessageHandler(onoff_mess_handler);

  // follow onoff state change
  onoff.follow();
}

/**************************************************************************/
/*!
    @brief  This loop function runs over and over again
*/
/**************************************************************************/
void loop()
{
  value = (value+1) % 100;

  Serial.print("Updating feed " PHOTOCELL_FEED " : ");
  Serial.print(value);
  photocell.print(value);
  Serial.println(" ... OK");

  delay(5000);
}

/**************************************************************************/
/*!
    @brief  Subscribe callback handler

    @param  topic_data  topic name's contents in byte array (not null terminated)
    @param  topic_len   topic name's length

    @param  mess_data   message's contents in byte array (not null terminated)
    @param  mess_len    message's length

    @note   'topic_data' and 'mess_data' are byte array without null-terminated
    like C-style string. Don't try to use Serial.print() directly, use the UTF8String
    datatype or Serial.write() instead.
*/
/**************************************************************************/
void onoff_mess_handler(char* topic_data, size_t topic_len, uint8_t* mess_data, size_t mess_len)
{
  // Use UTF8String class for easy printing of UTF8 data
  UTF8String utf8Topic(topic_data, topic_len);
  UTF8String utf8Message(mess_data, mess_len);

  // Print out topic name and message
  Serial.print("[ONOFF Feed] ");
  Serial.print(utf8Topic);
  Serial.print(" : ") ;
  Serial.println(utf8Message);  
}

/**************************************************************************/
/*!
    @brief  Connect to defined Access Point
*/
/**************************************************************************/
bool connectAP(void)
{
  // Attempt to connect to an AP
  Serial.print("Attempting to connect to: ");
  Serial.println(WLAN_SSID);

  if ( Feather.connect(WLAN_SSID, WLAN_PASS) )
  {
    Serial.println("Connected!");
  }
  else
  {
    Serial.printf("Failed! %s (%d)", Feather.errstr(), Feather.errno());
    Serial.println();
  }
  Serial.println();

  return Feather.connected();
}