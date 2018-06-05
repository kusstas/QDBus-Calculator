#include "calculator.h"
#include <QStringBuilder>
#include <QDebug>

#include "icalculator_adaptor.h"

Calculator::Calculator(QObject* parent) : QObject(parent)
{
    new ICalculatorAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/org/kusstas/calculator", this);
    dbus.registerService("org.kusstas.calculator");
}

Calculator::~Calculator()
{

}

QString Calculator::compute(QString const& data) const
{
    qDebug().noquote() << "query: " % data;
    std::stack<int> values;
    std::stack<QChar> operations;
    bool errorOperation = false;

    ErrorType error = ErrorType::NONE;
    int tmpNumber = 0;
    int countBracket = 0;
    int indexLastPart = -1;
    int i;

    for (i = 0; i < data.size(); i++) {
        QChar c = data[i];
        if (c.isSpace()) {
            continue;
        }
        if (c == '(') {
            if (indexLastPart != -1 && !(isOperator(data[indexLastPart]) || data[indexLastPart] == '(')) {
                error = ErrorType::OPERATOR_MISSED;
                break;
            }
            operations.push('(');
            countBracket++;
        }
        else if (c == ')') {
            if (indexLastPart != -1 && (isOperator(data[indexLastPart]) || data[indexLastPart] == '(')) {
                error = ErrorType::OPERAND_MISSED;
                break;
            }

            while (!operations.empty() && operations.top() != '(' && !errorOperation) {
                errorOperation = !computeOperation(values, operations.top());
                operations.pop();
            }

            if (errorOperation) {
                error = ErrorType::UNDEF_ERROR;
                break;
            }

            if (operations.empty()) {
                error = ErrorType::TOO_MANY_CLOSE_BRACKET;
                break;
            }
            operations.pop();
            countBracket--;
        }
        else if (isOperator(c)) {
            if (indexLastPart == -1 || !(data[indexLastPart].isDigit() || data[indexLastPart] == ')')) {
                error = ErrorType::OPERAND_MISSED;
                break;
            }
            while (!operations.empty() && priority(operations.top()) >= priority(c) && !errorOperation) {
                errorOperation = !computeOperation(values, operations.top());
                operations.pop();
            }

            if (errorOperation) {
                error = ErrorType::UNDEF_ERROR;
                break;
            }

            operations.push(c);
        }
        else if (c.isDigit()) {
            if (indexLastPart != -1 && (data[indexLastPart].isDigit() || data[indexLastPart] == ')')) { // operator missed
                error = ErrorType::OPERATOR_MISSED;
                break;
            }
            tmpNumber = c.toLatin1() - '0';
            i++;
            while (i < data.size() && data[i].isDigit()) {
                tmpNumber = tmpNumber * 10 + data[i].toLatin1() - '0';
                i++;
            }
            i--;
            values.push(tmpNumber);
        }
        else {
            error = ErrorType::UNEXPECTED_SYMBOL;
            break;
        }
        indexLastPart = i;
    }

    if (error == ErrorType::NONE) {
        if (indexLastPart != -1 && isOperator(data[indexLastPart])) {
            error = ErrorType::OPERAND_MISSED;
        }
        else if (countBracket % 2 == 1) {
            error = ErrorType::TOO_MANY_OPEN_BRACKET;
        }
        else if (values.empty()) {
            error = ErrorType::UNDEF_ERROR;
        }
        else {
            while(!operations.empty() && !errorOperation) {
                errorOperation = !computeOperation(values, operations.top());
                operations.pop();
            }
            if (errorOperation) {
                error = ErrorType::UNDEF_ERROR;
            }
        }
    }

    // handle error
    QString res;
    switch(error) {
    case ErrorType::UNDEF_ERROR:
        res = "UNDEF_ERROR";
        break;
    case ErrorType::UNEXPECTED_SYMBOL:
        res = QString("UNEXPECTED_SYMBOL,") % data[i];
        break;
    case ErrorType::TOO_MANY_OPEN_BRACKET:
        res = "TOO_MANY_(";
        break;
    case ErrorType::TOO_MANY_CLOSE_BRACKET:
        res = "TOO_MANY_)";
        break;
    case ErrorType::OPERATOR_MISSED:
        res = QString("OPERATOR_MISSED,") % QStringRef(&data, indexLastPart, i - indexLastPart + 1);
        break;
    case ErrorType::OPERAND_MISSED:
        res = "OPERAND_MISSED,";
        if (indexLastPart == -1) {
            res += QStringRef(&data, 0, i + 1);
        }
        else {
            res += QStringRef(&data, indexLastPart, i - indexLastPart + 1);
        }
        break;
    case ErrorType::NONE:
        res = QString::number(values.top());
        break;
    }
    emit result(res);
    return res;
}

bool Calculator::computeOperation(std::stack<int> &values, QChar op) const
{
    int v2 = values.top();
    values.pop();
    int v1 = values.top();
    values.pop();
    switch (op.toLatin1()) {
    case '+':
        values.push(v1 + v2);
        break;
    case '-':
        values.push(v1 - v2);
        break;
    case '*':
        values.push(v1 * v2);
        break;
    case '/':
        if (v2 == 0)
            return false;
        values.push(v1 / v2);
        break;
    }
    return true;
}
