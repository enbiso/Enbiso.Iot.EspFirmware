#ifndef app_controller_utils_h
#define app_controller_utils_h

#include "config.h"

void _update_network_from_web(config_network_t *data);
void _update_access_from_web(config_access_t *data);
void _update_mqtt_from_web(config_mqtt_t *data);
void _update_setup_from_web(config_setup_t *data);
void _update_gpio_from_web(config_gpio_t *data);

ioindex_t _fetch_iopin(String uri);

bool _check_auth();

#endif