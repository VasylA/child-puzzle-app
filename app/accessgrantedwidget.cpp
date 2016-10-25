#include "accessgrantedwidget.h"

#include <QLabel>
#include <QLayout>
#include <QPalette>
#include <QRadialGradient>
#include <QPropertyAnimation>

AccessGrantedWidget::AccessGrantedWidget(QWidget *parent)
    : QWidget(parent),
      _backgroundAnimation(new QPropertyAnimation(this, "COLOR")),
      _colorChannel(150)
{
    initUi();

    connect(_backgroundAnimation, SIGNAL(finished()), SLOT(runAnimation()));
    runAnimation();
}

void AccessGrantedWidget::initUi()
{
    QLabel *accessLabel = new QLabel();
    accessLabel->setText("<p style='font-size:100px; color:green'>Access<br> Granted</p>");
    accessLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    accessLabel->setAlignment(Qt::AlignCenter);

    QLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(accessLabel);
    setLayout(centralLayout);

    updateUi();
}

void AccessGrantedWidget::runAnimation()
{
    if(_backgroundAnimation->state() == QAbstractAnimation::Running)
    {
        _backgroundAnimation->stop();
        return;
    }

    _backgroundAnimation->setStartValue(150);
    _backgroundAnimation->setDuration(2000);
    _backgroundAnimation->setEndValue(250);

    QAbstractAnimation::Direction newDirection = (_backgroundAnimation->direction() == QAbstractAnimation::Forward) ? QAbstractAnimation::Backward
                                                                                                                    : QAbstractAnimation::Forward;
    _backgroundAnimation->setDirection(newDirection);

    _backgroundAnimation->start();
}

void AccessGrantedWidget::updateUi()
{
    QPoint widgetCenter(width() / 2, height() / 2);

    QRadialGradient gradient;
    gradient.setColorAt(1, QColor(0, _colorChannel, 20, 255));
    gradient.setColorAt(0, QColor(180, 180, 180, 255));
    gradient.setRadius(width() / 2.0);
    gradient.setCenter(widgetCenter);
    gradient.setFocalPoint(widgetCenter);

    QPalette colorScheme(palette());
    colorScheme.setBrush(QPalette::Background, gradient);

    setAutoFillBackground(true);
    setPalette(colorScheme);
}

int AccessGrantedWidget::colorChannel() const
{
    return _colorChannel;
}

void AccessGrantedWidget::setColorChannel(int colorChannel)
{
    _colorChannel = colorChannel;

    updateUi();
}
