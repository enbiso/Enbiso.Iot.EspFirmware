#include <Arduino.h>
#include "config.h"
#include "webserver.h"
#include <Logger.h>
#include "mqtt.h"
#include "network.h"
#include "io.h"

bool reset_await();

void setup()
{
    logger_init();
    config_init();

    if (reset_await())
        config_reset();

    io_setup();
    network_setup();
    webserver_setup();
    mqtt_setup();
}

void loop()
{
    network_execute();
    webserver_execute();
    mqtt_execute();
}

bool reset_await()
{
    PRINTLN("Press any key to reset device...");
    long startTime = millis();
    while (!Serial.available())
    {
        long currentTime = millis();
        if ((startTime + 2000) <= currentTime)
            return false;
    }
    return true;
}
