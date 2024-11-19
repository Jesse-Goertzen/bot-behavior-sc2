#include "Swarm.h"
#include <sc2api/sc2_typeenums.h>

Swarm::Swarm(sc2::ObservationInterface *oi, sc2::ActionInterface *ai) : observation(oi), actions(ai) { } 

void Swarm::onStep() {
    updateUnitCounts();
    // will need to check if we have an overlord hatching already
    if (state.getFoodRequirement() > observation->GetFoodCap()) {
        spawnUnit(sc2::UNIT_TYPEID::ZERG_OVERLORD);
    }
    spawnUnits();
}

void 

void Swarm::updateUnitCounts() {
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

void 
void Swarm::spawnUnits() {
    for (auto &unitTarget : state.unitTargets) {
        spawnUnit(unitTarget.first, unitTarget.second);
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

bool spawnUnit(std::pair<sc2::UNIT_TYPEID, int> unitTarget) {

}