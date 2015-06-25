#include "settings.h"

Settings::Settings()
{
}

Settings::~Settings()
{
}

QString Settings::section() const
{
	return mSettings.group();
}

void Settings::setSection(QString arg)
{
	if (mSettings.group() == arg)
		return;

	while (!mSettings.group().isEmpty()) mSettings.endGroup();
	mSettings.beginGroup(arg);

	emit sectionChanged(arg);
}

QVariant Settings::read(QString key)
{
	return mSettings.value(key);
}

void Settings::write(QString key, QVariant value)
{
	mSettings.setValue(key, value);
}

