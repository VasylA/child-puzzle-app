#ifndef SETTINGSCONTAINER
#define SETTINGSCONTAINER

struct SettingsContainer
{
    SettingsContainer()
        : rowCount(2),
          columnCount(3),
          gameTimerPeriod(20),  //seconds
          gameResetPeriod(10)   //seconds
    {}

    int rowCount;
    int columnCount;
    int gameTimerPeriod;
    int gameResetPeriod;
};

#endif // SETTINGSCONTAINER

