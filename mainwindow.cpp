#include "mainwindow.h"
#include "./ui_mainwindow.h"

QString appgroup="buysellmonitor";
QString filename="candledata.json",pair="BTCUSDT",timeframe="1h";
double lowprice, highprice;
int limit=168,day=24;
QStringList pairlist = {"1INCHUSDT","AAVEUSDT","ADAUSDT","ALGOUSDT","ALPHAUSDT","ANKRUSDT","ANTUSDT","ARUSDT","ARDRUSDT","ATOMUSDT","AVAXUSDT","BAKEUSDT","BALUSDT","BANDUSDT","BATUSDT","BCHUSDT","BNTUSDT","BTCUSDT","BTCSTUSDT","BTGUSDT","BTSUSDT","BTTUSDT","CELOUSDT","CELRUSDT","CFXUSDT","CHZUSDT","CKBUSDT","COMPUSDT","COTIUSDT","CRVUSDT","CTSIUSDT","CVCUSDT","DASHUSDT","DATAUSDT","DCRUSDT","DENTUSDT","DGBUSDT","DODOUSDT","DOGEUSDT","DOTUSDT","EGLDUSDT","ENJUSDT","EOSUSDT","ETCUSDT","ETHUSDT","FETUSDT","FILUSDT","FTMUSDT","FTTUSDT","FUNUSDT","GRTUSDT","HBARUSDT","HIVEUSDT","HNTUSDT","HOTUSDT","ICXUSDT","INJUSDT","IOSTUSDT","IOTXUSDT","JSTUSDT","KAVAUSDT","KMDUSDT","KNCUSDT","LINKUSDT","LPTUSDT","LRCUSDT","LSKUSDT","LTCUSDT","LUNAUSDT","MANAUSDT","MATICUSDT","MDXUSDT","MKRUSDT","MTLUSDT","NANOUSDT","NEARUSDT","NEOUSDT","NKNUSDT","NMRUSDT","NUUSDT","OCEANUSDT","OGNUSDT","OMGUSDT","ONEUSDT","ONGUSDT","ONTUSDT","OXTUSDT","QTUMUSDT","REEFUSDT","RENUSDT","REPUSDT","RIFUSDT","RLCUSDT","RSRUSDT","RUNEUSDT","RVNUSDT","SANDUSDT","SCUSDT","SKLUSDT","SOLUSDT","SRMUSDT","STMXUSDT","STORJUSDT","STRAXUSDT","STXUSDT","SUNUSDT","SUSHIUSDT","SXPUSDT","TFUELUSDT","THETAUSDT","TRXUSDT","UMAUSDT","UNIUSDT","VETUSDT","VTHOUSDT","WAVESUSDT","WINUSDT","WRXUSDT","XEMUSDT","XLMUSDT","XMRUSDT","XTZUSDT","XVGUSDT","YFIUSDT","ZECUSDT","ZENUSDT","ZILUSDT","ZRXUSDT"};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Timeframe Walker");
    setGeometry(loadsettings("position").toRect());
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    ui->pairlist->addItems(pairlist);
    ui->pairlist->setCurrentText(pair);
    ui->lookbackdays->setValue(limit/day);
    pair=ui->pairlist->currentText();
    do_download();


}

MainWindow::~MainWindow()
{
    savesettings("position",this->geometry());
    delete ui;
}

void MainWindow::do_download()
{
        QUrl url = QUrl(QString("https://www.binance.com/api/v3/klines?symbol="+pair+"&interval="+timeframe+"&limit="+QString::number(limit)));
        //QUrl url = QUrl(QString("https://www.binance.com/api/v3/depth?symbol=BTCUSDT"));

        QNetworkRequest request(url);
        manager->get(request);

}


QVariant MainWindow::loadsettings(QString settings)
{
    QVariant returnvar;
    QSettings appsettings("QTinman",appgroup);
    appsettings.beginGroup(appgroup);
    returnvar = appsettings.value(settings);
    appsettings.endGroup();
    return returnvar;
}

void MainWindow::savesettings(QString settings, QVariant attr)
{
    QSettings appsettings("QTinman",appgroup);
    appsettings.beginGroup(appgroup);
    appsettings.setValue(settings,QVariant::fromValue(attr));
    appsettings.endGroup();
}

QJsonDocument MainWindow::ReadJson(const QString &path)
{
    QFile file( path );
    QJsonArray jsonArray;
    QJsonDocument cryptolist;
    if( file.open( QIODevice::ReadOnly ) )
    {
        QByteArray bytes = file.readAll();
        file.close();

        QJsonParseError parserError;
        cryptolist = QJsonDocument::fromJson(bytes, &parserError);
        //qDebug() << parserError.errorString();

        QJsonObject jsonObject = cryptolist.object();

    }
    return cryptolist;
}

bool MainWindow::readJsonFile(QString file_path, QVariantMap& result)
{
    // step 1
    QFile file_obj(file_path);
    if (!file_obj.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to open " << file_path << "\n";
    exit(1);
    }

    // step 2
    QTextStream file_text(&file_obj);
    QString json_string;
    json_string = file_text.readAll();
    file_obj.close();
    QByteArray json_bytes = json_string.toLocal8Bit();

    // step 3
    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if (json_doc.isNull()) {
        qDebug() << "Failed to create JSON doc." << "\n";
        return false;
    }
    if (!json_doc.isObject()) {
        qDebug() << "JSON is not an object." << "\n";
        return false;
    }

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) {
        qDebug() << "JSON object is empty." << "\n";
        return false;
    }

    // step 4
    result = json_obj.toVariantMap();
    return true;
}

void MainWindow::process_json()
{

    QJsonDocument jsonDoc = ReadJson(filename);
    double low,high;
    QString QHigh_date, QLow_date;
    for (int i=0;i<limit;i++)
    {
        QString Qlow = jsonDoc[i][3].toString();
        QString Qhigh = jsonDoc[i][2].toString();

        low = Qlow.toDouble();
        high = Qhigh.toDouble();
        if (lowprice > low || lowprice == 0) {
            lowprice=low;
            double timestamp = jsonDoc[i][0].toDouble();
            QDateTime dt;
            dt.setMSecsSinceEpoch(timestamp);
            QLow_date = dt.toString();
        }
        if (highprice < high) {
            highprice=high;
            double timestamp = jsonDoc[i][0].toDouble();
            QDateTime dt;
            dt.setMSecsSinceEpoch(timestamp);
            QHigh_date = dt.toString();
        }

    }
    ui->transferLog->appendPlainText("Timeframe: "+timeframe+" - Number of candles: "+QString::number(limit));
    ui->transferLog->appendPlainText("Pair: "+pair);
    ui->transferLog->appendPlainText("Lowest price: "+ QLocale(QLocale::English).toString(lowprice,'F',2) + " Date: " + QLow_date);
    ui->transferLog->appendPlainText("Highest price: "+ QLocale(QLocale::English).toString(highprice,'F',2) + " Date: " + QHigh_date);
    double percent_change=(highprice/lowprice*100)-100;
    ui->transferLog->appendPlainText("Percent change: "+ QLocale(QLocale::English).toString(percent_change,'F',2)+"%");
    ui->transferLog->appendPlainText("------------------------");
}

void MainWindow::replyFinished (QNetworkReply *reply)
{
    if(reply->error())
    {
        ui->transferLog->appendPlainText("ERROR!");
        ui->transferLog->appendPlainText(reply->errorString());
    }
    else
    {
        //ui->transferLog->appendPlainText(reply->header(QNetworkRequest::ContentTypeHeader).toString());
        //ui->transferLog->appendPlainText(reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString());
        //ui->transferLog->appendPlainText(reply->header(QNetworkRequest::ContentLengthHeader).toString());
        //ui->transferLog->appendPlainText(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
        //ui->transferLog->appendPlainText(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        //ui->transferLog->appendPlainText("OK");

        QFile *file = new QFile(filename);
        if (file->error()) qDebug() << file->errorString();
        if(file->open(QFile::WriteOnly))
        {
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        delete file;
        process_json();
    }

    reply->deleteLater();
}

void MainWindow::on_pairlist_activated(int index)
{
    pair = ui->pairlist->currentText();
    limit = ui->lookbackdays->value()*day;
    lowprice=0;
    highprice=0;
    do_download();
}


void MainWindow::on_lookbackdays_editingFinished()
{
    pair = ui->pairlist->currentText();
    if (ui->lookbackdays->value() <= 41) {
        timeframe = "1h";
        day=24;
    } else if (ui->lookbackdays->value() >= 42 && ui->lookbackdays->value() <= 82) {
        timeframe = "2h";
        day=12;
    } else if (ui->lookbackdays->value() >= 83 && ui->lookbackdays->value() <= 164) {
        timeframe = "4h";
        day=6;
    } else if (ui->lookbackdays->value() >= 165 && ui->lookbackdays->value() <= 329) {
        timeframe = "8h";
        day=3;
    }
    limit = ui->lookbackdays->value()*day;
    lowprice=0;
    highprice=0;
    do_download();
}
