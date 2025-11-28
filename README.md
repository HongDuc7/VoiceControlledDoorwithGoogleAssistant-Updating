- How it Works (The Architecture)
Because Google Assistant runs in the cloud, it cannot talk directly to your board. We need like a "middleman" (a server) to transmit the message.

- The Flow:
For constant: 
You: "Hey Google, Open the Door."

Step1: Google Assistant: Sends the command to a service called IFTTT (If This Then That).

Step2: IFTTT: Sends a signal to an IoT Cloud (like Adafruit IO or Blynk).

Step3: ESP32/ESP8266: Constantly listens to the Cloud. When it sees "OPEN", it activates the motor.

Step4: Hardware: The Servo motor rotates, or the Solenoid lock pulls back.
