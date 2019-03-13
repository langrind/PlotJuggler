#ifndef DATASTREAM_JSON_H
#define DATASTREAM_JSON_H

#include <QtPlugin>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkDatagram>
#include <thread>
#include "PlotJuggler/datastreamer_base.h"


class  DataStreamJSON: public DataStreamer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.icarustechnology.PlotJuggler.DataStreamer" "../datastreamer.json")
    Q_INTERFACES(DataStreamer)

public:

    DataStreamJSON();

    virtual bool start() override;

    virtual void shutdown() override;

    virtual bool isRunning() const override;

    virtual ~DataStreamJSON();

    virtual const char* name() const override { return "DataStreamer JSON"; }

    virtual bool isDebugPlugin() override { return false; }

    virtual QDomElement xmlSaveState(QDomDocument &doc) const override;

    virtual bool xmlLoadState(QDomElement &parent_element ) override;

private:

    void loop();

    std::thread _thread;

    bool _running;
    int  _port;
    void pushSingleValue( QJsonObject jobj, const double t, const QString & valueName );
    void pushSingleCycle(QNetworkDatagram &datagram);

    QUdpSocket * _sock;
};

#endif // DATAATREAM_JSON_H
