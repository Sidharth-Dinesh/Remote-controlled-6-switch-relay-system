## Documentaion (section-wise)

### Purpose: Code to switch 6-channel relay on/off based on the JSON parsed.

#### 1. Connectivity:
  1. Connect to ethernet (LAN):
    * After establishing connection, we check using curl-functions.
    * If we fail to establish connection via LAN, we try to do so using WIFI.
  2. Connect to Wifi:
    * After establishing connection, we check using curl-functions.
    * If we fail to establish connection via WIFI too, we exit the process as internet is not available.

#### 2. MQTT:
  1. Send alive mqtt messages every 5mins:
    * We publish "Hello World!" message to topic: device/alive.
    * This is done to ensure that the process is alive and working.
  2. Receive messages from MQTT:
    * First we create a JSON file containing the switching-status that has to be done for each switch.
    * We process this JSON as an MQTT message under the topic: device/config.

#### 3. JSON parser:
  1. Parse JSON received from the MQTT:
    * The processed JSON needs to be parsed to read respective status values.

#### 4. 6 switch Relay:
  1. Switch relay on/off based on the JSON:
    * If we find status value under each switch as "ON", then we turn that switch on using digitalWrite function.
    * Else we keep it low, which is also the initial default value.
    * This is kept under an infinite loop.

Errors and Logging reporting has been implemented in the code as per requirements using appropriate functions.
