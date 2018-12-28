#ifndef PLOTDATA_QWT_H
#define PLOTDATA_QWT_H

#include <QColor>
#include <qwt_series_data.h>
#include <qwt_plot_marker.h>
#include "PlotJuggler/plotdata.h"

class TimeseriesQwt: public QwtSeriesData<QPointF>
{
public:

    TimeseriesQwt(const PlotData* base, double time_offset);

    virtual QPointF sample( size_t i ) const override;

    virtual QRectF boundingRect() const override;

    virtual size_t size() const override;

    PlotData::RangeTimeOpt getVisualizationRangeX();

    PlotData::RangeValueOpt getVisualizationRangeY(PlotData::RangeTime range_X);

    nonstd::optional<QPointF> sampleFromTime(double t);

    virtual void updateCache() = 0;

public slots:

    void onTimeoffsetChanged(double offset);

protected:
    const PlotData* _plot_data;

    std::vector<QPointF> _cached_curve;

    QRectF _bounding_box;

    double _time_offset;
};

//---------------------------------------------------------

class Timeseries_NoTransform: public TimeseriesQwt
{
public:
    Timeseries_NoTransform(const PlotData* base, double time_offset):
        TimeseriesQwt(base,time_offset)
    {
        updateCache();
    }

     void updateCache() override;
};

class Timeseries_1stDerivative: public TimeseriesQwt
{
public:
    Timeseries_1stDerivative(const PlotData* base, double time_offset):
        TimeseriesQwt(base,time_offset)
    {
        updateCache();
    }

     void updateCache() override;
};

class Timeseries_2ndDerivative: public TimeseriesQwt
{
public:
    Timeseries_2ndDerivative(const PlotData* base, double time_offset):
        TimeseriesQwt(base,time_offset)
    {
        updateCache();
    }

     void updateCache() override;
};



#endif // PLOTDATA_H
