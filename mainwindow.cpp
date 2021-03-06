#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QAction>

QString appgroup="buysellmonitor";
QString filename="candledata.json",pair="BTCUSDT",timeframe="1h";

long startdate;
int limit=24,days=24;
QStringList timeframes={"5m","1h","2h","4h","8h","1d"};
bool customtimeframe=false;
QStringList pairlist = {"1INCHUSDT","AAVEUSDT","ADAUSDT","ALGOUSDT","ALICEUSDT","ALPHAUSDT","ANKRUSDT","ANTUSDT","ARUSDT","ARDRUSDT","ATOMUSDT",
                        "AVAXUSDT","AXSUSDT","BAKEUSDT","BALUSDT","BANDUSDT","BATUSDT","BCHUSDT","BNTUSDT","BTCUSDT","BTCSTUSDT","BTGUSDT","BTSUSDT","BTTUSDT",
                        "CAKEUSDT","CELOUSDT","CELRUSDT","CFXUSDT","CHZUSDT","CKBUSDT","COMPUSDT","COTIUSDT","CRVUSDT","CTSIUSDT","CVCUSDT",
                        "DASHUSDT","DATAUSDT","DCRUSDT","DENTUSDT","DGBUSDT","DODOUSDT","DOGEUSDT","DOTUSDT","EGLDUSDT","ENJUSDT","EOSUSDT",
                        "ETCUSDT","ETHUSDT","FETUSDT","FILUSDT","FTMUSDT","FTTUSDT","FUNUSDT","GRTUSDT","HBARUSDT","HIVEUSDT","HNTUSDT","HOTUSDT",
                        "ICPUSDT","ICXUSDT","INJUSDT","IOSTUSDT","IOTXUSDT","JSTUSDT","KAVAUSDT","KMDUSDT","KNCUSDT","KSMUSDT","LINKUSDT","LPTUSDT",
                        "LRCUSDT","LSKUSDT","LTCUSDT","LUNAUSDT","MANAUSDT","MATICUSDT","MDXUSDT","MKRUSDT","MTLUSDT","NANOUSDT","NEARUSDT",
                        "NEOUSDT","NKNUSDT","NMRUSDT","NUUSDT","OCEANUSDT","OGNUSDT","OMGUSDT","ONEUSDT","ONGUSDT","ONTUSDT","OXTUSDT","PAXUSDT",
                        "QTUMUSDT","REEFUSDT","RENUSDT","REPUSDT","RIFUSDT","RLCUSDT","RSRUSDT","RUNEUSDT","RVNUSDT","SANDUSDT","SCUSDT","SHIBUSDT",
                        "SKLUSDT","SNXUSDT","SOLUSDT","SRMUSDT","STMXUSDT","STORJUSDT","STRAXUSDT","STXUSDT","SUNUSDT","SUPERUSDT","SUSHIUSDT",
                        "SXPUSDT","TFUELUSDT","THETAUSDT","TKOUSDT","TRXUSDT","UMAUSDT","UNIUSDT","VETUSDT","VTHOUSDT","WAVESUSDT","WINUSDT",
                        "WRXUSDT","XEMUSDT","XLMUSDT","XMRUSDT","XRPUSDT","XTZUSDT","XVGUSDT","XVSUSDT","YFIUSDT","ZECUSDT","ZENUSDT","ZILUSDT",
                        "ZRXUSDT"};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDateTime cdt=QDateTime::currentDateTime().addDays(-1);
    this->setWindowTitle("Timeframe Walker");
    setGeometry(loadsettings("position").toRect());
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    ui->pairlist->addItems(pairlist);
    ui->pairlist->setCurrentText(pair);
    ui->timeframes->addItems(timeframes);
    ui->timeframes->setCurrentText(timeframe);
    ui->rsi->setValue(35);
    QDate cd = QDate::currentDate();
    ui->startdate->setDate(cd.addDays(-1));
    connect(ui->reload, SIGNAL(clicked()), this,SLOT(reload_pressed()));
    startdate = cdt.currentMSecsSinceEpoch()-86400000;
    ui->message->setText("Forward date: "+ cd.toString("ddd d MMM yy"));
    ui->forwarddays->setValue(limit/days);
    pair=ui->pairlist->currentText();
    do_download();
}

MainWindow::~MainWindow()
{
    savesettings("position",this->geometry());
    delete ui;
}

void MainWindow::do_download()  // Docs https://binance-docs.github.io/apidocs/spot/en/
{
        QUrl url = QUrl(QString("https://www.binance.com/api/v3/klines?symbol="+pair+"&interval="+timeframe+"&limit="+QString::number(limit))+"&startTime="+QString::number(startdate));
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

double calc_rsi(QStringList rsi_list)
{
    int counter=0,samples=15;
    double avgGain=0,avgLoss=0,rsi=0;
    for (int i=1;i<samples;i++ ) {
        double value=rsi_list[i].toDouble();
        if (value>=0) avgGain+=value;
        else avgLoss+=value*-1;
        counter++;
    }
    avgGain/=counter;
    avgLoss/=counter;
    for (int i=counter+1;i<rsi_list.count();i++) {
        double value=rsi_list[i].toDouble();
        if (value>=0) {
            avgGain=(avgGain*(13)+value)/14;
            avgLoss=(avgLoss*(13))/14;
        } else {
            value*=-1;
            avgLoss=(avgLoss*(13)+value)/14;
            avgGain=(avgGain*(13))/14;
        }
    }
    double rs=avgGain/avgLoss;
    rsi=100-(100/(1+rs));
    return rsi;
}

void MainWindow::process_json()
{
    QStringList rsi_list;
    QJsonDocument jsonDoc = ReadJson(filename);
    double low,high,open,close,previous_close=0;
    QDateTime dt;
    double lowprice=0,highprice=0,lowopen=0,highopen=0,lowclose=0,highclose=0, averageprice=0;
    QString QHigh_date, QLow_date,QOpen_low_date,QOpen_high_date,QClose_low_date,QClose_high_date;
    for (int i=0;i<limit;i++)
    {
        QString Qopen = jsonDoc[i][1].toString(); // 1=open
        QString Qhigh = jsonDoc[i][2].toString(); // 2=high
        QString Qlow = jsonDoc[i][3].toString(); // 3=low
        QString Qclose = jsonDoc[i][4].toString(); // 4=close
        if (i==0) rsi_list.append(QString::number(Qclose.toDouble()-Qopen.toDouble()));
        if (i>0) rsi_list.append(QString::number(Qclose.toDouble()-previous_close));
        previous_close=Qclose.toDouble();
        low = Qlow.toDouble();
        high = Qhigh.toDouble();
        open = Qopen.toDouble();
        close = Qclose.toDouble();
        averageprice+=low+high;
        if (lowprice > low || lowprice == 0) {
            lowprice=low;
            double timestamp = jsonDoc[i][0].toDouble(); // open time
            dt.setMSecsSinceEpoch(timestamp);
            QLow_date = dt.toString("ddd d MMM - hh:mm");
        }
        if (highprice < high) {
            highprice=high;
            double timestamp = jsonDoc[i][0].toDouble(); // open time
            dt.setMSecsSinceEpoch(timestamp);
            QHigh_date = dt.toString("ddd d MMM - hh:mm");
        }
        if (lowopen > open || lowopen == 0) {
            lowopen=open;
            double timestamp = jsonDoc[i][0].toDouble(); // open time
            dt.setMSecsSinceEpoch(timestamp);
            QOpen_low_date = dt.toString("ddd d MMM - hh:mm");
        }
        if (highopen < open) {
            highopen=open;
            double timestamp = jsonDoc[i][0].toDouble(); // open time
            dt.setMSecsSinceEpoch(timestamp);
            QOpen_high_date = dt.toString("ddd d MMM - hh:mm");
        }
        if (lowclose > close || lowclose == 0) {
            lowclose=open;
            double timestamp = jsonDoc[i][6].toDouble(); // close time
            dt.setMSecsSinceEpoch(timestamp);
            QClose_low_date = dt.toString("ddd d MMM - hh:mm");
        }
        if (highclose < close) {
            highclose=open;
            double timestamp = jsonDoc[i][6].toDouble(); // close time
            dt.setMSecsSinceEpoch(timestamp);
            QClose_high_date = dt.toString("ddd d MMM - hh:mm");
        }
    }
    averageprice = averageprice/limit/2;
    double rsi=0;
    if (limit==33) rsi = calc_rsi(rsi_list);
    QDate cd = QDate::currentDate();
    int today=(QDateTime::currentDateTime().currentMSecsSinceEpoch()+startdate)/86400000;
    cd = cd.addDays(-(limit/days)-today);
    int trailingZeros=2;
    if (previous_close<0.001) trailingZeros=7;
    if (previous_close<0.1&&previous_close>0.001) trailingZeros=5;
    //qDebug() << trailingZeros << " " << previous_close;
    if (!ui->seekAll->isChecked()) {
        ui->transferLog->appendPlainText("Timeframe: "+timeframe+" - Number of candles: "+QString::number(limit));
        ui->transferLog->appendPlainText("Pair: "+pair+" Startdate: "+ui->startdate->date().toString("ddd d MMM"));
        //ui->transferLog->appendPlainText("--- Low ---");
        ui->transferLog->appendPlainText("Last close: "+QLocale(QLocale::English).toString(previous_close,'F',trailingZeros));
        ui->transferLog->appendPlainText("Lowest price: "+ QLocale(QLocale::English).toString(lowprice,'F',trailingZeros) + " Date: " + QLow_date);
        ui->transferLog->appendPlainText("Lowest open: "+ QLocale(QLocale::English).toString(lowopen,'F',trailingZeros) + " Date: " + QOpen_low_date);
        ui->transferLog->appendPlainText("Lowest close: "+ QLocale(QLocale::English).toString(lowclose,'F',trailingZeros) + " Date: " + QClose_low_date);
        //ui->transferLog->appendPlainText("--- High ---");
        ui->transferLog->appendPlainText("Average price: "+ QLocale(QLocale::English).toString(averageprice,'F',trailingZeros));
        if (limit==33) ui->transferLog->appendPlainText("RSI: "+ QLocale(QLocale::English).toString(rsi,'F',2));
        ui->transferLog->appendPlainText("Highest price: "+ QLocale(QLocale::English).toString(highprice,'F',trailingZeros) + " Date: " + QHigh_date);
        ui->transferLog->appendPlainText("Highest open: "+ QLocale(QLocale::English).toString(highopen,'F',trailingZeros) + " Date: " + QOpen_high_date);
        ui->transferLog->appendPlainText("Highest close: "+ QLocale(QLocale::English).toString(highclose,'F',trailingZeros) + " Date: " + QClose_high_date);
        double percent_change=(highprice/lowprice*100)-100;
        ui->transferLog->appendPlainText("Percent change: "+ QLocale(QLocale::English).toString(percent_change,'F',2)+"%");
        ui->transferLog->appendPlainText("------------------------");
    } else {
        if (ui->rsi->value() >= rsi) {
            ui->transferLog->appendPlainText(pair+" Last close "+QLocale(QLocale::English).toString(previous_close,'F',trailingZeros)+" RSI "+QLocale(QLocale::English).toString(rsi,'F',2));
        }
    }
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
    limit = ui->forwarddays->value()*days;
    do_download();
}

void MainWindow::reload_pressed()
{
    customtimeframe=true;
    if (ui->timeframes->currentText() == "5m") {
        timeframe = "1h";
        days=24;
        qDebug() << timeframe;
    }
    refresh();
}

void MainWindow::refresh()
{
    QDateTime edt=QDateTime(ui->startdate->date(),QTime::currentTime());
    pair = ui->pairlist->currentText();

    if (ui->forwarddays->value() <= 41 && !customtimeframe) {
        timeframe = "1h";
        days=24;
    } else if (ui->forwarddays->value() >= 42 && ui->forwarddays->value() <= 82 && !customtimeframe) {
        timeframe = "2h";
        days=12;
    } else if (ui->forwarddays->value() >= 83 && ui->forwarddays->value() <= 164 && !customtimeframe) {
        timeframe = "4h";
        days=6;
    } else if (ui->forwarddays->value() >= 165 && ui->forwarddays->value() <= 329 && !customtimeframe) {
        timeframe = "8h";
        days=3;
    } else if (!customtimeframe) {
        timeframe = "1d";
        days=1;
    }
    if (customtimeframe) {
        timeframe = ui->timeframes->currentText();
        if (timeframe=="1h") days=24;
        if (timeframe=="2h") days=12;
        if (timeframe=="4h") days=6;
        if (timeframe=="8h") days=3;
        if (timeframe=="1d") days=1;
    }

    limit = ui->forwarddays->value()*days;
    startdate = edt.toMSecsSinceEpoch();
    QDate cd = QDate::currentDate();
    int today=(QDateTime::currentDateTime().currentMSecsSinceEpoch()-startdate)/86400000;
    cd = cd.addDays((limit/days)-today);
    ui->message->setText("Forward date: "+ ui->startdate->date().toString("ddd d MMM yy"));
    if ((limit > 1000 && customtimeframe)) {
        if (limit > 1000 && customtimeframe) ui->transferLog->appendPlainText("ERROR! Custom timeframe out of scope! Reduce days\n or use bigger timeframe.\n");
        customtimeframe=false;
    } else {
        if (cd > QDate::currentDate()) {
            cd = QDate::currentDate();
            ui->startdate->setDate(cd.addDays(-ui->forwarddays->value()));
            edt=QDateTime(ui->startdate->date(),QTime::currentTime());
            limit = ui->forwarddays->value()*days;
            startdate = edt.toMSecsSinceEpoch();
            ui->message->setText("Forward date: "+ ui->startdate->date().toString("ddd d MMM yy"));
        }
        customtimeframe=false;
        do_download();
    }
}

void MainWindow::on_forwarddays_editingFinished()
{
    //refresh();
}

void MainWindow::on_startdate_editingFinished()
{
    //refresh();
}


void MainWindow::on_timeframes_activated(int index)
{
    customtimeframe=true;
    //refresh();
}

void delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(1000);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::on_rsi_calc_pressed()
{
    double candles=0;
    timeframe = ui->timeframes->currentText();
    if (timeframe=="5m") candles=2.75;
    if (timeframe=="1h") candles=33;
    if (timeframe=="2h") candles=66;
    if (timeframe=="4h") candles=132;
    if (timeframe=="8h") candles=364;
    if (timeframe=="1d") candles=792;
    limit=33;
    startdate = QDateTime(QDate::currentDate(),QTime::currentTime()).toMSecsSinceEpoch()-(3600000*candles);
    if (ui->seekAll->isChecked()) {
        ui->transferLog->appendPlainText("RSI search < "+QString::number(ui->rsi->value())+" - Timeframe "+ timeframe);
        ui->message->setText("RSI searching...Please wait");
        QString tmpPair=pair;
        ui->reload->setHidden(true);
        ui->seekAll->setText("Cancel");
        ui->timeframes->setHidden(true);
        for ( const auto& i : pairlist  ) {
            if(ui->message->isHidden())
                ui->message->show();
            else
                ui->message->hide();
          pair=i;
          do_download();
          ui->pairlist->setCurrentText(pair);
          if (!ui->seekAll->isChecked()) break;
          delay();
        }
        if(ui->message->isHidden()) ui->message->show();
        ui->transferLog->appendPlainText("------------------------");
        ui->seekAll->setChecked(false);
        ui->reload->setHidden(false);
        ui->seekAll->setText("Seek all");
        ui->timeframes->setHidden(false);
        pair=tmpPair;
        ui->pairlist->setCurrentText(pair);
        ui->message->setText("RSI search done!");
    } else do_download();

}
