/** @file main.cpp
 *  @brief Application for 6 switch relay system
 */

/* --- Standard Includes --- */
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdarg.h>

#include <si/shunyaInterfaces.h>


/* RapidJSON Includes */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include "log_error/log_error.h"

#define SWITCH1 3
#define SWITCH2 4
#define SWITCH3 5
#define SWITCH4 7
#define SWITCH5 8
#define SWITCH6 10

#define DEVICE_ID 100
#define MQTT_MSG_READ_FAIL 0
#define MQTT_MSG_READ_SUCCESS 1

#define SYS_CMD_BUFSIZE 1024

#define CONN_CONFIG_FILE "/etc/shunya/conn.config"
#define JSON_MAX_READ_BUF_SIZE 65536

using namespace std;
using namespace rapidjson;

/* Global variables */

/* Mqtt object */
mqttObj mqtt;

/* JSON documents */
rapidjson::Document keepAlive;
rapidjson::Document mqttMesg;
rapidjson::Document connConfig;

/* Status for MQTT message read */
int8_t mqttMesgReadStatus = MQTT_MSG_READ_FAIL;

/* Keep count of time for keep alive messages */
unsigned int prev_time = 0;

/* Internet connectivity status */
int8_t internetStatus = 0;


/**
 * @brief JSON document to JSON string
 *
 * @param doc JSON doc
 * @return const char* JSON string
 */
const char *jsonDoc2Text()
{
    rapidjson::StringBuffer buffer;
    buffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    keepAlive.Accept(writer);
    return strdup( buffer.GetString() );
}

/**
 * @brief Initialize the Keep alive JSON document 
 *          which will get sent to the mqtt topic
 * 
 */
void initKeepAliveJson()
{
    /* Declare the First JSON object */
    keepAlive.SetObject();
    /* Add object as a member to the JSON  document `keepAlive`*/
    keepAlive.AddMember("deviceId", "DEVICE_ID", keepAlive.GetAllocator());
    keepAlive.AddMember("status", "dead", keepAlive.GetAllocator());
}

/* ------------------------------------------------ */
/*            Connection Section Starts             */
/* ------------------------------------------------ */

/**
 * @brief Runs shell commands and returns the output of the shell command
 * 
 * @param out command output
 * @param cmd shell command
 * @param ... 
 * @return int8_t 1 on Success and 0 on failure
 */
int8_t runSysCmd(char *out, char *cmd, ...)
{
    va_list ap;
    char cmdFull[4096];
    va_start(ap, 1);        //msg=1
    vsnprintf(cmdFull, sizeof(cmdFull), cmd, ap);
    va_end(ap);
    FILE *result = popen(cmdFull, "r" );

    if (result == 0) {
        fprintf( stderr, "Could not execute\n" );
        Logger("Could not execute system command");
        return 1;
    }

    char buf[SYS_CMD_BUFSIZE];

    while (fgets(buf, SYS_CMD_BUFSIZE, result)) {
        fprintf(stdout, "%s", buf);
        sprintf(out, "%s", buf);
    }

    pclose(result);
    return 0;
}

/**
 * @brief Parse Connection config file
 * 
 * @return int8_t 0 on success and -1 on failure
 */
int8_t parseConnConfig()
{
    int8_t ret = -1;
    /* Open the example.json file in read mode */
    FILE *fp = fopen(CONN_CONFIG_FILE, "rb");

    if (fp != NULL) {
        /* Declare read buffer */
        char readBuffer[JSON_MAX_READ_BUF_SIZE];
        /* Declare stream for reading the example stream */
        FileReadStream frstream(fp, readBuffer, sizeof(readBuffer));
        /* Parse example.json and store it in `d` */
        ParseResult ok = connConfig.ParseStream(frstream);

        /* This code always returns empty document evenn though the document is not empty
         * Hence I have made a quick fix which will skip the error.
         */
        if (!ok) {
            fprintf(stderr,"Error: JSON file Invalid!. Please check if the JSON file  \"%s \" is valid. \n \t Reason: %s Error code: (%u)",CONN_CONFIG_FILE,GetParseError_En(ok.Code()), ok.Offset());
            Logger("Error: JSON file Invalid!. Please check the Json File");

        } else {
            ret = 0;
        }

    } else {
        fprintf(stderr,"Error Reading JSON config file: %s", strerror(errno));
        Logger("Error Reading JSON config file");
    }

    /* Close the example.json file*/
    fclose(fp);
    return ret;
}

/**
 * @brief Checks the Internet connection
 * 
 * @return int8_t 1 on Success 
 * @return int8_t 0 on Failure 
 */
int8_t checkInternet()
{
    int8_t rc = 1;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "www.google.com");

        while ((res = curl_easy_perform(curl)) != CURLE_OK) {
            switch (res) {
            case CURLE_COULDNT_CONNECT:
            case CURLE_COULDNT_RESOLVE_HOST:
            case CURLE_COULDNT_RESOLVE_PROXY:
                break;
                rc = 0;

            default:
                rc = 0;
            }
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
        return rc;
    }
    return rc;
}


/**
 * @brief Connects to LAN
 * 
 * @return int8_t 
 */
int8_t connectToLAN()
{
    char outpuBuf[4096];
    int8_t rc;
    fprintf(stdout, "Attempting to connect to LAN DHCP mode ..");
    /* Get MAC address */
    rc = runSysCmd(outpuBuf, "ip addr show %s | grep \"link/ether\b\" | awk '{print $2}' | cut -d/ -f1","eth0");

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to get MAC address.");
        Logger("Error! Unable to get MAC address.");
    }

    /* IF MAC address empty then Error */
    if (strcmp("", outpuBuf) == 0) {
        fprintf(stderr, "Error! LAN interface not found.");
        Logger("Error! LAN interface not found.");

    } else {
        fprintf(stdout, "LAN MAC Address: %s", outpuBuf);
    }

    memset(outpuBuf, 0x00, sizeof(outpuBuf));
    rc = runSysCmd(outpuBuf, "ip addr show %s | grep \"inet\b\" | awk '{print $2}' | cut -d/ -f1","eth0");

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to get IP address of LAN");
        Logger("Error! Unable to get IP address of LAN");
    }

    if (strcmp("", outpuBuf) == 0) {
        fprintf(stderr, "Error! LAN not connected.");
        Logger("Error! LAN not connected.");

    } else {
        fprintf(stdout, "LAN IP Address: %s", outpuBuf);
    }

    /*30 sec delay*/
    delay(30*1000);

    if ( checkInternet() != 1) {
        fprintf(stderr, "Error! Cannot connect to Internet.");
        Logger("Error! Cannot connect to Internet.");
        return 0;
    }

    return 1;
}

/**
 * @brief Connects to wifi by reading the /etc/shunya/conn.json file
 * 
 * @return int8_t 
 */
int8_t connectToWifi()
{
    if (parseConnConfig() < 0){
        fprintf(stderr, "Error! Failed to parse Connection config. Exiting..");
        Logger("Error! Failed to parse Connection config. Exiting..");
        exit(1);
    }

    char outpuBuf[4096];
    int8_t rc;
    fprintf(stdout, "Attempting to connect to Wifi DHCP mode ..");
    /* Get MAC address */
    rc = runSysCmd(outpuBuf, "ip addr show %s | grep \"link/ether\b\" | awk '{print $2}' | cut -d/ -f1","wlan0");

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to get MAC address.");
        Logger("Error! Unable to get MAC address.");
    }

    /* IF MAC address empty then Error */
    if (strcmp("", outpuBuf) == 0) {
        fprintf(stderr, "Error! Wifi interface not found.");
        Logger("Error! Wifi interface not found.");
        return 0;

    } else {
        fprintf(stdout, "Wifi MAC Address: %s", outpuBuf);
    }

    memset(outpuBuf, 0x00, sizeof(outpuBuf));
    rc = runSysCmd(outpuBuf, "ip addr show %s | grep \"inet\b\" | awk '{print $2}' | cut -d/ -f1","wlan0");

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to get IP address of Wifi");
        Logger("Error! Unable to get IP address of Wifi");
        return 0;
    }

    if (strcmp("", outpuBuf) == 0) {
        fprintf(stderr, "Error! Wifi not connected.");
        Logger("Error! Wifi not connected.");
        return 0;

    } else {
        fprintf(stdout, "Wifi IP Address: %s", outpuBuf);
    }

    memset(outpuBuf, 0x00, sizeof(outpuBuf));
    rc = runSysCmd(outpuBuf, "nmcli device wifi connect %s password %s",connConfig["wifi"]["ssid"].GetString(),connConfig["wifi"]["password"].GetString());

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to connect to Wifi");
        Logger("Error! Unable to connect to Wifi");
        return 0;
    }

    char gateway[4096];
    char net[4096];
    memset(outpuBuf, 0x00, sizeof(outpuBuf));
    rc = runSysCmd(outpuBuf, "route -n | grep -i -m 1 'eth0' | awk '{print $1}'");

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to set Wifi route");
        Logger("Error! Unable to set Wifi route");
        return 0;
    }

    if (strcmp("", outpuBuf) == 0) {
        fprintf(stderr, "Error! Wifi not connected.");
        Logger("Error! Wifi not connected.");
        return 0;

    } else {
        strcpy(net,outpuBuf);
    }

    memset(outpuBuf, 0x00, sizeof(outpuBuf));
    rc = runSysCmd(outpuBuf, "route -n | grep -i -m 1 'eth0' | awk '{print $2}'");

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to set Wifi route");
        Logger("Error! Unable to set Wifi route");
        return 0;
    }

    if (strcmp("", outpuBuf) == 0) {
        fprintf(stderr, "Error! Wifi not connected.");
        Logger("Error! Wifi not connected.");
        return 0;

    } else {
        strcpy(gateway,outpuBuf);
    }

    memset(outpuBuf, 0x00, sizeof(outpuBuf));
    rc = runSysCmd(outpuBuf, "route del -net %s gw %s eth0", net, gateway);

    if (rc != 0) {
        fprintf(stderr, "Error! Unable to set Wifi route");
        Logger("Error! Unable to set Wifi route");
        return 0;
    }

    /*30 sec delay*/
    delay(30*1000);

    if (checkInternet() != 1) {
        fprintf(stderr, "Error! Cannot connect to Internet.");
        Logger("Error! Cannot connect to Internet.");
        return 0;
    }

    return 1;
}

/**
 * @brief Connects to Internet
 * 
 * 1. First attempts LAN connection DHCP
 * 2. Then attempts Wifi connection 
 * 
 * @return int8_t 
 */
int8_t connectToInternet()
{
    int8_t rc;
    rc = connectToLAN();

    if (rc != 1) {
        delay(1000);
        rc = connectToWifi();

        if (rc != 1) {
            return 0;
        }
    }

    return 1;
}
/* ------------------------------------------------ */
/*            Connection Section Ends               */
/* ------------------------------------------------ */

/**
 * @brief MQTT subscribe callback
 * 
 * @param topicLen 
 * @param topic 
 * @param msgLen 
 * @param msg 
 */
void processMqttMessages(int topicLen, char *topic, int msgLen, char *msg)
{
    /* Parse JSON message */
    StringStream mqttMsg(msg);
    ParseResult ok = mqttMesg.ParseStream(mqttMsg);

    if (!ok) {
        fprintf(stderr, "Error: mqtt Message Invalid!. \n \t Reason: %s Error code: (%u)",GetParseError_En(ok.Code()), ok.Offset());
        Logger("Error: mqtt Message Invalid!");

        mqttMesgReadStatus = MQTT_MSG_READ_FAIL;

    } else {
        mqttMesgReadStatus = MQTT_MSG_READ_SUCCESS;
    }
}

/**
 * @brief Json write function for ON/OFF
 * 
 */
void json_write()
{   
    /* Declare the First JSON object */
    mqttMesg.SetObject();

    /* Add object as a member to the JSON  document `d`*/
    mqttMesg.AddMember("Switch1", "off", mqttMesg.GetAllocator());
    mqttMesg.AddMember("Switch2", "on", mqttMesg.GetAllocator());
    mqttMesg.AddMember("Switch3", "off", mqttMesg.GetAllocator());
    mqttMesg.AddMember("Switch4", "on", mqttMesg.GetAllocator());
    mqttMesg.AddMember("Switch5", "off", mqttMesg.GetAllocator());
    mqttMesg.AddMember("Switch6", "on", mqttMesg.GetAllocator());

    /* Open the output.json file in write mode */
    FILE *out = fopen("output.json", "wb");

    /* Declare write buffer */ 
    char writeBuffer[65536];

    /* Declare stream for writing the output stream */
    FileWriteStream os(out, writeBuffer, sizeof(writeBuffer));

    /* Make the output easier to read for Humans (Pretty) */
    PrettyWriter<FileWriteStream> writer(os);

    /* Write the JSON document `d` into the file `output.json`*/
    mqttMesg.Accept(writer);

    /* Close the output.json file*/
    fclose(out);
}

/**
 * @brief Json parse function for reading values ON/OFF
 * 
 */
void json_parse()
{
    /* Open the output.json file in read mode */
    FILE* fp = fopen("output.json", "rb"); 

    /* Declare read buffer */
    char readBuffer[65536];

    /* Declare stream for reading the example stream */
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    /* Parse example.json and store it in `rapidjson::Document mqttMesg` */
    mqttMesg.ParseStream(is);

    /* Close the output.json file*/
    fclose(fp);
}

/**
 * @brief Setup function, runs once 
 * 
 */
void setup()
{
    /* Initialize the GPIO pins */
    pinMode(SWITCH1, OUTPUT);
    pinMode(SWITCH2, OUTPUT);
    pinMode(SWITCH3, OUTPUT);
    pinMode(SWITCH4, OUTPUT);
    pinMode(SWITCH5, OUTPUT);
    pinMode(SWITCH6, OUTPUT);

    /* Set GPIO pins to LOW */
    digitalWrite(SWITCH1, LOW);
    digitalWrite(SWITCH2, LOW);
    digitalWrite(SWITCH3, LOW);
    digitalWrite(SWITCH4, LOW);
    digitalWrite(SWITCH5, LOW);
    digitalWrite(SWITCH6, LOW);

    delay(1000);

    /* Connect to internet */
    connectToInternet();
    delay(60*1000);
    internetStatus = checkInternet();

    /* Initialize MQTT */
    mqtt = newMqtt("mqtt");
    mqttConnect(&mqtt);
    mqttSetSubCallback(&mqtt, processMqttMessages);
    mqttSubscribe(&mqtt, "device/config");

    json_write();
    json_parse();

    /* Initialize Keep alive JSON*/
    initKeepAliveJson();
}

/**
 * @brief Loop function
 * 
 */
void loop()
{
    if (internetStatus == 1) 
    {
        /* Set GPIO if message read successfully */
        if (MQTT_MSG_READ_SUCCESS == mqttMesgReadStatus) 
        {
            if (strncmp("on", mqttMesg["Switch1"].GetString(), strlen("on"))) {
            digitalWrite(SWITCH1, HIGH);

            } else {
                digitalWrite(SWITCH1, LOW);
            }

            if (strncmp("on", mqttMesg["Switch2"].GetString(), strlen("on"))) {
            digitalWrite(SWITCH2, HIGH);

            } else {
                digitalWrite(SWITCH2, LOW);
            }

            if (strncmp("on", mqttMesg["Switch3"].GetString(), strlen("on"))) {
            digitalWrite(SWITCH3, HIGH);

            } else {
                digitalWrite(SWITCH3, LOW);
            }

            if (strncmp("on", mqttMesg["Switch4"].GetString(), strlen("on"))) {
            digitalWrite(SWITCH4, HIGH);

            } else {
                digitalWrite(SWITCH4, LOW);
            }

            if (strncmp("on", mqttMesg["Switch5"].GetString(), strlen("on"))) {
            digitalWrite(SWITCH5, HIGH);

            } else {
                digitalWrite(SWITCH5, LOW);
            }

            if (strncmp("on", mqttMesg["Switch6"].GetString(), strlen("on"))) {
            digitalWrite(SWITCH6, HIGH);

            } else {
                digitalWrite(SWITCH6, LOW);
            }
        }

        /* Keep alive messages every 5 mins */
        if ( (millis() - prev_time) > 5*60*1000) 
        { // 5(min)*60(secs)*1000(ms)
            keepAlive["status"].SetString("alive");
            mqttPublish(&mqtt, "device/alive", "%s", jsonDoc2Text());
            internetStatus = checkInternet();
            /* Update the previous sent time */
            prev_time = millis();
        }

    } 
    else 
    {
        connectToInternet();
        delay(60*1000);
        internetStatus = checkInternet();
    }
}


int main()
{
    /* Initalise the library */
    initLib();
    /* Setup the application */
    setup();

    /* loop */
    while (1) {
        loop();
    }

    return 0;
}
