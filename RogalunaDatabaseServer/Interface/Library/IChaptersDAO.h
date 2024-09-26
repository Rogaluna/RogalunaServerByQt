#ifndef ICHAPTERSDAO_H
#define ICHAPTERSDAO_H

#include <QString>
#include <QJsonObject>

namespace Library {

class IChaptersDAO
{
public:
    virtual ~IChaptersDAO() {}

    virtual bool addChapter(const QString& bookId, int chapterNumber, const QString& title, const QString &group, const QString &fileName) = 0;
    virtual QJsonObject getChapterDetails(const QString& bookId, int chapterNumber) = 0;
    virtual bool updateChapter(const QString& bookId, int chapterNumber, int newChapterNumber, const QString& title, const QString& group, int wordCount) = 0;
    virtual bool deleteChapter(const QString& bookId, int chapterNumber) = 0;
    virtual QJsonArray listChapters(const QString& bookId) = 0;

    virtual QPair<QVector<QString>, bool> getAllChapterFileName(const QString &bookId) = 0;
};

}

#endif // ICHAPTERSDAO_H
