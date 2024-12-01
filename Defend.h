#ifndef DEFEND_H
#define DEFEND_H

#include <vector>
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
// #include "BasicSc2Bot.h"

class BasicSc2Bot;

class Defend {
public:
    void DefendAgainstAttackers(BasicSc2Bot& bot);
    void GetEnemies(BasicSc2Bot& bot);
    void GatherDefenders(BasicSc2Bot& bot);

private:
    std::vector<const sc2::Unit*> enemies;
    std::vector<const sc2::Unit*> defenders;
};

#endif // DEFEND_H