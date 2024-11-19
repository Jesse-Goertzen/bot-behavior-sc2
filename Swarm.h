#ifndef SWARM_H
#define SWARM_H

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <cmath>
#include <cstdint>
#include <sc2api/sc2_typeenums.h>

#include <math.h>

using UnitTargets = std::map<sc2::UNIT_TYPEID, int>;
class Swarm {
    public:
        class SwarmState {
            public:
            UnitTargets unitTargets;

            sc2::ObservationInterface* observation;
            SwarmState(UnitTargets targets) : unitTargets(targets) {}
            void setUnitTarget(sc2::UNIT_TYPEID typeID, int count) {
                unitTargets[typeID] = count;
            }

            int getUnitTarget(const sc2::UNIT_TYPEID typeID) {
                if (unitTargets.find(typeID) == unitTargets.end()) return -1;
                return unitTargets[typeID];
            }

            int getFoodRequirement() const { 
                float foodTotal = 0.0f;
                for (auto const &unit : unitTargets) {
                    foodTotal += getUnitFoodCost(unit.first) * unit.second;           
                }
                // foodCost for each type stored as a float, but can only get int values
                // of production so round up as needed.
                return static_cast<int>(std::ceil(foodTotal));
            }

            float getUnitFoodCost(sc2::UNIT_TYPEID typeID) const {
                // should maybe assert instead?
                if (static_cast<uint32_t>(typeID) > observation->GetUnitTypeData().size()) return 0.0;

                uint32_t type_int = static_cast<uint32_t>(typeID);
                return observation->GetUnitTypeData()[type_int].food_required;
            }

        };  

        Swarm(sc2::ObservationInterface* oi, sc2::ActionInterface* ai);
        void onStep();
        void changeState(SwarmState state);
    private:
        bool spawnUnit(sc2::UNIT_TYPEID, int count=1);
        bool spawnUnit(std::pair<sc2::UNIT_TYPEID, int> unitTarget);
        bool spawnOverlord();
        bool spawnHatchery();
        bool spawnDrone();
        bool spawnZergling();

        void updateUnitCounts();
        void checkFoodAndOverlords();
        void spawnUnits();
        
        sc2::ObservationInterface* observation;
        sc2::ActionInterface* actions;

        SwarmState state;

        sc2::Units larvae;
        sc2::Units drones;
        sc2::Units overlords;

        int targetCountDrones;
        int targetCountZerglings;

};


#endif