#ifndef MD5HASHGENERATOR_H
#define MD5HASHGENERATOR_H

#ifndef QCRYPTOGRAPHICHASH_H
#include <QCryptographicHash>
#endif

#define GENERATE_MD5(input) \
([](auto&& value) -> QString { \
        using T = std::decay_t<decltype(value)>; \
        if constexpr (std::is_same_v<T, QString>) { \
            /* 如果输入是 QString，则计算字符串的 MD5 */ \
            return QString(QCryptographicHash::hash(value.toUtf8(), QCryptographicHash::Md5).toHex()); \
    } else if constexpr (std::is_same_v<T, QFile>) { \
            /* 如果输入是 QFile，则计算文件的 MD5 */ \
            if (!value.open(QIODevice::ReadOnly)) return QString(); \
            QCryptographicHash hash(QCryptographicHash::Md5); \
            hash.addData(&value); \
            value.close(); \
            return QString(hash.result().toHex()); \
    } else { \
            return QString(); \
    } \
})(input)

#endif // MD5HASHGENERATOR_H
