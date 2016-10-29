#ifndef TESTPOINTSCONTROLLER_H
#define TESTPOINTSCONTROLLER_H

#include "gpio_class.h"

#include <QObject>

class TestpointsController : public QObject
{
    Q_OBJECT

public:
    TestpointsController(QObject *parent = 0);
    ~TestpointsController();

public slots:
    void resetOutGpiosStatus();
    void sendPuzzleCompeteSignalToOutGpios();
    void sendPuzzleIncompeteSignalToOutGpios();

signals:
    void initialAppStateRequested();
    void laserPassed();
    void laserFailed();

private:
    void initTestpoints();

private slots:
    void checkInGpioStatus();

private:
    GPIOClass _gpio4;
    GPIOClass _gpio5;
    GPIOClass _gpio6;
    GPIOClass _gpio17;

//    QTimer _statusCheckTimer;

    static const int STATUS_CHECK_PERIOD = 300;
};

#endif // TESTPOINTSCONTROLLER_H
