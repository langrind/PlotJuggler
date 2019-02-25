#include "datastream_json.h"
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <thread>
#include <mutex>
#include <chrono>
#include <thread>
#include <math.h>

DataStreamJSON::DataStreamJSON()
{
    QStringList  words_list;
    words_list
        << "motor.1.targetRPM"
        << "motor.2.targetRPM"
        << "motor.3.targetRPM"
        << "motor.4.targetRPM"
        << "motor.5.targetRPM"
        << "motor.6.targetRPM"
        << "motor.1.targetTorque"
        << "motor.2.targetTorque"
        << "motor.3.targetTorque"
        << "motor.4.targetTorque"
        << "motor.5.targetTorque"
        << "motor.6.targetTorque"
        << "motor.1.actualRPM"
        << "motor.2.actualRPM"
        << "motor.3.actualRPM"
        << "motor.4.actualRPM"
        << "motor.5.actualRPM"
        << "motor.6.actualRPM"
        << "motor.1.actualTorque"
        << "motor.2.actualTorque"
        << "motor.3.actualTorque"
        << "motor.4.actualTorque"
        << "motor.5.actualTorque"
        << "motor.6.actualTorque"
        ;

    int N = words_list.size();

    foreach( const QString& name, words_list)
    {
        const std::string name_str = name.toStdString();
        dataMap().addNumeric(name_str);
    }
    dataMap().addNumeric("empty");

    _sock = new QUdpSocket(this);
    _sock->bind(QHostAddress::LocalHost, 5006);
}

bool DataStreamJSON::start()
{
    _running = true;
    //pushSingleCycle(); - restore this? the example does this
    _thread = std::thread([this](){ this->loop();} );
    return true;
}

void DataStreamJSON::shutdown()
{
    _running = false;
    if( _thread.joinable()) _thread.join();
}

bool DataStreamJSON::isRunning() const { return _running; }

DataStreamJSON::~DataStreamJSON()
{
    shutdown();
}

QDomElement DataStreamJSON::xmlSaveState(QDomDocument &doc) const
{
    return QDomElement();
}

bool DataStreamJSON::xmlLoadState(QDomElement &parent_element)
{
    return false;
}

void DataStreamJSON::pushSingleValue( QJsonObject jobj, const double t, QString & prefix, QString & valueName )
{
    QString fullName = prefix;
    fullName.append(valueName);

    std::unordered_map<std::string, PlotData>::iterator got = dataMap().numeric.find(fullName.toStdString());
    if ( got == dataMap().numeric.end() ) {
        printf ("Did not find '%s' in dataMap\n", fullName.toStdString().c_str() );
        return;
    }

    auto& plot = got->second;
    double y = jobj[valueName].toDouble();

    plot.pushBack( PlotData::Point( t, y ) );
}

void DataStreamJSON::pushSingleCycle(QNetworkDatagram &datagram)
{
	std::lock_guard<std::mutex> lock( mutex() );

    QJsonDocument jdoc = QJsonDocument::fromJson(datagram.data());
    QJsonObject jobj = jdoc.object();

    QString prefix;
    if ( jobj.contains("message") ) {
        prefix.append(jobj["message"].toString());
        prefix.append(".");
    }

    if ( jobj.contains("instance") ) {
        int foo = jobj["instance"].toInt();
        prefix.append(QString::fromUtf8(std::to_string(foo).c_str()));
        prefix.append(".");
    }

    using namespace std::chrono;
    static std::chrono::high_resolution_clock::time_point initial_time = high_resolution_clock::now();
    const double offset = duration_cast< duration<double>>( initial_time.time_since_epoch() ).count() ;
    auto now =  high_resolution_clock::now();
    const double t = duration_cast< duration<double>>( now - initial_time ).count() ;
#if 0
    QString targetRpmFullName = prefix;
    targetRpmFullName.append("targetRPM");

    std::unordered_map<std::string, PlotData>::iterator got = dataMap().numeric.find(targetRpmFullName.toStdString());
    if ( got == dataMap().numeric.end() ) {
        printf ("Did not find '%s' in dataMap\n", targetRpmFullName.toStdString().c_str() );
        return;
    }


    auto& plot = got->second;
    double y =  jobj[QString("targetRPM")].toDouble();

    plot.pushBack( PlotData::Point( t + offset, y ) );
#else
    QString targetRPM("targetRPM");
    pushSingleValue( jobj, t + offset, prefix, targetRPM);

    QString targetTorque("targetTorque");
    pushSingleValue( jobj, t + offset, prefix, targetTorque );

    QString actualRPM("actualRPM");
    pushSingleValue( jobj, t + offset, prefix, actualRPM );

    QString actualTorque("actualTorque");
    pushSingleValue( jobj, t + offset, prefix, actualTorque );
#endif

#if 0
    for (auto& it: dataMap().numeric )
    {
        if ( it.first == "empty") continue;

        QString name = prefix;
        name.append(QString::fromUtf8(it.first.c_str()));

        //if ( !jobj.contains(QString::fromUtf8(it.first.c_str())) ) continue;
        if ( !jobj.contains(name) ) continue;

        auto& plot = it.second;
        const double t = duration_cast< duration<double>>( now - initial_time ).count() ;
        //double y =  jobj[QString::fromUtf8(it.first.c_str())].toDouble();
        double y =  jobj[name].toDouble();

        plot.pushBack( PlotData::Point( t + offset, y ) );
    }
#endif
}

void DataStreamJSON::loop()
{
    _running = true;
    while( _running )
    {
        while (_sock->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = _sock->receiveDatagram();
            pushSingleCycle(datagram);
        }
        std::this_thread::sleep_for ( std::chrono::milliseconds(10) );
    }
}
