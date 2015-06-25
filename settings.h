#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings: public QObject
{
	Q_OBJECT
public:
	Settings();
	~Settings();

	Q_PROPERTY(QString section READ section WRITE setSection NOTIFY sectionChanged)

	QString section() const;

public slots:
	void setSection(QString arg);

	QVariant read(QString key);
	void write(QString key, QVariant value);

signals:
	void sectionChanged(QString arg);

private:
	QSettings mSettings;
};

#endif // SETTINGS_H
