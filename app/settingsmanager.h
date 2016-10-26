#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "settingscontainer.h"

#include <QString>

class QXmlStreamReader;

class SettingsManager
{
public:
    SettingsManager();
    explicit SettingsManager(const QString &filePath);

    QString filePath() const;
    void setFilePath(const QString &filePath);

    bool loadSettings(const QString &filePath = QString());
    SettingsContainer settings() const;

private:
    bool loadTimeSettings(QXmlStreamReader &reader);
    bool loadDimentionSettings(QXmlStreamReader &reader);

    bool findNextStartElement(QXmlStreamReader &reader,
                              QString elementName,
                              Qt::CaseSensitivity cs = Qt::CaseInsensitive,
                              bool lookOneLevelElementsOnly = false);

private:
    QString _filePath;
    SettingsContainer _settings;
};

#endif // SETTINGSMANAGER_H
