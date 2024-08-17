#ifndef TESTDAO_H
#define TESTDAO_H

#include "basedao.h"
#include <QList>
#include <QPair>
#include <QString>

class ROGALUNADATABASESERVER_EXPORT TestDAO : public BaseDAO
{
public:
    explicit TestDAO(QSqlDatabase& db) : BaseDAO(db) {}

    bool createTable();
    bool insertTest(const QString& name);
    QList<QPair<int, QString>> getAllTests();
    QPair<int, QString> getTestById(int id);
};

#endif // TESTDAO_H
