import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtCharts 2.3
import com.statistics.demo 1.0

Page {
    id: page

    readonly property int indent: 40
    readonly property int space: 5

    width: 600
    height: 400

    function startNewSession() {
        executor.startNewSession()

        startButton.enabled = false
        openButton.enabled = true
        newSessionButton.enabled = false

        busy.running = false
        busy.visible = false

        chartView.visible = false
        progress.value = 0
        stateLabel.text = qsTr("Please open file with statistics")
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Choose a text file for statistics")
        nameFilters: ["Text File (*.txt)"]
        onAccepted: {
            executor.fileName = Qt.resolvedUrl(fileDialog.fileUrl).toString()
            startButton.enabled = true
            stateLabel.text = qsTr("File is given. We are ready to start")
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("Statistics picker")
        icon: StandardIcon.Critical
        modality: Qt.WindowModal
        onAccepted: startNewSession()
    }

    StatisticsExecutor {
        id: executor

        onFileCantOpenErrorRecieved: {
            messageDialog.text = qsTr("Open file error!")
            messageDialog.open()
        }

        onIsEmptyErrorRecieved: {
            messageDialog.text = qsTr("File is empty!")
            messageDialog.open()
        }

        onFileOpened: {
            stateLabel.text = qsTr("File is opened. Preparing reading...")
            progress.value = 0.1
        }

        onFileRead: {
            stateLabel.text = progress
            progress.value = 0.25
        }

        onStatisticsPicked: {
            stateLabel.text = qsTr("Statistics is picked. Sorting...")
            progress.value = 0.75
        }

        onDataSorted: {
            stateLabel.text = qsTr("Data is sorted. Preparing to display")

            progress.value = 0.9
        }

        onDisplayPrepared: {
            stateLabel.text = qsTr("Displaying")
            progress.value = 1

            busy.running = false
            busy.visible = false

            chartView.visible = true

            newSessionButton.enabled = true
        }
    }

    header: ToolBar {
        id: toolBar

        anchors.left: parent.left
        anchors.right: parent.right


        Row {
            leftPadding: indent
            spacing: space
            ToolButton {
                id: openButton

                antialiasing: true
                hoverEnabled: true

                ToolTip.text: qsTr("Open")
                ToolTip.visible: openButton.hovered

                contentItem: Image {
                    source: "images/open-file.png"
                    opacity: enabled ? 1 : 0.5
                }


                onClicked: {
                    fileDialog.open()
                }
            }

            ToolButton {
                id: startButton

                enabled: false
                antialiasing: true
                hoverEnabled: true

                ToolTip.visible: startButton.hovered
                ToolTip.text: qsTr("Run")

                contentItem: Image {
                    source: "images/run.png"
                    opacity: enabled ? 1 : 0.5
                }

                onClicked: {
                    startButton.enabled = false
                    openButton.enabled = false

                    busy.visible = true
                    busy.running = true
                    executor.execute()
                }
            }

            ToolButton {
                id: newSessionButton
                enabled: false
                antialiasing: true
                hoverEnabled: true

                ToolTip.visible: newSessionButton.hovered
                ToolTip.text: qsTr("Start new session");

                contentItem: Image {
                    source: "images/refresh.png"
                    opacity: enabled ? 1 : 0.5
                }

                onClicked: startNewSession()
            }
        }
    }

    ChartView {
        id: chartView

        anchors {
            left: parent.left
            right: parent.right

            leftMargin: indent
            rightMargin: indent
        }

        visible: false
        height: parent.height

        theme: ChartView.ChartThemeBlueCerulean
        antialiasing: true

        BarSeries {
            id: barSeries
            barWidth: 1
            axisX: BarCategoryAxis { categories: executor.categories }
            axisY: ValueAxis {
                min: 0
                max: Math.max.apply(Math, Array.from(executor.values))
            }

            BarSet {
                values: Array.from(executor.values)
            }
        }
    }

    BusyIndicator {
        id: busy

        anchors.centerIn: parent

        running: false
        visible: false
    }

    footer: Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
        }

        spacing: space

        ProgressBar {
            id: progress

            anchors {
                left: parent.left
                right: parent.right
                leftMargin: indent
                rightMargin: indent
            }

            value: 0.0
        }

        Label {
            id: stateLabel

            anchors {
                right: parent.right
                left: parent.left
                leftMargin: indent
                rightMargin: indent
            }

            text: qsTr("Please open file with statistics")
        }
    }
}

