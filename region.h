#ifndef REGION_H
#define REGION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include <QVariant>

class Street;
class District;
class StreetList;

class Region : public QObject {
	Q_OBJECT
public:
	Region(QObject* parent = NULL);
	Region(const QString& name, QObject* parent = NULL);
	Region(const QVariantMap& props, QObject* parent = NULL);
	~Region();

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

	QString name() const;

	Q_INVOKABLE QStringList districts() const;
	Q_INVOKABLE void addDistrict(District* district);
	Q_INVOKABLE void removeDistrict(District* district);

public slots:
	void setName(QString arg);

	void check() const;

signals:
	void nameChanged(QString arg);

private:
	QString mName;
	QMap<QString, District*> mDistricts;
};

#endif // REGION_H
