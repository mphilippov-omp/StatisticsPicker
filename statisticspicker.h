#ifndef STATISTICSPICKER_H
#define STATISTICSPICKER_H

#include <QObject>
#include <QVector>
#include <QPair>

class StatisticsPicker : public QObject
{
    Q_OBJECT
public:
    explicit StatisticsPicker(QObject *parent = nullptr);

    void setFileName(const QString &fileName);

    QVector<QPair<QString, int>> getStatistics() const;

    bool hasError() const;

public slots:
    void pickTopStatistics();

private:
    bool m_hasError;
    QString m_fileName;
    QVector<QPair<QString, int>> m_statistics;

    void readDataAndPickStatistics(const QString &fileName);
    int getIndex(const QVector<QPair<QString, int>> &data, const QString &occurence);
    void sortData();

signals:
    void fileOpened();
    void fileRead(const QString &progress);
    void statisticsPicked();
    void dataSorted();

    void isEmptyErrorRecieved();
    void fileCantOpenErrorRecieved();
};

#endif // STATISTICSPICKER_H
