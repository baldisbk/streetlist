#include "tokenparser.h"

#include <QStack>
#include "street.h"

#include <QDebug>

QString Token::toString() const {
	switch(type) {
	case ttOperation:
		switch(value) {
		case otPlus: return "+";
		case otMinus: return "-";
		case otMultiply: return "*";
		case otDivide: return "/";
		case otOpen: return "(";
		case otClose: return ")";
		default: return "?";
		}
	case ttNumber: return QString::number(value);
	case ttVariable: return "x"+QString::number(value);
	default: return "INV";
	}
}

bool TokenParser::setExpr(const QString &exp)
{
	expression = exp;
	tokens.clear();
	polish.clear();
	error.clear();
	return lexic() && syntax() && semantic();
}

bool TokenParser::syntax()
{
	if (!error.isEmpty())
		return false;
	bool literExp = true;
	int index = 0;
	foreach(const Token &token, tokens) {
		++index;
		if (literExp) {
			if (token.type == ttOperation) {
				if (token.value != otOpen) {
					error = "Operand expected at token " +
							QString::number(index);
					return false;
				}
			} else
				literExp = false;
		} else {
			if (token.type == ttOperation) {
				switch (token.value) {
				case otOpen:
					error = "Operand expected at token " +
							QString::number(index);
					return false;
				case otClose: break;
				default: literExp = true;
				}
			} else {
				error = "Operator expected at token " +
						QString::number(index);
				return false;
			}
		}
	}
	if (literExp) {
		error = "Operand expected at token " +
				QString::number(index);
		return false;
	}
	return true;
}

bool TokenParser::semantic()
{
	if (!error.isEmpty())
		return false;
	QStack<Token> stack;
	foreach (const Token& token, tokens) {
		if (token.type == ttOperation) {
			switch (token.value) {
			case otPlus:
			case otMinus:
				while(!stack.isEmpty() && stack.top().value <= otMinus)
					polish.append(stack.pop());
				stack.push(token);
				break;
			case otMultiply:
			case otDivide:
				while(!stack.isEmpty() && stack.top().value <= otDivide)
					polish.append(stack.pop());
			case otOpen:
				stack.push(token);
				break;
			case otClose:
				while(true) {
					if (stack.isEmpty()) {
						error = "Bracket mismatch in semantic";
						return false;
					}
					Token tok = stack.pop();
					if (tok.value != otOpen)
						polish.append(tok);
					else
						break;
				}
			default:;
			}
		} else
			polish.append(token);
	}
	while (!stack.isEmpty() && stack.top().value < otOpen)
		polish.append(stack.pop());
	return true;
}

bool TokenParser::lexic()
{
	expression.remove(" ");
	QStringList oper;
	oper
		<< "\\*"
		<< "\\/"
		<< "\\+"
		<< "\\-"
		<< "\\("
		<< "\\)"
		   ;
	QRegExp re(QString("(\\d+)|x|%1").arg(oper.join("|")));
	bool var = false;
	QString tmp;
	int index = 0;
	int brackets = 0;
	while (true) {
		int newindex = re.indexIn(expression, index);
		if (newindex != -1) {
			if (index != newindex) {
				error = "Wrong symbol at " + QString::number(index);
				return false;
			}
			tmp = re.cap();
			index = newindex + tmp.size();
		} else if (index < expression.size() - 1) {
			error = "Wrong symbol at " + QString::number(index);
			return false;
		} else
			break;
		int op = oper.indexOf("\\"+tmp);
		if (op != -1) {
			if (var) {
				error = "No letter number at " + QString::number(index);
				return false;
			}
			switch(op) {
			case otOpen: ++brackets; break;
			case otClose: --brackets; break;
			default:;
			}
			if (brackets < 0) {
				error = "Wrong closing bracket at " + QString::number(index);
				return false;
			}
			tokens.append(Token(ttOperation, op));
		} else if (tmp == "x")
			var = true;
		else {
			if (var)
				tokens.append(Token(ttVariable, tmp.toInt()));
			else
				tokens.append(Token(ttNumber, tmp.toInt()));
			var = false;
		}
	}
	if (brackets != 0) {
		error = "Brackets mismatch at " + QString::number(index);
		return false;
	}
	return true;
}

QString TokenParser::calculate(Street *street)
{
	QStack<int> stack;
	foreach(Token t, polish) {
		switch(t.type) {
		case ttOperation:
		{
			int op1, op2;
			if (stack.isEmpty()) {
				error = "Bad stack";
				return QString();
			}
			op1 = stack.pop();
			if (stack.isEmpty()) {
				error = "Bad stack";
				return QString();
			}
			op2 = stack.pop();
			switch (t.value) {
			case otPlus: stack.push(op1 + op2); break;
			case otMinus: stack.push(op2 - op1); break;
			case otMultiply: stack.push(op1 * op2); break;
			case otDivide: if (op1 == 0) {
					error = "Zero divide";
					stack.push(1);
				} else
					stack.push(op2 / op1);
				break;
			}
			break;
		}
		case ttNumber:
		case ttVariable:
		{
			int num = t.value;
			if (t.type == ttVariable)
				num = street->letterNumber(num);
			if (num <= 0)
				return QString();
			stack.push(num);
		}
		default:;
		}
	}
	if (stack.isEmpty()) {
		error = "Stack is empty";
		return QString();
	} else if (stack.size() > 1) {
		error = "Stack is overfilled";
		return QString();
	}
	return QString::number(stack.top());
}

QString TokenParser::errors() const
{
	return error;
}


bool MultiParser::setExpr(const QString &exp)
{
	parsers.clear();
	QRegExp re("[\\dx\\+\\-\\:\\*\\(\\)]+");
	pattern = exp.toLower();
	// replace latin with cyrillic
	pattern.replace("c", "с");
	pattern.replace("k", "к");
	pattern.replace("b", "вл");
	// replace long with short
	pattern.replace("стр", "с");
	int index = 0;
	int pos = 0;
	while(true) {
		pos = re.indexIn(pattern, pos);
		if (pos == -1)
			break;
		QString var = re.cap();
		QString rep = "%"+QString::number(++index);
		pattern.replace(pos, var.size(), rep);
		var.replace(":", "/");
		pos += rep.size();
		TokenParser tp;
		if (tp.setExpr(var))
			parsers.append(tp);
		else {
			error = tp.errors();
			return false;
		}
	}
	return true;
}

QString MultiParser::calculate(Street *street)
{
	QString res = pattern;
	foreach(TokenParser tp, parsers) {
		res = res.arg(tp.calculate(street));
		if (!tp.errors().isEmpty()) {
			error = tp.errors();
			return QString();
		}
	}
	return res;
}

QString MultiParser::errors() const
{
	return error;
}
