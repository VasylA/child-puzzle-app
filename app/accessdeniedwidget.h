#ifndef ACCESSDENIEDWIDGET_H
#define ACCESSDENIEDWIDGET_H

#include <QWidget>

class QLabel;

class AccessDeniedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccessDeniedWidget(QWidget *parent = 0);

    int textSize() const;
    void setTextSize(int textSize);

signals:

public slots:

private:
    void initUi();
    void updateLabel();

private slots:
    void runAnimation();

private:
    static const QString labelText;

    QLabel *_accessLabel;
    bool _isWarningColor;

    int _textSize;
};

#endif // ACCESSDENIEDWIDGET_H
