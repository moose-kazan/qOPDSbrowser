#include "helpers.h"

#include <QRegularExpression>

Helpers::Helpers() = default;

QString Helpers::cleanFileName(QString baseName, const QString& extension) {
    const QRegularExpression re("[^\\w\\d_\\-\\.]+", QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);

    baseName = baseName.replace(re, "_");
    if (!extension.isNull()) {
        if (!extension.startsWith('.')) {
            baseName.append('.');
        }
        baseName.append(extension);
    }

    return baseName;
}
