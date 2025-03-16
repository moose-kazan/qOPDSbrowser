#ifndef URLVALIDATOR_H
#define URLVALIDATOR_H

#include <QRegularExpressionValidator>

class UrlValidator : public QRegularExpressionValidator
{
public:
    UrlValidator(QObject *parent = nullptr);
};

#endif // URLVALIDATOR_H
