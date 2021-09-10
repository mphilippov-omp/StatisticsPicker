#include "statisticspicker.h"
#include <QTextStream>

#include <QFile>
#include <QDebug>

#define MAX_STATISTICS_THRESHOLD 15

StatisticsPicker::StatisticsPicker(QObject *parent)
    : QObject(parent)
    , m_hasError(false)
{

}

void StatisticsPicker::pickTopStatistics()
{
    m_statistics.clear();
    m_hasError = false;

    readDataAndPickStatistics(m_fileName);

    if (!m_hasError) {
        emit statisticsPicked();

        sortData();

        emit dataSorted();
    }
}

bool StatisticsPicker::hasError() const
{
    return m_hasError;
}

QVector<QPair<QString, int>> StatisticsPicker::getStatistics() const
{
    return m_statistics;
}

void StatisticsPicker::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

void StatisticsPicker::readDataAndPickStatistics(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        m_hasError = true;
        emit fileCantOpenErrorRecieved();
    } else {
        emit fileOpened();

        QTextStream out(&file);

        quint64 size = file.size();

        if (size == 0) {
            m_hasError = true;
            emit isEmptyErrorRecieved();
            return;
        }

        for (QString word = out.readLine(); !word.isNull(); word = out.readLine()) {
            word = word.trimmed();
            if (!word.isEmpty()) {
                int index = getIndex(m_statistics, word);

                if (index < 0) {
                    QPair<QString, int> pair(word, 1);
                    m_statistics << pair;
                } else {
                    m_statistics[index].second++;
                }
            }

            auto remains = file.bytesAvailable();
            auto progress = ((size - remains) * 100) / size;

            emit fileRead(tr("%1\% of file is read").arg(progress));
        };
    }
}

int StatisticsPicker::getIndex(const QVector<QPair<QString, int>> &data, const QString &occurence)
{
    for(int i = 0; i < data.count(); i++) {
        if (data[i].first == occurence) {
            return i;
        }
    }

    return -1;
}

void StatisticsPicker::sortData()
{
    qSort(m_statistics.begin(), m_statistics.end(),
          [&](const QPair<QString, int> &a, const QPair<QString, int> &b)
          {
            if (a.second > b.second)
                return true;
            else if (a.second == b.second && a.first < b.first)
                return true;
            else return false;
          });

    if (m_statistics.size() > MAX_STATISTICS_THRESHOLD) {
        m_statistics.resize(MAX_STATISTICS_THRESHOLD);
    }

    qSort(m_statistics.begin(), m_statistics.end(), [&](const QPair<QString, int> &a, const QPair<QString, int> &b) {return a.first < b.first;});
}
