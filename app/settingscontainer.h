#ifndef SETTINGSCONTAINER
#define SETTINGSCONTAINER

struct SettingsContainer
{
    SettingsContainer()
        : rowCount(2),
          columnCount(3),
          gameTimerPeriod(20000),  //milliseconds
          gameResetPeriod(10000)   //milliseconds
    {}

    int rowCount;
    int columnCount;
    int gameTimerPeriod;
    int gameResetPeriod;
};

#endif // SETTINGSCONTAINER

