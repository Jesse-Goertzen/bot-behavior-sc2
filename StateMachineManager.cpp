#include "StateMachineManager.h"
#include "BasicSc2Bot.h"

// Moves us to the next state in the BotState list
void StateMachineManager::completeState() {
    current_state = static_cast<BotState>(current_state + 1);
};

// BUILD_FIRST_DRONE,
void StateMachineManager::BuildFirstDrone(BasicSc2Bot& bot) {

    if (bot.unit_manager.BuildDrone(bot.observation, bot.actions)) {
        std::cout << ">BUILT FIRST DRONE" << std::endl;
        // State is complete
        completeState();
    }

}

// BUILD_FIRST_OVERLORD
void StateMachineManager::BuildFirstOverlord(BasicSc2Bot& bot) {

    if (bot.unit_manager.BuildOverlord(bot.observation, bot.actions)) {
        std::cout << "BUILT FIRST OVERLORD" << std::endl;
        // State is complete
        bot.state_machine.completeState();
    }

}