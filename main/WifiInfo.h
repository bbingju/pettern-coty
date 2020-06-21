#ifndef WIFIINFO_H
#define WIFIINFO_H

#include <WiFi.h>
#include <Preferences.h>

class WifiInfo
{
public:
    String _ssid;
    String _password;
    bool _set_new_wifi;

    WifiInfo(): _ssid(""), _password("")
    {
        _pref.begin("wifiinfo", true);
        _ssid = _pref.getString("ssid", "");
        _password = _pref.getString("password", "");
        _set_new_wifi = _pref.getBool("set_new_wifi", false);
        _pref.end();
    }

    void save(String ssid, String password)
    {
        _ssid = ssid;
        _password = password;
        _set_new_wifi = true;

        _pref.begin("wifiinfo", false);
        _pref.putString("ssid", _ssid);
        _pref.putString("password", _password);
        _pref.putBool("set_new_wifi", _set_new_wifi);
        _pref.end();
    }

    void save(String ssid)
    {
        save(ssid, "");
    }

    void setup()
    {
        printf("%d\r\n", WiFi.status());
    }

private:
    Preferences _pref;
};

#endif /* WIFIINFO_H */
