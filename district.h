#ifndef DISTRICT_H
#define DISTRICT_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QVariant>

class Street;
class Region;
class StreetList;

class District : public QObject {
	Q_OBJECT
public:
	District(QObject* parent = NULL);
	District(const QString& name, QObject* parent = NULL);
	District(const QVariantMap& props, QObject* parent = NULL);
	~District();

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(Region* region READ region WRITE setRegion NOTIFY regionChanged)

	QString name() const;
	Region* region() const;

	Q_INVOKABLE QStringList streets() const;
	Q_INVOKABLE void addStreet(Street* street);
	Q_INVOKABLE void removeStreet(Street* street);

public slots:
	void setName(QString arg);
	void setRegion(Region* region);

	void check() const;

signals:
	void nameChanged(QString arg);
	void regionChanged(Region* arg);

private:
	QMap<QString, Street*> mStreets;
	QString mName;
	Region* mRegion;
};

#endif // DISTRICT_H
