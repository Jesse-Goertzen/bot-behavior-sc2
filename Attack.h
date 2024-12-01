#ifndef ATTACK_H
#define ATTACK_H

#include <vector>
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
// #include "BasicSc2Bot.h"

class BasicSc2Bot;

class Attack {
public:
    void RoachRush(BasicSc2Bot& bot);
    void GetPriorityTargets(BasicSc2Bot& bot);
    void GetWeakTargets(BasicSc2Bot& bot);
    void GetCloseTargets(const sc2::Unit* unit, BasicSc2Bot& bot);
    void GatherRoaches(BasicSc2Bot& bot);
    void AttackTargets(BasicSc2Bot& bot);
    bool ScoutWithOverlord(BasicSc2Bot& bot);

    void AttackWithUnitType(sc2::UnitTypeID unit_type, BasicSc2Bot& bot);
    void AttackWithUnit(sc2::UnitTypeID unit_type, BasicSc2Bot& bot);

private:
    std::vector<const sc2::Unit*> targets;
    std::vector<const sc2::Unit*> roaches;
};

#endif // ATTACK_H

