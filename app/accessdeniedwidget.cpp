#include "accessdeniedwidget.h"

#include <QLabel>
#include <QLayout>
#include <QPalette>
#include <QTimer>

const QString AccessDeniedWidget::labelText = "Access Denied";

AccessDeniedWidget::AccessDeniedWidget(QWidget *parent)
    : QWidget(parent),
      _accessLabel(new QLabel),
      _isWarningColor(false),
      _textSize(120)
{
    initUi();

    runAnimation();
}

void AccessDeniedWidget::initUi()
{
    _accessLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _accessLabel->setAlignment(Qt::AlignCenter);
    _accessLabel->setWordWrap(true);

    QLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(_accessLabel);
    setLayout(centralLayout);
}

void AccessDeniedWidget::updateLabel()
{
    QString htmlText = QString("<p style='font-size:%0px'><b>%1</b></p>")
            .arg(_textSize)
            .arg(labelText);

    _accessLabel->setText(htmlText);
}

void AccessDeniedWidget::runAnimation()
{
    _isWarningColor = !_isWarningColor;

    QColor textColor = (_isWarningColor) ? Qt::black : Qt::red;
    QColor bgColor = (_isWarningColor) ? Qt::red : Qt::white;

    QPalette colorScheme(palette());
    colorScheme.setBrush(QPalette::Background, bgColor);
    colorScheme.setBrush(QPalette::WindowText, textColor);

    setAutoFillBackground(true);
    setPalette(colorScheme);

    _accessLabel->setAutoFillBackground(true);
    _accessLabel->setPalette(colorScheme);

    updateLabel();

    QTimer::singleShot(300, this, SLOT(runAnimation()));
}

int AccessDeniedWidget::textSize() const
{
    return _textSize;
}

void AccessDeniedWidget::setTextSize(int textSize)
{
    _textSize = textSize;

    updateLabel();
}
