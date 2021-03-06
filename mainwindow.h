#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QJsonDocument ReadJson(const QString &path);
    void process_json();
    void do_download();
    QVariant loadsettings(QString settings);
    void savesettings(QString settings, QVariant attr);
    bool readJsonFile(QString file_path, QVariantMap& result);

    ~MainWindow();

public slots:
    void replyFinished (QNetworkReply *reply);
    void refresh();
    void reload_pressed();
private slots:
    void on_pairlist_activated(int index);

    void on_forwarddays_editingFinished();

    void on_startdate_editingFinished();

    void on_timeframes_activated(int index);

    void on_rsi_calc_pressed();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
};
#endif // MAINWINDOW_H
