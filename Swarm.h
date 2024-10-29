#ifndef SWARM_H
#define SWARM_H

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

class Swarm {
    public:
        Swarm(sc2::ObservationInterface* oi, sc2::ActionInterface* ai);
        void onStep();
        void changeState(SwarmState state);

    private:
        void spawnOverlord();
        void spawnHatchery();
        void spawnDrone();

        void updateUnits();
        
        sc2::ObservationInterface* observation;
        sc2::ActionInterface* actions;

        SwarmState state;

        sc2::Units larvae;
        sc2::Units drones;
        sc2::Units overlords;

        int targetCountDrones;
        int targetCountZerglings;

};


class SwarmState {
    int targetCountDrones;
    int targetCountZerglings;
    
}

#endif