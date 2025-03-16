#include "urlvalidator.h"

UrlValidator::UrlValidator(QObject *parent) : QRegularExpressionValidator(parent) {
    setRegularExpression(QRegularExpression("^https?://.+", QRegularExpression::CaseInsensitiveOption));
}
