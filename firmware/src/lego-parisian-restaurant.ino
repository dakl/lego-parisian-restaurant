/*
 * Project lego-parisian-restaurant
 * Description: Control my LEGO Parisian Restaurant over http
 * Author: Daniel Klevebring
 * Date: 2017-09-17
 */

// ------------
// LEGO Parisian Restaurant Light Automation
// ------------

int time_to_live = 16777215;

int PINS[] = {D0, D1, D2, D3};
int INITIAL_STATES[] = {0, 0, 0, 0};

void reset_handler()
{
    // tell the world what we are doing
    Particle.publish("reset", "going down for reboot NOW!", time_to_live);
}

int getPinIndex(String command)
{
    // parse the relay number
    int pinIndex = command.charAt(1) - '0';
    // do a sanity check
    if (pinIndex < 0 || pinIndex > 3) return -1;
    else return pinIndex;
}

/*
command format: pinIndex,state
examples: `0,HIGH`, `3,LOW`
POST /v1/devices/{DEVICE_ID}/pin
# EXAMPLE REQUEST
curl https://api.particle.io/v1/devices/0123456789abcdef/pin \
-d access_token=123412341234 -d params=1,HIGH
*/
int pinControl(String command)
{
    // parse the relay number
    int pinIndex = getPinIndex(command);
    if (pinIndex == -1) return -1;
    int pin = PINS[pinIndex];

    // find out the state of the relay
    int state = 0;
    if (command.substring(2,6) == "HIGH") state = 1;
    else if (command.substring(2,5) == "LOW") state = 0;
    else return -1;

    // write to the appropriate relay
    digitalWrite(pin, state);

    return 1;
}

/*
command format: 1 (range 0 to 3)
POST /v1/devices/{DEVICE_ID}/state
# EXAMPLE REQUEST
curl https://api.particle.io/v1/devices/0123456789abcdef/state \
-d access_token=123412341234 -d params=1
*/
int pinState(String args)
{
    // parse the relay number
    int pinIndex = getPinIndex(args);
    if (pinIndex == -1) return -1;

    return digitalRead(PINS[pinIndex]);
}

void ready() {
    for(int k = 0; k < 5; k++){
        for (int i=0; i<arraySize(PINS); i++) {
            digitalWrite(PINS[i], HIGH);
        }
        delay(100);
        for (int i=0; i<arraySize(PINS); i++) {
            digitalWrite(PINS[i], LOW);
        }
        delay(100);
    }

    Particle.publish("booted", "particle-relay-hub is ready.", time_to_live);
}


void setup() {
    Particle.publish("booting", "Lego house lights are setting up.", time_to_live);

    // register the reset handler
    System.on(reset, reset_handler);

    for (int i=0; i<arraySize(PINS); i++) {
        pinMode(PINS[i], OUTPUT);
    }

    // register control function
    Particle.function("pin", pinControl);

    // register state function
    Particle.function("state", pinState);

    ready();
}

void loop() {}
