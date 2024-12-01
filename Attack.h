#ifndef ATTACK_H
#define ATTACK_H

#include <vector>
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

class Attack : public sc2::Agent{
public:
    void RoachRush(const sc2::ObservationInterface* observation);
    void GetPriorityTargets(const sc2::ObservationInterface* observation);
    void GetWeakTargets(const sc2::ObservationInterface* observation);
    void GetCloseTargets(const sc2::Unit* unit, const sc2::ObservationInterface* observation);
    void GatherRoaches(const sc2::ObservationInterface* observation);
    void AttackTargets(const sc2::ObservationInterface* observation);
    bool ScoutWithOverlord(const sc2::ObservationInterface* observation);

    void AttackWithUnitType(sc2::UnitTypeID unit_type, const sc2::ObservationInterface* observation);
    void AttackWithUnit(sc2::UnitTypeID unit_type, const sc2::ObservationInterface* observation);

private:
    std::vector<const sc2::Unit*> targets;
    std::vector<const sc2::Unit*> roaches;
};

#endif // ATTACK_H

