#include "io.h"
#include "config.h"
#include <Logger.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include "mqtt.h"

String io_status()
{
    LOG_TRACE("io_status");
    config_io_t iodata = config_io_get();
    StaticJsonDocument<400> resp;
    for (size_t i = 0; i < _IO_COUNT; i++)
    {
        if (iodata.gpio[i].func != IO_UNUSED)
        {
            JsonObject jobj = resp.createNestedObject();
            jobj["io"] = iodata.gpio[i].label.c_str();
            jobj["val"] = io_fetch(i);
            switch (iodata.gpio[i].func)
            {
            case IO_READONLY:
                jobj["mode"] = "ro";
                break;
            case IO_READWRITE:
                jobj["mode"] = "rw";
                break;
            case IO_WRITEONLY:
                jobj["mode"] = io_valid_pin(iodata.gpio[i].map) ? "rw" : "wo";
                break;
            }
        }
    }
    String respStr;
    serializeJson(resp, respStr);
    return respStr;
}

uint8_t io_fetch(ioindex_t pin)
{
    LOG_TRACE("io_fetch");
    config_gpio_t gpio = config_gpio_get(pin);
    if (io_valid_pin(gpio.map))
    {
        pin = gpio.map;
    }
    uint8_t val = digitalRead(pin);
    LOGF_TRACE("digitalRead(%d):%d", pin, val);
    if (gpio.invert)
        val = (val == HIGH) ? LOW : HIGH;
    return val;
}

bool io_update(ioindex_t pin, uint8_t val, bool persist, bool publish)
{
    LOGF_TRACE("io_update(%d,%d,%d)", pin, val, persist);
    config_gpio_t gpio = config_gpio_get(pin);
    if (gpio.func == IO_READONLY)
        return false;
    if (gpio.toggle)
        val = HIGH;
    if (gpio.invert)
        val = (val == HIGH) ? LOW : HIGH;
    LOGF_TRACE("digitalWrite(%d,%d)", pin, val);
    digitalWrite(pin, val);
    if (gpio.persist)
    {
        gpio.value = val;
        config_gpio_set(pin, gpio);
        if (persist)
            config_io_commit();
    }
    if (publish)
    {
        mqtt_send(String(gpio.value), String("io/") + gpio.label);
    }
    return true;
}

bool io_update(String status)
{
    LOG_TRACE("io_update(status)");
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, status);
    if (error)
        return false;
    JsonArray jstat = doc.to<JsonArray>();
    for (size_t i = 0; i < jstat.size(); i++)
    {
        if (!jstat[i].is<JsonObject>())
            continue;

        JsonObject jobj = jstat[i].as<JsonObject>();
        unsigned int gpioindex = config_gpio_index(jobj["io"]);

        if (gpioindex < 0)
            continue;

        io_update(gpioindex, jobj["val"], false);
    }
    config_io_commit();
    return true;
}

void io_setup()
{
    LOG_TRACE("io_setup");
    config_io_t cfg = config_io_get();
    for (ioindex_t i = 0; i < _IO_COUNT; i++)
    {
        io_setup(i);
    }
}

void io_setup(ioindex_t pin)
{
    config_gpio_t gpio = config_gpio_get(pin);
    switch (gpio.func)
    {
    case IO_READONLY:
        pinMode(pin, INPUT);
        break;
    case IO_READWRITE:
    case IO_WRITEONLY:
        pinMode(pin, OUTPUT);
        if (gpio.persist)
        {
            LOGF_TRACE("Set GPIO%d to %d", pin, gpio.value);
            io_update(pin, gpio.value);
        }
    default:
        break;
    }
}

bool io_toggle(ioindex_t pin, bool publish)
{
    LOG_TRACE("io_toggle(" + String(pin) + ")");
    int8_t val = io_fetch(pin);
    int8_t tval = val == HIGH ? LOW : HIGH;
    LOG_TRACE("val:" + String(val) + ", tval:" + String(tval));
    return io_update(pin, tval, true, publish);
}

bool io_valid_pin(ioindex_t pin)
{
    return pin >= _IO_STARTPIN && pin <= _IO_ENDPIN;
}