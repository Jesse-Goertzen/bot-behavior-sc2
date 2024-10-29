#include "Swarm.h"
#include <sc2api/sc2_typeenums.h>

Swarm::Swarm(sc2::ObservationInterface *oi, sc2::ActionInterface *ai) : observation(oi), actions(ai) { } 

void Swarm::onStep() {
    updateUnits();
    if (observation->GetFoodCap() < state.getFoodRequirement) {
        // build more overlords
        actions->UnitCommand(larvae.back(), sc2::ABILITY_ID::TRAIN_OVERLORD);
        larvae.pop_back();
    }

    if (drones.size() < state.targetCountDrones) {
        if (!larvae.empty()) {
            // should probably check how many drones are assigned to each
            // hatchery to see if we actually need more
            // Probably should use all available larvae for drones 
            actions->UnitCommand(larvae.back(), sc2::ABILITY_ID::TRAIN_DRONE);
        }
    }


}

void Swarm::updateUnits() {
    sc2::Units units = observation->GetUnits(sc2::Unit::Alliance::Self);
    for (const auto unit : units) {
        sc2::UNIT_TYPEID type = unit->unit_type;
        switch (type) {
            case sc2::UNIT_TYPEID::ZERG_DRONE:
                drones.push_back(unit);
                break;
            
            case sc2::UNIT_TYPEID::ZERG_LARVA:
                larvae.push_back(unit); 
                break;

            case sc2::UNIT_TYPEID::ZERG_OVERLORD:
                overlords.push_back(unit);
                break;        
        }        
    }
}

bool Swarm::spawnDrone() {
    if (larvae.empty()) return false;

}


bool Swarm::spawnOverlord() {
    if (larvae.empty()) return false;

}

bool spawnUnit(sc2::UNIT_TYPEID, int count=1) {

}