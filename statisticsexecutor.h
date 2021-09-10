#ifndef STATISTICSEXECUTOR_H
#define STATISTICSEXECUTOR_H

#include <QObject>
#include <QString>
#include <QStringList>

class StatisticsPicker;
class StatisticsExecutor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(QList<int> values READ values NOTIFY valuesChanged)

public:
    StatisticsExecutor(QObject * parent = nullptr);
    virtual ~StatisticsExecutor() override;

    QString fileName() const;
    void setFileName(const QString &fileUrl);

    QStringList categories() const;

    QList<int> values() const;

    Q_INVOKABLE void execute();
    Q_INVOKABLE void startNewSession();

signals:
    void fileNameChanged();
    void categoriesChanged();
    void valuesChanged();

    void fileOpened();
    void fileRead(const QString &progress);
    void statisticsPicked();
    void dataSorted();
    void displayPrepared();

    void fileCantOpenErrorRecieved();
    void isEmptyErrorRecieved();

private:
    QString m_fileName;
    QStringList m_categories;
    QList<int> m_values;
    StatisticsPicker * m_statisticsPicker;
};

#endif // STATISTICSEXECUTOR_H
