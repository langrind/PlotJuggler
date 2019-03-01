#include "datastream_json.h"
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QInputDialog>
#include <thread>
#include <mutex>
#include <chrono>
#include <thread>
#include <math.h>

DataStreamJSON::DataStreamJSON()
{
    // copied this from example, is it needed?
    dataMap().addNumeric("empty");
}

bool DataStreamJSON::start()
{
    if( !_running )
    {
        if( _port <= 0 )
        {
            bool ok;
            _port = QInputDialog::getInt(nullptr, tr(""),
                                         tr("UDP Port to receive JSON data:"), 5006, 1111, 65535, 1, &ok);
            if( !ok)
            {
                return _running;
            }
        }

        if( _port > 0 )
        {
            qDebug() << "JSON Streamer receiving on UDP port " << _port;
            _sock = new QUdpSocket(this);
            _sock->bind(QHostAddress::LocalHost, _port);

            _running = true;
            _thread = std::thread([this](){ this->loop();} );
            return true;
        }
    }
    else
    {
        qDebug() << "JSON Streamer already running on port " << _port;
        QMessageBox::information(nullptr,"Info",QString("JSON Streamer already running on port: %1").arg(_port));
    }
    return _running;
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
    QDomElement elem = doc.createElement("networkConfig");
    elem.setAttribute("port", _port );
    return elem;
}

bool DataStreamJSON::xmlLoadState(QDomElement &parent_element)
{
    QDomElement network_config =  parent_element.firstChildElement( "networkConfig" );
    if( !network_config.isNull() )
    {
        if( network_config.hasAttribute("port") )
        {
            _port = network_config.attribute("port").toInt();
            return true;
        }
    }

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
