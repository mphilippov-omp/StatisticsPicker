#include "statisticsexecutor.h"

#include <QtConcurrent/QtConcurrent>
#include <QUrl>
#include <QDir>
#include <QThread>

#include "statisticspicker.h"

StatisticsExecutor::StatisticsExecutor(QObject *parent)
    : QObject(parent)
    , m_statisticsPicker(new StatisticsPicker())
{
    connect(m_statisticsPicker, &StatisticsPicker::fileOpened, this, &StatisticsExecutor::fileOpened);
    connect(m_statisticsPicker, &StatisticsPicker::fileRead, this, &StatisticsExecutor::fileRead);
    connect(m_statisticsPicker, &StatisticsPicker::dataSorted, this, &StatisticsExecutor::dataSorted);
    connect(m_statisticsPicker, &StatisticsPicker::statisticsPicked, this, &StatisticsExecutor::statisticsPicked);

    connect(m_statisticsPicker, &StatisticsPicker::isEmptyErrorRecieved, this, &StatisticsExecutor::isEmptyErrorRecieved);
    connect(m_statisticsPicker, &StatisticsPicker::fileCantOpenErrorRecieved, this, &StatisticsExecutor::fileCantOpenErrorRecieved);
}

StatisticsExecutor::~StatisticsExecutor()
{
    m_statisticsPicker->deleteLater();
    m_statisticsPicker = nullptr;
}

QString StatisticsExecutor::fileName() const
{
    return m_fileName;
}

void StatisticsExecutor::setFileName(const QString &fileUrl)
{
    QString fileName;
    const QUrl url(fileUrl);
    if (url.isLocalFile()) {
        fileName = QDir::toNativeSeparators(url.toLocalFile());
    } else {
        fileName = fileUrl;
    }

    if (m_fileName != fileName) {
        m_fileName = fileName;
        m_statisticsPicker->setFileName(m_fileName);
        emit fileNameChanged();
    }
}

void StatisticsExecutor::execute()
{
    QThread * thread = new QThread(this);

    m_statisticsPicker->moveToThread(thread);

    connect(thread, &QThread::started, m_statisticsPicker, &StatisticsPicker::pickTopStatistics);

    thread->start();

    connect(m_statisticsPicker, &StatisticsPicker::isEmptyErrorRecieved, thread, &QThread::terminate);
    connect(m_statisticsPicker, &StatisticsPicker::fileCantOpenErrorRecieved, thread, &QThread::terminate);
    connect(m_statisticsPicker, &StatisticsPicker::dataSorted, thread, &QThread::quit);

    connect(thread, &QThread::finished, this, [&](){
        if (!m_statisticsPicker->hasError()) {
            QVector<QPair<QString, int>> result = m_statisticsPicker->getStatistics();

            foreach (const auto &pair, result) {
                m_values << pair.second;
                m_categories << pair.first;
            }

            emit categoriesChanged();
            emit valuesChanged();
            emit displayPrepared();
        }
    });
}

void StatisticsExecutor::startNewSession()
{
    setFileName(QString());
    m_categories.clear();
    m_values.clear();

    emit categoriesChanged();
    emit valuesChanged();
}

QList<int> StatisticsExecutor::values() const
{
    return m_values;
}

QStringList StatisticsExecutor::categories() const
{
    return m_categories;
}
