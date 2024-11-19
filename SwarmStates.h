#ifndef SWARM_STATES_H
#define SWARM_STATES_H

#include "Swarm.h"

using State = Swarm::SwarmState;

static UnitTargets phase1_targets = {
    {sc2::UNIT_TYPEID::ZERG_DRONE,       10},
    {sc2::UNIT_TYPEID::ZERG_ZERGLING,    1}
};
static State phase1(phase1_targets);

static UnitTargets phase2_targets = {
    {},
    {},
};
static State phase2(phase2_targets);

#endif