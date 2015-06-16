#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <QList>
#include <QString>

class Street;

enum TokenType {
	ttInvalid,
	ttOperation,
	ttNumber,
	ttVariable
};
enum OperationType {
	otMultiply = 0,
	otDivide,
	otPlus,
	otMinus,
	otOpen,
	otClose
};

struct Token {
	TokenType type;
	int value;
	Token(): type(ttInvalid) {}
	Token(TokenType t, int v): type(t), value(v) {}
	QString toString() const;
};

class TokenParser {
public:
	bool setExpr(const QString &exp);
	bool syntax();
	bool semantic();
	bool lexic();
	QString calculate(Street* street);
	QString errors() const;

private:
	QList<Token> tokens;
	QList<Token> polish;
	QString expression;
	QString error;
};

class MultiParser {
public:
	bool setExpr(const QString &exp);
	QString calculate(Street* street);
	QString errors() const;
private:
	QList<TokenParser> parsers;
	QString error;
	QString pattern;
};

#endif // TOKENPARSER_H
