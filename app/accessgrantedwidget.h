#ifndef ACCESSGRANTEDWIDGET_H
#define ACCESSGRANTEDWIDGET_H

#include <QWidget>

class QLabel;
class QPropertyAnimation;

class AccessGrantedWidget : public QWidget
{
    Q_OBJECT

public:
    Q_PROPERTY(qreal COLOR READ colorChannel WRITE setColorChannel)

    explicit AccessGrantedWidget(QWidget *parent = 0);

    int colorChannel() const;
    void setColorChannel(int colorChannel);

    int textSize() const;
    void setTextSize(int textSize);

signals:

public slots:

private:
    void initUi();
    void updateUi();
    void updateLabel();

private slots:
    void runAnimation();

private:
    static const QString labelText;

    QLabel *_accessLabel;

    QPropertyAnimation *_backgroundAnimation;
    int _colorChannel;

    int _textSize;
};

#endif // ACCESSGRANTEDWIDGET_H
