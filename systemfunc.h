#ifndef SYSTEMFUNC_H
#define SYSTEMFUNC_H

#include <QObject>
#include <QTime>

class SystemFunc : public QObject
{
	Q_OBJECT
public:
	explicit SystemFunc(QObject *parent = 0);
	~SystemFunc();
public slots:
	void startTimer();
	int restartTimer();
	void processEvents() const;
	int elapsedTimer() const;
private:
	QTime mTimer;
};

#endif // SYSTEMFUNC_H
