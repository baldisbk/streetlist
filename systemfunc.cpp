#include "systemfunc.h"

#include <QCoreApplication>

SystemFunc::SystemFunc(QObject *parent) : QObject(parent) {}

SystemFunc::~SystemFunc() {}

void SystemFunc::startTimer()
{
	mTimer.start();
}

int SystemFunc::restartTimer()
{
	return mTimer.restart();
}

void SystemFunc::processEvents() const
{
	QCoreApplication::processEvents();
}

int SystemFunc::elapsedTimer() const
{
	return mTimer.elapsed();
}
