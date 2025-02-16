//
// Created by happycactus on 07/08/19.
//

#include "LoaderTestSuite.h"

#include "db/ConfigTable.h"
#include "db/ScenarioConfigTable.h"
#include "NodesLoader.h"
#include "EdgesLoader.h"
#include "DatabaseInputImpl/VesselsLoader.h"
#include "DatabaseInputImpl/MetiersLoader.h"
#include "Node.h"
#include "ClosuresLoader.h"
#include "readdata.h"

#include <boost/log/trivial.hpp>

struct LoaderTestSuite::Impl {
    msqlitecpp::v2::Storage &db;

    displace::in::ConfigTable config;
    displace::in::ScenarioConfigTable scenario;

    int graphsce;
    int biosce;
    int fleetsce;

    std::vector<Node *> nodes;
    adjacency_map_t admap;

    explicit Impl(msqlitecpp::v2::Storage &db_)
            : db(db_)
    {}
};

LoaderTestSuite::LoaderTestSuite(msqlitecpp::v2::Storage &db)
        : p(spimpl::make_unique_impl<Impl>(db))
{

}

void LoaderTestSuite::prepare()
{
    BOOST_LOG_TRIVIAL(info) << "Loading configuration";

    p->config.query(p->db);

    BOOST_LOG_TRIVIAL(info) << "Loading scenario data";
    p->scenario.query(p->db);

    p->graphsce = p->scenario.getAs<int>("a_graph");
    p->biosce = p->scenario.getAs<int>("biolsce");
    p->fleetsce = p->scenario.getAs<int>("fleetsce");

    BOOST_LOG_TRIVIAL(info) << "Graph Scenario: " << p->graphsce;
}

void LoaderTestSuite::loadNodes()
{
    BOOST_LOG_TRIVIAL(info) << "Loading nodes for graphsce " << p->graphsce;

    NodesLoader loader(p->db);
    p->nodes = loader.load(p->graphsce);

    BOOST_LOG_TRIVIAL(info) << "Nodes loaded: " << p->nodes.size() << " nodes";
}

void LoaderTestSuite::loadEdges()
{
    BOOST_LOG_TRIVIAL(info) << "Loading edges for graphsce " << p->graphsce;

    EdgesLoader loader(p->db);
    p->admap = loader.load(p->graphsce);

    size_t nnum = p->admap.size();
    size_t ednum = 0;
    for (auto &nn : p->admap) {
        ednum += nn.second.size();
    }

    BOOST_LOG_TRIVIAL(info) << "Edges: loaded " << nnum << " nodes adjancencies, total " << ednum << " edges";
}

void LoaderTestSuite::loadVessels()
{
    BOOST_LOG_TRIVIAL(info) << "Loading edges for fleetsce " << p->fleetsce;

    VesselsLoader loader(p->db);
    auto allvessels = loader.getListOfAllVessels();

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << allvessels.size() << " Vessels name";

    auto fuelprices = loader.getInitFuelPrices();

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << fuelprices.size() << " Initial fuel prices";

	std::vector<std::shared_ptr<VesselsLoader::VesselData>> v;
    size_t count = 0, ecount = 0;
    for (auto &vessel : allvessels) {
        for (int quarter = 1; quarter <= 4; ++quarter) {
            auto data = loader.getVesselData(vessel, quarter);
            ++count;
			v.push_back(data);
        }

        auto ecfeat = loader.getEconomicFeature(vessel);
        ecount += ecfeat.size();
    }

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << ecount << " Economic Features and " << count << " Records";
}


void LoaderTestSuite::loadMetiers()
{
    MetiersLoader loader(p->db);
    auto allmetiers = loader.getListOfAllMetiers();

    BOOST_LOG_TRIVIAL(info) << "Metiers: loaded " << allmetiers.size() << " Metiers name";

  
    std::vector<std::shared_ptr<MetiersLoader::MetierData>> v;
    size_t count = 0, ecount = 0;
    for (auto& metier : allmetiers) {
        for (int quarter = 1; quarter <= 4; ++quarter) {
            auto data = loader.getMetierData(metier, quarter);
            ++count;
            v.push_back(data);
        }

        ecount += ecfeat.size();
    }

    BOOST_LOG_TRIVIAL(info) << "Vessels: loaded " << ecount;
}


void LoaderTestSuite::loadMetierClosures()
{
    BOOST_LOG_TRIVIAL(info) << "Loading Metier Closures for graphsce " << p->graphsce;

    ClosuresLoader loader(p->db);
    vector<NodeBanningInfo> ban_info[12];

    size_t count = 0;
    for (int period = 1; period <= 12; ++period) {
        loader.read_metier_closures(p->graphsce, period, ban_info[period - 1]);
        count += ban_info[period - 1].size();
    }

    BOOST_LOG_TRIVIAL(info) << "Closures for metiers loaded: " << count;
}

void LoaderTestSuite::loadVSizeClosures()
{
    BOOST_LOG_TRIVIAL(info) << "Loading Vsize Closures for graphsce " << p->graphsce;

    ClosuresLoader loader(p->db);
    vector<NodeBanningInfo> ban_info[12];

    size_t count = 0;
    for (int period = 1; period <= 12; ++period) {
        loader.read_vsize_closures(p->graphsce, period, ban_info[period - 1]);
        count += ban_info[period - 1].size();
    }

    BOOST_LOG_TRIVIAL(info) << "Closures for Vsize loaded: " << count;
}
