#include "vesseldeftable.h"

using namespace sqlite;

VesselDefTable::VesselDefTable(std::shared_ptr<SQLiteStorage> db, std::string name)
    : SQLiteTable(db, name)
{
    if (db->tableExists(name))
        db->dropTable(name);

    create(std::make_tuple(fldId,
                           fldVesselId,
                           fldSpeeds,
                           fldFuelcons,
                           fldLengths,
                           fldVKWs,
                           fldCarrycapacities,
                           fldTankcapacities,
                           fldNbfpingspertrips,
                           fldResttime_par1s,
                           fldResttime_par2s,
                           fldAv_trip_duration,
                           fldMult_fuelcons_when_steaming,
                           fldMult_fuelcons_when_fishing,
                           fldMult_fuelcons_when_returning,
                           fldMult_fuelcons_when_inactive,
                           fldFirm_ids));
}

void VesselDefTable::feedVesselsDefTable(const std::vector<std::string> &vesselids, const std::vector<double> &speeds, const std::vector<double> &length)
{
    auto l = vesselids.size();
    for (int i = 0; i < l; ++i) {
        insert (fldVesselId.assign(vesselids[i]),
                fldSpeeds.assign(speeds[i]),
                fldLengths.assign(length[i]));
    }
}

