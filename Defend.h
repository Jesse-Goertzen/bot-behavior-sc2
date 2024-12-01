#ifndef DEFEND_H
#define DEFEND_H

#include <vector>
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

class Defend : public sc2::Agent{
public:
    void DefendAgainstAttackers(const sc2::ObservationInterface* observation);
    void GetEnemies(const sc2::ObservationInterface* observation);
    void GatherDefenders(const sc2::ObservationInterface* observation);

private:
    std::vector<const sc2::Unit*> enemies;
    std::vector<const sc2::Unit*> defenders;
};

#endif // DEFEND_H