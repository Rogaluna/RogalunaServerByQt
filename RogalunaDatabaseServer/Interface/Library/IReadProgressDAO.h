#ifndef IREADPROGRESSDAO_H
#define IREADPROGRESSDAO_H

#include <QJsonObject>
#include <QString>


namespace Library {

class IReadProgressDAO
{
public:
    virtual ~IReadProgressDAO() {}

    /**
     * @brief 更新用户对特定书籍的阅读进度
     * @param userId 用户 ID
     * @param bookId 书籍 ID
     * @param chapterNumber 当前章节号
     * @param progress 章节内的进度（以百分比或秒为单位）
     * @return 如果更新成功，返回 true；否则返回 false
     */
    virtual bool updateProgress(int userId, const QString& bookId, int chapterNumber, int progress) = 0;

    /**
     * @brief 获取用户对特定书籍的阅读进度
     * @param userId 用户 ID
     * @param bookId 书籍 ID
     * @return 如果找到进度信息，返回包含用户阅读进度的 QJsonObject，否则将返回空的 json 对象
     */
    virtual QJsonObject getProgress(int userId, const QString& bookId) = 0;

    /**
     * @brief 删除用户对特定书籍的阅读进度记录
     * @param userId 用户 ID
     * @param bookId 书籍 ID
     * @return 如果删除成功，返回 true；否则返回 false
     */
    virtual bool deleteProgress(int userId, const QString& bookId) = 0;
};

}

#endif // IREADPROGRESSDAO_H
