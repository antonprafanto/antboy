#pragma once
#include <Arduino.h>
#include <AntBoy.h>
#include "AntOS_Theme.h"
#include "AntOS_Settings.h"
#include "AntOS_StatusBar.h"
#include "AntOS_Splash.h"
#include "AntOS_QuickSettings.h"
#include "AntOS_Launcher.h"

class AntOSClass {
public:
    AntOS_SettingsClass&      Settings      = AntOS_Settings;
    AntOS_StatusBarClass&     StatusBar     = AntOS_StatusBar;
    AntOS_SplashClass&        Splash        = AntOS_Splash;
    AntOS_QuickSettingsClass& QuickSettings = AntOS_QuickSettings;
    AntOS_LauncherClass&      Launcher      = AntOS_Launcher;

    void begin();
    void update();
    void requestRedraw();

private:
    bool _redrawPending = true;
};

extern AntOSClass AntOS;
