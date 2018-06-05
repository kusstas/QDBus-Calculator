#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QString>
#include <string>
#include <stack>

class Calculator : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kusstas.ICalculator")

public:

    enum class ErrorType
    {
        NONE,
        OPERATOR_MISSED,
        OPERAND_MISSED,
        UNEXPECTED_SYMBOL,
        TOO_MANY_OPEN_BRACKET,
        TOO_MANY_CLOSE_BRACKET,
        UNDEF_ERROR
    };

    explicit Calculator(QObject* parent = nullptr);
    virtual ~Calculator();

public slots:

    QString compute(QString const& data) const;

signals:

    void result(QString const& result) const;

private:

    inline bool isOperator(QChar c) const { return c == '+' || c == '-' || c == '*' || c == '/'; }
    inline int priority(QChar c) const
    {
        if (c == '+' || c == '-')
            return 1;
        if (c == '*' || c == '/')
            return 2;
        return -1;
    }

    bool computeOperation(std::stack<int>& values, QChar op) const;
};

#endif // CALCULATOR_H
