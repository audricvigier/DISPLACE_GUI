#include "populationsstatplot.h"

#include <displacemodel.h>
#include <storage/sqliteoutputstorage.h>
#include <storage/tables/vesselslogliketable.h>
#include <sqlitestatement.h>

#include <qcustomplot.h>

#include <QPen>

using namespace displace::plot;

PopulationsStatPlot::PopulationsStatPlot(QCustomPlot *plt)
{
    plot = plt;
    timeline = new QCPItemLine(plot);

    plot->addItem(timeline);

    mPalette = PaletteManager::instance()->palette(PopulationRole);
}

void PopulationsStatPlot::update(DisplaceModel *model, displace::plot::PopulationStat stat)
{
    plot->clearGraphs();

    QList<int> interPopList = model->getInterestingPops();
    QList<int> interSizeList = model->getInterestingSizes();
    QList<int> graphList;
    bool showtotal = model->isInterestingSizeTotal();
    bool showavg =  model->isInterestingSizeAvg();
    bool showmin =  model->isInterestingSizeMin();
    bool showmax =  model->isInterestingSizeMax();

    if (showmax)
        graphList.push_front(-4);
    if (showmin)
        graphList.push_front(-3);
    if (showavg)
        graphList.push_front(-2);
    if (showtotal)
        graphList.push_front(-1);

    if (graphList.size() == 0)
        graphList.append(interSizeList);

    /* If no size is selected, but aggregate is selected, select all sizes */
    if (interSizeList.size() == 0 && graphList.size() != 0) {
        for(int i = 0; i < model->getSzGrupsCount(); ++i)
            interSizeList.push_back(i);
    }

    int graphNum = graphList.size();

    QList<QCPGraph *>graphs;

    double t = model->getCurrentStep();
    if (timeline != nullptr) {
        timeline->start->setCoords(t, timelineMin);
        timeline->end->setCoords(t, timelineMax);
    }

    foreach (int ipop, interPopList) {
        for (int igraph = 0; igraph < graphNum; ++igraph) {
            // Creates graph. Index in list are: ip * nsz + isz
            QCPGraph *graph = plot->addGraph();
            QColor col = mPalette.colorByIndex(ipop);

            graph->setLineStyle(QCPGraph::lsLine);
            graph->setPen(QPen(QBrush(col),2));

            col.setAlpha(128);
            graph->setBrush(QBrush(col));

            AggregationType aggtype = AggregationType::None;

            switch (graphList[igraph]) {
            case -4:
                graph->setName(QString(QObject::tr("Pop %1 Max")).arg(ipop));
                aggtype = AggregationType::Max;
                break;
            case -3:
                graph->setName(QString(QObject::tr("Pop %1 Min")).arg(ipop));
                aggtype = AggregationType::Min;
                break;
            case -2:
                graph->setName(QString(QObject::tr("Pop %1 Avg")).arg(ipop));
                aggtype = AggregationType::Avg;
                break;
            case -1:
                graph->setName(QString(QObject::tr("Pop %1 Total")).arg(ipop));
                aggtype = AggregationType::Sum;
                break;
            default:
                graph->setName(QString(QObject::tr("Pop %1 Group %2")).arg(ipop).arg(graphList[igraph]+1));
                aggtype = AggregationType::None;
                break;
            }

            auto v = getData(model, stat, aggtype, ipop, graphList[igraph]);
            graph->setData(std::get<0>(v), std::get<1>(v));
            graphs.push_back(graph);
        }
    }

    switch (stat) {
    case PopulationStat::Aggregate:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("Numbers ('000)"));
        break;
    case PopulationStat::Mortality:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("F"));
        break;
    case PopulationStat::SSB:
        plot->xAxis->setLabel(QObject::tr("Time (h)"));
        plot->yAxis->setLabel(QObject::tr("SSB (kg)"));
        break;
    }


    plot->rescaleAxes();
    plot->replot();
}

void PopulationsStatPlot::setCurrentTimeStep(double t)
{
    timeline->start->setCoords(t, timelineMin);
    timeline->end->setCoords(t, timelineMax);
}

std::tuple<QVector<double>, QVector<double> > PopulationsStatPlot::getData(DisplaceModel *model, displace::plot::PopulationStat stattype, displace::plot::AggregationType aggtype,
                                                                           int popid, int grpid)
{
    auto db = model->getOutputStorage();
    if (db == nullptr)
        throw std::runtime_error("null db");

    auto dt = db->getPopulationStatData(stattype, aggtype, popid, grpid);

    QVector<double> kd = QVector<double>::fromStdVector(dt.t), vd = QVector<double>::fromStdVector(dt.v);

    double rc = 0;
    // make running sum
    for (int i = 0; i < vd.size(); ++i) {
        rc += vd[i];
        vd[i] = rc;
    }

    return std::make_tuple(kd, vd);
}


void PopulationsStatPlot::createPopup(GraphInteractionController::PopupMenuLocation location, QMenu *menu)
{
    if (location == GraphInteractionController::PopupMenuLocation::Plot) {
        menu->addAction(QObject::tr("Save Data"), std::bind(&PopulationsStatPlot::saveTo, this));
    }
}

void PopulationsStatPlot::saveTo()
{
    if (!lastModel)
        return;

    auto db = lastModel->getOutputStorage();
    if (db == nullptr)
        return;

    QString fn = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save plot data"), QString(), QObject::tr("Csv file (*.csv)"));
    if (!fn.isEmpty()) {
        QFile file(fn);
        if (!file.open(QIODevice::ReadWrite)) {
            QMessageBox::warning(nullptr, QObject::tr("Error"), QObject::tr("Cannot save to %1: %2").arg(fn).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);


        QList<int> interPopList = lastModel->getInterestingPops();
        QList<int> interSizeList = lastModel->getInterestingSizes();

        for (auto ip : interPopList) {
            for (auto is : interSizeList) {
                AggregationType aggtype = AggregationType::None;

                strm << "Pop," << ip << ",Size," << is << "\n";

                auto v = getData(lastModel, lastStat, aggtype, ip, is);
                auto &k = std::get<0>(v);
                auto &d = std::get<1>(v);

                strm << "t";
                for (int i = 0; i < k.size(); ++i) {
                    strm << "," << k[i];
                }
                strm << "\nv";
                for (int i = 0; i < d.size(); ++i) {
                    strm << "," << d[i];
                }
                strm << "\n\n";
            }
        }
    }
}