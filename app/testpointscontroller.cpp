#include "testpointscontroller.h"

#include <QTimer>

TestpointsController::TestpointsController()
{
    initTestpoints();


}

TestpointsController::~TestpointsController()
{
    _gpio4.unexport_gpio();
    _gpio5.unexport_gpio();
    _gpio6.unexport_gpio();
    _gpio17.unexport_gpio();
}

void TestpointsController::resetOutGpiosStatus()
{
    _gpio4.setval_gpio("0");
    _gpio17.setval_gpio("0");
}

void TestpointsController::sendPuzzleCompeteSignalToOutGpios()
{
    _gpio4.setval_gpio("1");
    _gpio17.setval_gpio("0");
}

void TestpointsController::sendPuzzleIncompeteSignalToOutGpios()
{
    _gpio4.setval_gpio("0");
    _gpio17.setval_gpio("1");
}

void TestpointsController::initTestpoints()
{
    _gpio4.set_gpionum("4");
    _gpio4.export_gpio();
    _gpio4.setdir_gpio("out");
    _gpio4.setval_gpio("0");

    _gpio17.set_gpionum("17");
    _gpio17.export_gpio();
    _gpio17.setdir_gpio("out");
    _gpio17.setval_gpio("0");

    _gpio5.set_gpionum("5");
    _gpio5.export_gpio();
    _gpio5.setdir_gpio("in");

    _gpio6.set_gpionum("6");
    _gpio6.export_gpio();
    _gpio6.setdir_gpio("in");
}

void TestpointsController::checkInGpioStatus()
{
    string gpio5Value;
    string gpio6Value;

    int readStatusCode = _gpio5.getval_gpio(gpio5Value);
    if (readStatusCode >= 0)
        readStatusCode = _gpio6.getval_gpio(gpio6Value);

    if (readStatusCode < 0)
    {
        QTimer::singleShot(STATUS_CHECK_PERIOD, this, SLOT(checkInGpioStatus()));
        return;
    }

    if ( (gpio5Value == "0") && (gpio5Value == "0"))
        emit appFreezeRequested();

    if ( (gpio5Value == "1") && (gpio5Value == "0"))
        emit signal1();

    if ( (gpio5Value == "1") && (gpio5Value == "1"))
        emit signal2();

    QTimer::singleShot(STATUS_CHECK_PERIOD, this, SLOT(checkInGpioStatus()));
}
