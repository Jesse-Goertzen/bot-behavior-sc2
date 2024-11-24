#include "StateMachineManager.h"
#include "BasicSc2Bot.h"

// Moves us to the next state in the BotState list
void StateMachineManager::completeState() {
    current_state = static_cast<BotState>(current_state + 1);
};

// BUILD_FIRST_DRONE, BUILD_SECOND_DRONE, BUILD_THIRD_DRONE etc.
void StateMachineManager::BuildDrone(BasicSc2Bot& bot) {

    if (bot.unit_manager.BuildDrone(bot.observation, bot.actions)) {
        // State is complete
        std::cout << ">Completed state: " << current_state << std::endl;
        completeState();
    }

}

// BUILD_FIRST_OVERLORD
void StateMachineManager::BuildOverlord(BasicSc2Bot& bot) {

    if (bot.unit_manager.BuildOverlord(bot.observation, bot.actions)) {
        // State is complete
        std::cout << ">Completed state: " << current_state << std::endl;
        completeState();
    }
}

// FIRST_EXPAND
// void StateMachineManager::FirstExpand(BasicSc2Bot& bot) {

//     if (bot.observation->GetMinerals() > 300) {

//         if (TryExpand(sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){

//             // State is complete
//             std::cout << ">Completed state: " << current_state << std::endl;
//             completeState();

//         }
//         else {
//             std::cout << ">Failed state: " << current_state << std::endl;
//         }
        
//     }
// }