#ifndef CHAPTERSDAO_H
#define CHAPTERSDAO_H

#include <BaseDAO.h>

#include <Interface/Library/IChaptersDAO.h>

namespace Library {

class ROGALUNADATABASESERVER_EXPORT ChaptersDAO : public BaseDAO, public IChaptersDAO
{
public:
    explicit ChaptersDAO(QSqlDatabase& db, const QString& schema = "library")
        : BaseDAO(db, schema, "chapters") {}

public:
    bool addChapter(const QString& bookId, int chapterNumber, const QString& title, const QString &group, const QString &fileName) override;
    QJsonObject getChapterDetails(const QString& bookId, int chapterNumber) override;
    bool updateChapter(const QString& bookId, int chapterNumber, int newChapterNumber, const QString& title, const QString& group, int wordCount) override;
    bool deleteChapter(const QString& bookId, int chapterNumber) override;
    QJsonArray listChapters(const QString& bookId) override;

    QPair<QVector<QString>, bool> getAllChapterFileName(const QString &bookId) override;
};

}

#endif // CHAPTERSDAO_H
