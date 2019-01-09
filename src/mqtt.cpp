#include "mqtt.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "logger.h"
#include "cmd.h"

bool _mqtt_enabled = false;
config_mqtt_t _mqtt_cfg;
String _mqtt_outTopic;
String _mqtt_inTopic;
String _mqtt_clientId;

WiFiClient _wificlient;
PubSubClient _mqttclient(_wificlient);

void _callback(char *topic, byte *payload, unsigned int length);

void mqtt_setup()
{
    if (!config_mqtt_enabled())
        return;

    if (_mqttclient.connected())
        _mqttclient.disconnect();

    _mqtt_enabled = true;
    _mqtt_cfg = config_mqtt_get();
    _mqtt_clientId = config_name_get();
    _mqtt_inTopic = String("cmd/") + _mqtt_cfg.topic + String("/#");
    _mqtt_outTopic = String("res/") + _mqtt_cfg.topic;
    PRINTSTATUS("MQTT", _mqtt_cfg.server + " port " + _mqtt_cfg.port);
    _mqttclient.setServer(_mqtt_cfg.server.c_str(), _mqtt_cfg.port);
    _mqttclient.setCallback(_callback);
}

bool _tryconnect()
{
    if (_mqttclient.connected())
        return true;

    LOG_INFO("Attempting MQTT connection...");
    if (!_mqttclient.connect(_mqtt_clientId.c_str(), _mqtt_cfg.username.c_str(), _mqtt_cfg.password.c_str()))
    {
        LOG_ERROR("MQTT connection Failed, rc: " + _mqttclient.state());
        return false;
    }
    LOG_INFO("MQTT Connected");
    PRINTSTATUS("Topic IN", _mqtt_inTopic);
    PRINTSTATUS("Topic OUT", _mqtt_outTopic);
    mqtt_send(MQTT_ACK);
    _mqttclient.subscribe(_mqtt_inTopic.c_str());
    return true;
}

void mqtt_execute()
{
    if (!_mqtt_enabled || !_tryconnect())
        return;

    _mqttclient.loop();
}

bool mqtt_send(String payload, String domain)
{
    if (domain == "")
    {
        _mqtt_outTopic += "/";
        _mqtt_outTopic += domain;
    }
    return _mqttclient.publish(_mqtt_outTopic.c_str(), payload.c_str());
}

cmd_t _mqtt_parse_cmd(const char *topic, byte *payload)
{
    cmd_t cmd;
    char tmp[4][32];
    size_t count = sscanf(topic, "%[^'/']/%[^'/']/%[^'/']/%s", tmp[0], tmp[1], tmp[2], tmp[3]);
    if (count >= 4)
        cmd.domain = tmp[3];
    if (count >= 3)
        cmd.domain_type = tmp[2];
    if (count >= 2)
        cmd.topic_name = tmp[1];
    if (count >= 1)
        cmd.topic_cmd = tmp[0];
    count = sscanf((char *)payload, "%[^':']:%s", tmp[0], tmp[1]);
    if (count >= 2)
        cmd.params = tmp[1];
    if (count >= 1)
        cmd.command = tmp[0];
    return cmd;
}

void _callback(char *topic, byte *payload, size_t length)
{
    LOG_TRACE("MQTT Message arrived");
    payload[length] = '\0';
    cmd_t cmd = _mqtt_parse_cmd(topic, payload);
    PRINTSTATUS("Topic CMD", cmd.topic_cmd);
    PRINTSTATUS("Topic Name", cmd.topic_name);
    PRINTSTATUS("Domain Type", cmd.domain_type);
    PRINTSTATUS("Domain", cmd.domain);
    PRINTSTATUS("Command", cmd.command);
    PRINTSTATUS("Params", cmd.params);
    cmd_execute(cmd);
}