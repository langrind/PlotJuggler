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
    dataMap().addNumeric("empty");

    _sock = new QUdpSocket(this);
    _sock->bind(QHostAddress::LocalHost, 5006);
}

bool DataStreamJSON::start()
{
    _running = true;
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

void DataStreamJSON::pushSingleValue( QJsonObject jobj, const double t, const QString & valueName )
{
    std::unordered_map<std::string, PlotData>::iterator got = dataMap().numeric.find(valueName.toStdString());
    if ( got == dataMap().numeric.end() ) {
        printf ("Did not find '%s' in dataMap\n", valueName.toStdString().c_str() );
        return;
    }

    auto& plot = got->second;
    double y = jobj[valueName].toDouble();

    plot.pushBack( PlotData::Point( t, y ) );
}

void DataStreamJSON::pushSingleCycle(QNetworkDatagram &datagram)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(datagram.data());
    QJsonObject jobj = jdoc.object();

	std::lock_guard<std::mutex> lock( mutex() );

    using namespace std::chrono;
    static std::chrono::high_resolution_clock::time_point initial_time = high_resolution_clock::now();
    const double offset = duration_cast< duration<double>>( initial_time.time_since_epoch() ).count() ;
    auto now =  high_resolution_clock::now();
    const double t = duration_cast< duration<double>>( now - initial_time ).count() ;

    foreach(const QString& key, jobj.keys()) {
        QJsonValue value = jobj.value(key);

        std::unordered_map<std::string, PlotData>::iterator got = dataMap().numeric.find(key.toStdString());
        if ( got == dataMap().numeric.end() ) {
            const std::string name_str = key.toStdString();
            dataMap().addNumeric(name_str);
        }

        pushSingleValue( jobj, t + offset, key );
    }
}

void DataStreamJSON::loop()
{
    _running = true;
    while( _running )
    {
        _sock->waitForReadyRead();
        while (_sock->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = _sock->receiveDatagram();
            pushSingleCycle(datagram);
        }
    }
}
