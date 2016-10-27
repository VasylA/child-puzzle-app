#include "settingsmanager.h"

#include <QFile>
#include <QXmlStreamReader>

SettingsManager::SettingsManager()
    : _filePath("")
{
}

SettingsManager::SettingsManager(const QString &filePath)
    : _filePath(filePath)
{
}

QString SettingsManager::filePath() const
{
    return _filePath;
}

void SettingsManager::setFilePath(const QString &filePath)
{
    _filePath = filePath;
}

bool SettingsManager::loadSettings(const QString &filePath)
{
    if (!filePath.isEmpty())
        setFilePath(filePath);

    QFile file(_filePath);
    QXmlStreamReader reader;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    reader.setDevice(&file);

    if (!findNextStartElement(reader, "settings"))
        return false;

    bool isSuccess = true;

    if (!(reader.isStartElement() && reader.name() == "settings"))
        return false;

    while (!reader.atEnd())
    {
        reader.readNext();
        if (!reader.isStartElement())
            continue;

        if (reader.name() == "timing")
        {
            isSuccess &= loadTimeSettings(reader);
        }
        else if (reader.name() == "dimentions")
        {
            isSuccess &= loadDimentionSettings(reader);
        }
    }

    file.close();
    return isSuccess;
}

SettingsContainer SettingsManager::settings() const
{
    return _settings;
}

bool SettingsManager::loadTimeSettings(QXmlStreamReader &reader)
{
    if (!(reader.isStartElement() && reader.name() == "timing"))
        return false;

    QXmlStreamAttributes attributes = reader.attributes();

    int gameTimerSecondsPeriod = 0;
    int resetTimeSecondsPeriod = 0;

    if (attributes.hasAttribute("gameTime"))
        gameTimerSecondsPeriod = attributes.value("gameTime").toString().toInt();

    if (attributes.hasAttribute("resetTime"))
        resetTimeSecondsPeriod = attributes.value("resetTime").toString().toInt();

    const int millisecondsPerSecond = 1000;

    if (gameTimerSecondsPeriod > 0)
        _settings.gameTimerPeriod = gameTimerSecondsPeriod * millisecondsPerSecond;

    if (resetTimeSecondsPeriod > 0)
        _settings.gameResetPeriod = resetTimeSecondsPeriod * millisecondsPerSecond;

    return true;
}

bool SettingsManager::loadDimentionSettings(QXmlStreamReader &reader)
{
    if (!(reader.isStartElement() && reader.name() == "dimentions"))
        return false;

    QXmlStreamAttributes attributes = reader.attributes();

    int rowsCount = 0;
    int columnsCount = 0;

    if (attributes.hasAttribute("rowsCount"))
        rowsCount = attributes.value("rowsCount").toString().toInt();

    if (attributes.hasAttribute("columnsCount"))
        columnsCount = attributes.value("columnsCount").toString().toInt();

    if (rowsCount > 0)
        _settings.rowCount = rowsCount;

    if (columnsCount > 0)
        _settings.columnCount = columnsCount;

    return true;
}

bool SettingsManager::findNextStartElement(QXmlStreamReader &reader, QString elementName, Qt::CaseSensitivity cs, bool lookOneLevelElementsOnly)
{
    while (!reader.atEnd())
    {
        if (reader.readNextStartElement())
        {
            if (reader.name().compare(elementName,cs) == 0)
                return true;

            if (lookOneLevelElementsOnly)
                reader.skipCurrentElement();
        }
    }
    return false;
}
