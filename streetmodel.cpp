#include "streetmodel.h"

#include "street.h"

StreetModel::StreetModel(QObject *parent):
	QAbstractItemModel(parent), mHost(NULL)
{
}

StreetModel::StreetModel(StreetList *host, QObject *parent):
	QAbstractItemModel(parent), mHost(host)
{
}

StreetModel::~StreetModel()
{
}

QModelIndex StreetModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	else
		return createIndex(row, column);
}

QModelIndex StreetModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

int StreetModel::rowCount(const QModelIndex &) const
{
	return mStreets.size();
}

int StreetModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant StreetModel::data(const QModelIndex &index, int role) const
{
	if (	index.parent().isValid() ||
		index.row() < 0 ||
		index.row() >= mStreets.size())
		return QVariant();
	Street* street = mStreets.at(index.row());
	switch(role) {
	case Qt::DisplayRole:
	case WholeNameRole:
		return street->wholeName();
	case NameRole:
		return street->name();
	case TypeRole:
		return street->type();
	case NumberRole:
		return street->number();
	case HousesRole:
		return street->houses();
	case SecondaryRole:
		return street->secondary();
	default:;
	}
	return QVariant();
}

QHash<int, QByteArray> StreetModel::roleNames() const
{
	QHash<int, QByteArray> res;
	res[WholeNameRole] = "wholeName";
	res[NameRole] = "name";
	res[TypeRole] = "type";
	res[NumberRole] = "number";
	res[HousesRole] = "houses";
	res[SecondaryRole] = "secondary";
	return res;
}

StreetList *StreetModel::host() const
{
	return mHost;
}

void StreetModel::setHost(StreetList *host)
{
	if (mHost == host)
		return;

	mHost = host;
	init();
	emit hostChanged(host);
}

QString StreetModel::nameForRow(int row) const
{
	if (row >= 0 && row < mStreets.size())
		return mStreets.at(row)->wholeName();
	else
		return QString();
}

int StreetModel::rowForName(QString name) const
{
	for (int i = 0; i < mStreets.size(); ++i)
		if (mStreets.at(i)->wholeName() == name)
			return i;
	return -1;
}

void StreetModel::init()
{
	if (!mHost) return;

	beginResetModel();

	mStreets.clear();
	mFilter.clear();
	QStringList streets = mHost->streets();
	foreach (QString street, streets) {
		Street* str = mHost->street(street);
		mStreets.append(str);
	}
	foreach(QString dist, mHost->districts())
		mFilter[dist] = true;

	endResetModel();
}

void StreetModel::filter(QString district, bool flag)
{
	mFilter[district] = flag;
}

void StreetModel::refresh()
{
	if (!mHost) return;

	beginResetModel();
	mStreets.clear();

	QStringList streets = mHost->streets();
	foreach (QString street, streets) {
		Street* str = mHost->street(street);
		bool filtered = true;
		foreach(QString district, str->districts())
			if (mFilter.value(district, false)) {
				filtered = false;
				break;
			}
		if (!filtered)
			mStreets.append(str);
	}

	endResetModel();
}

enum TokenType {
	Invalid,
	Operation,
	Number,
	Variable
};
enum OperationType {
	Multiply = 0,
	Divide,
	Plus,
	Minus,
	Open,
	Close
};

struct Token {
	TokenType type;
	int value;
	Token(): type(Invalid) {}
	Token(TokenType t, int v): type(t), value(v) {}
	QString toString() const {
		switch(type) {
		case Operation:
			switch(value) {
			case Plus: return "+";
			case Minus: return "-";
			case Multiply: return "*";
			case Divide: return "/";
			case Open: return "(";
			case Close: return ")";
			default: return "?";
			}
		case Number: return QString::number(value);
		case Variable: return "x"+QString::number(value);
		default: return "INV";
		}
	}
};

class TokenParser {
public:
	TokenParser(const QString &exp): expression(exp) {}
	bool syntax()
	{
		if (!error.isEmpty())
			return false;
		bool literExp = true;
		int index = 0;
		foreach(const Token &token, tokens) {
			++index;
			if (literExp) {
				if (token.type == Operation) {
					if (token.value != Open) {
						error = "Operand expected at token " +
							QString::number(index);
						return false;
					}
				} else
					literExp = false;
			} else {
				if (token.type == Operation) {
					switch (token.value) {
					case Open:
						error = "Operand expected at token " +
							QString::number(index);
						return false;
					case Close: break;
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

	bool semantic()
	{
		if (!error.isEmpty())
			return false;
		QStack<Token> stack;
		foreach (const Token& token, tokens) {
			if (token.type == Operation) {
				switch (token.value) {
				case Plus:
				case Minus:
					while(!stack.isEmpty() && stack.top().value <= Minus)
						polish.append(stack.pop());
					stack.push(token);
					break;
				case Multiply:
				case Divide:
					while(!stack.isEmpty() && stack.top().value <= Divide)
						polish.append(stack.pop());
				case Open:
					stack.push(token);
					break;
				case Close:
					while(true) {
						if (stack.isEmpty()) {
							error = "Bracket mismatch in semantic";
							return false;
						}
						Token tok = stack.pop();
						if (tok.value != Open)
							polish.append(tok);
						else
							break;
					}
				default:;
				}
			} else
				polish.append(token);
		}
		while (!stack.isEmpty() && stack.top().value < Open)
			polish.append(stack.pop());
		return true;
	}

	bool lexic() {
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
				case Open: ++brackets; break;
				case Close: --brackets; break;
				default:;
				}
				if (brackets < 0) {
					error = "Wrong closing bracket at " + QString::number(index);
					return false;
				}
				tokens.append(Token(Operation, op));
			} else if (tmp == "x")
				var = true;
			else {
				if (var)
					tokens.append(Token(Variable, tmp.toInt()));
				else
					tokens.append(Token(Number, tmp.toInt()));
				var = false;
			}
		}
		if (brackets != 0) {
			error = "Brackets mismatch at " + QString::number(index);
			return false;
		}
		return true;
	}

	QString calculate(Street* street) {
		QStack<int> stack;
		foreach(Token t, polish) {
			switch(t.type) {
			case Operation:
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
				case Plus: stack.push(op1 + op2); break;
				case Minus: stack.push(op1 - op2); break;
				case Multiply: stack.push(op1 * op2); break;
				case Divide: if (op2 == 0) {
						error = "Zero divide";
						stack.push(1);
					} else
						stack.push(op1 / op2);
					break;
				}
				break;
			}
			case Number:
			case Variable:
			{
				int num = t.value;
				if (t.type == Variable)
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

	QString error;
private:
	QList<Token> tokens;
	QList<Token> polish;
	QString expression;
};

void StreetModel::bruteforce(const QString &exp, int indexFrom)
{
	int stindex = indexFrom;
	if (stindex < 0)
		stindex = 0;
	else {
		++stindex;
		if (stindex == mStreets.size())
			stindex = 0;
	}
	int index = stindex;
	TokenParser tp(exp);
	tp.lexic() && tp.syntax() && tp.semantic();
	if (!tp.error.isEmpty()) {
		emit error(QString("Expression error: %1").arg(tp.error));
		return;
	}
	while (true) {
		QString res = tp.calculate(mStreets[index]);
		if (!tp.error.isEmpty()) {
			emit error(QString("Evaluate error: %1").arg(tp.error));
			return;
		}
		if (!res.isEmpty()) {
			int hIndex = mStreets[index]->houseList().indexOf(res);
			if (hIndex != -1) {
				emit selected(index, hIndex);
				return;
			}
		}

		++index;
		if (index == mStreets.size())
			index = 0;
		if (index == stindex)
			break;
	}
	emit notFound();
}
