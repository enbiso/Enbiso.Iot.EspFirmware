#ifndef app_view_activation_index_h
#define app_view_activation_index_h

#include <Arduino.h>

String view_activation_index = R"=====(
<html lang="en" class="route-index">

<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Setup IOT</title>
    <style>
        body {
    	margin: 30px;
        font-size: 14px;
    	font-family: system-ui;
    }
    .row {
     	display: flex;
        margin: 5px;
    }
    label {
    	margin-right: 20px;
        padding: 10px 0px;
        display: flex;
    	width: 50%;
    }
    input[type="text"],  input[type="password"] {
        padding:10px;
        font-size: 14px;
        width: 50%;
    }
    .radios {
        padding:10px;
        width: 50%;
    }
    button {
        padding:10px;
    	background-color: #0078e7;
    	color: white;
    	border: 0px;
        font-size: 16px;
    	margin: 10px auto;
    	width: 100%;
    }
  </style>
</head>

<body>
    <form method="POST" action="/">
        <div class="row">
            <label>Device Name</label>
            <input required type="text" name="name" placeholder="Device Name" />
        </div>
        <div class="row">
            <label>Access Password</label>
            <input required type="text" name="access" placeholder="Access Password" />
        </div>
        <div class="row">
            <label>WiFi SSID</label>
            <input required type="text" name="wifi_ssid" placeholder="WIFI SSID" />
        </div>
        <div class="row">
            <label>WiFi Password</label>
            <input type="password" name="wifi_password" placeholder="WIFI Password" />
        </div>
        <div class="row">
            <label>DHCP</label>
            <div class="radios">
                <input type="radio" name="dhcp" value="1" checked>&nbsp;Enable&nbsp;
                <input type="radio" name="dhcp" value="0">&nbsp;Disable
            </div>
        </div>
        <div id="dhcp-section">
            <div class="row">
                <label>IP Address</label>
                <input class="dchp-input" type="text" name="ip" placeholder="IP Address" />
            </div>
            <div class="row">
                <label>Subnet Mask</label>
                <input class="dchp-input" type="text" name="subnet" placeholder="Subnet Mask" />
            </div>
            <div class="row">
                <label>Default Gateway</label>
                <input class="dchp-input" type="text" name="gateway" placeholder="Gateway" />
            </div>
        </div>
        <div class="row">
            <label>Custom DNS</label>
            <div class="radios">
                <input type="radio" name="dns" value="1">&nbsp;Enable&nbsp;
                <input type="radio" name="dns" value="0" checked>&nbsp;Disable
            </div>
        </div>
        <div id="dns-section">
            <div class="row">
                <label>Primary DNS</label>
                <input class="dns-input" type="text" name="dns1" placeholder="Primary DNS" />
            </div>
            <div class="row">
                <label>Secondary DNS</label>
                <input class="dns-input" type="text" name="dns2" placeholder="Secondary DNS" />
            </div>
        </div>
        <div class="row">
            <button type="submit">Activate</button>
        </div>
    </form>
    <script>
        const dnsRadios = document.getElementsByName('dns');
        const dhcpRadios = document.getElementsByName('dhcp');
        function dnsChange(value) {
            const inputs = document.getElementsByClassName('dns-input');
            for (var j = 0; j < inputs.length; j++) {
                inputs[j].disabled = value === '0';
            }
            document.getElementById('dns-section').style.display = value === '0' ? 'none' : 'block';
        }
        function dhcpChange(value) {
            const inputs = document.getElementsByClassName('dchp-input');
            for (var j = 0; j < inputs.length; j++) {
                inputs[j].disabled = value === '1';
            }
            if (value === '0') {
                for (var j = 0, length = dnsRadios.length; j < length; j++) {
                    dnsRadios[j].checked = dnsRadios[j].value === '1';
                }
                dnsChange("1");
            }
            document.getElementById('dhcp-section').style.display = value === '1' ? 'none' : 'block';
        }
        for (var i = 0, length = dnsRadios.length; i < length; i++) {
            dnsRadios[i].addEventListener('change', function () {
                dnsChange(this.value);
            });
        }
        for (var i = 0, length = dhcpRadios.length; i < length; i++) {
            dhcpRadios[i].addEventListener('change', function () {
                dhcpChange(this.value);
            });
        }
        dnsChange("0");
        dhcpChange("1");
    </script>

    <body>

</html>
)=====";

#endif