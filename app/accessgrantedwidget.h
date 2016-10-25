#ifndef ACCESSGRANTEDWIDGET_H
#define ACCESSGRANTEDWIDGET_H

#include <QWidget>

class QPropertyAnimation;

class AccessGrantedWidget : public QWidget
{
    Q_OBJECT

public:
    Q_PROPERTY(qreal COLOR READ colorChannel WRITE setColorChannel)

    explicit AccessGrantedWidget(QWidget *parent = 0);

    int colorChannel() const;
    void setColorChannel(int colorChannel);

signals:

public slots:

private:
    void initUi();
    void updateUi();

private slots:
    void runAnimation();

private:
    QPropertyAnimation *_backgroundAnimation;
    int _colorChannel;

};

#endif // ACCESSGRANTEDWIDGET_H
