#include "schedulerjob.h"

SchedulerJob::SchedulerJob()
{

}

SimulationRun &SchedulerJob::addAt(int position)
{
    mList.insert(position, SimulationRun());
    return mList[position];
}