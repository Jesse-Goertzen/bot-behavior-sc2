#include "Swarm.h"

Swarm::Swarm(sc2::ObservationInterface *oi, sc2::ActionInterface *ai) : observation(oi), actions(ai) { } 

void Swarm::onStep() {
    updateUnits();
    if (observation->GetFoodCap() < targetCountDrones + targetCountZerglings) {
        // build more overlords
        actions->UnitCommand(larvae.front(), sc2::ABILITY_ID::TRAIN_OVERLORD);
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

void Swarm::spawnDrone() {

}