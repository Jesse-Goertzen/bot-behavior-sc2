/*

ROACH RUSH
Determine threshold (8-12?)
Common timing is after the Roach Warren is completed (and once roach army is built up)

Scouting
Overlord scouting
- Worker count
- Army size
- Defenses
If the oponent is too well-prepared, wait for more roaches?

Select target
Focus on high value targets
- Workers
- Key structures
Rally roaches near enemy base
Attack

KAIA'S OG PLAN
1. scout with overlord to see enemies defensive units
2. commence attack if defensive units is under threshold
3. attack = roaches attacking by priority

Issues
1. which enemy base if multiple
2. which strategy is efficient
3. how to determine threshold
4. how to handle strong defense from enemy teams

Upgraded PLAN?
...

*/


#include "Attack.h"

// -------------attack controller----------------

// main function for controlling the roach rush
// determine when and how we attack
void Attack::RoachRush(const sc2::ObservationInterface* observation) {

    // Prio attack
    GetPriorityTargets(observation);

    // Weak attack
    //GetWeakTargets(observation);

    // Close attack
    //GetCloseTargets(observation);

    GatherRoaches(observation);
    AttackTargets(observation);
}

// -------------attack policies----------------

// get targets based on priority - workers, key structures, other
void Attack::GetPriorityTargets(const sc2::ObservationInterface* observation) {
    targets.clear();

    sc2::Units enemy_units = observation->GetUnits(sc2::Unit::Alliance::Enemy);

    // targets based on priority
    for (const auto& unit : enemy_units) {
        // high priority - workers
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_SCV ||
            unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE ||
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_DRONE) {
            targets.push_back(unit);
        }
        // medium priority - key structures
        else if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_BARRACKS ||
                 unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE ||
                 unit->unit_type == sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL) {
            targets.push_back(unit);
        }
        // low priority - other structures
        else {
            targets.push_back(unit);
        }
    }
}

// get targets based on health - weakest/lowest first
void Attack::GetWeakTargets(const sc2::ObservationInterface* observation) {
    targets.clear();

    sc2::Units enemy_units = observation->GetUnits(sc2::Unit::Alliance::Enemy);
    std::sort(enemy_units.begin(), enemy_units.end(), [](const sc2::Unit* a, const sc2::Unit* b) {
        return a->health < b->health;
    });

    for (const auto& unit : enemy_units){
        targets.push_back(unit);
    }
}

// get targets based on proximity - closest first
// close to unit sent in
void Attack::GetCloseTargets(const sc2::Unit* unit, const sc2::ObservationInterface* observation) {
    targets.clear();

    sc2::Units enemy_units = observation->GetUnits(sc2::Unit::Alliance::Enemy);
    std::sort(enemy_units.begin(), enemy_units.end(), [&unit](const sc2::Unit* a, const sc2::Unit* b) {
        return Distance2D(unit->pos, a->pos) < Distance2D(unit->pos, b->pos);
    });

    for (const auto& unit : enemy_units){
        targets.push_back(unit);
    }
}

// -------------helper functions----------------

// keep track of our roaches in a vector
void Attack::GatherRoaches(const sc2::ObservationInterface* observation) {
    roaches.clear();

    sc2::Units all_units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto& unit : all_units) {
        if (unit->unit_type == sc2::UNIT_TYPEID::ZERG_ROACH) {
            roaches.push_back(unit);
        }
    }
}

void Attack::AttackTargets(const sc2::ObservationInterface* observation) {
    
    if (targets.empty()){
        return;
    }
    for (const auto& roach : roaches) {
        // no orders
        if (roach->orders.empty()) {
            Actions()->UnitCommand(roach, sc2::ABILITY_ID::ATTACK, targets.front()->pos);
            continue;
        }

        // order not attacking, make it attacking
        if (roach->orders.front().ability_id != sc2::ABILITY_ID::ATTACK) {
            Actions()->UnitCommand(roach, sc2::ABILITY_ID::ATTACK, targets.front()->pos);
        }
    }
}

// send an overlord to the enemy base to checkout defense units
// determine whether we should send roaches now (limited defense) or build a bigger army
bool Attack::ScoutWithOverlord(const sc2::ObservationInterface* observation) {
    sc2::Units overlords = observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_OVERLORD));
    if (overlords.empty()) { 
        return false; 
    }

    // enemy starting location
    const sc2::Point2D enemy_base = observation->GetGameInfo().enemy_start_locations.front();

    // move first overlord to the enemy base
    const sc2::Unit* overlord = overlords.front();
    if (overlord->orders.empty()) {
        Actions()->UnitCommand(overlord, sc2::ABILITY_ID::SMART, enemy_base);
    }

    sc2::Units enemies = observation->GetUnits(sc2::Unit::Alliance::Enemy, [enemy_base](const sc2::Unit& unit) {
        return Distance2D(unit.pos, enemy_base) < 10.0f; // consider units near the base
    });

    // count defense units
    int defense_units = 0;
    int defense_structures = 0;

    for (const auto& unit : enemies) {
        // count defense structures
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_BUNKER ||
            unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON ||
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_SPINECRAWLER) {
            defense_structures++;
        }
        // count defense units
        if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_MARINE ||
            unit->unit_type == sc2::UNIT_TYPEID::TERRAN_MARAUDER ||
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_ZERGLING ||
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_ROACH ||
            unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_ZEALOT) {
            defense_units++;
        }
    }

    std::cout << "defensive units found: " << defense_units << std::endl;
    return (defense_units <= 10 && defense_structures <= 2); // threshold for defense units/structures ?? completely random right now
}

// basic attack functions: https://github.com/Blizzard/s2client-api/blob/master/examples/common/bot_examples.cc
/* void Attack::AttackWithUnitType(sc2::UnitTypeID unit_type, const sc2::ObservationInterface* observation) {
    // retrieve units that match unit type
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type));
    // each unit attack
    for (const auto& unit : units) {
        AttackWithUnit(unit, observation);
    }
}

void Attack::AttackWithUnit(const sc2::Unit* unit, const sc2::ObservationInterface* observation) {
    // grabs general enemies
    sc2::Units enemy_units = observation->GetUnits(sc2::Unit::Alliance::Enemy);
    if (enemy_units.empty()) {
        return;
    }
    // no orders
    if (unit->orders.empty()) {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, enemy_units.front()->pos);
        return;
    }
    // order not attacking, make it attacking
    if (unit->orders.front().ability_id != sc2::ABILITY_ID::ATTACK) {
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, enemy_units.front()->pos);
    }
} */