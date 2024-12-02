#include "BasicSc2Bot.h"

#include "header_files.h"
#include "utility.h"

#include "StateMachineManager.h"
#include "UnitManager.h"

void BasicSc2Bot::OnGameStart() {

    // Get the observation object and store it in a pointer
    observation = Observation();

    // Get the action interface and store it as a pointer
    actions = Actions();

    // Query interface store as pointer
    query = Query();

    // Set the current state
    state_machine.current_state = StateMachineManager::START;

    // Get all expansion locations
    expansions_ = sc2::search::CalculateExpansionLocations(observation, query);

    // https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L153C1-L155C40
    // Set the start location
    startLocation_ = observation->GetStartLocation();
    staging_location_ = startLocation_;

    unit_manager.OnGameStart(*this);

}

void BasicSc2Bot::OnStep() {
    // Switch case for all the steps we will do
    switch (state_machine.current_state) {
        
        case StateMachineManager::START:
            // std::cout << "Starting State" << std::endl;
            state_machine.StartingState(*this);
            break;

        case StateMachineManager::PRE_FIRST_EXPANSION:
            // std::cout << "Pre Expansion State" << std::endl;
            state_machine.PreFirstExpansionState(*this);
            break;

        case StateMachineManager::FIRST_EXPANSION:
            // std::cout << "Expansion State" << std::endl;
            state_machine.FirstExpansionState(*this);
            break;

        case StateMachineManager::POST_FIRST_EXPANSION:
            // std::cout << "Post Expansion State" << std::endl;
            state_machine.PostFirstExpansionState(*this);
            break;

        case StateMachineManager::QUEENING:
            // std::cout << "Queen State" << std::endl;
            state_machine.QueeningState(*this);
            attack.ScoutWithOverlord(*this);
            break;

        case StateMachineManager::MORE_EXTRACTING:
            // std::cout << "More Extracting State" << std::endl;
            state_machine.MoreExtractingState(*this);
            break;
        case StateMachineManager::SATURATE_EXTRACTORS:
            // std::cout << "Saturate Extractors State" << std::endl;
            state_machine.SaturateExtractorsState(*this);
            break;

        case StateMachineManager::ROACH_WARREN:
            state_machine.RoachWarrenState(*this);
            break;

        case StateMachineManager::BASE_DEFENSE:
            state_machine.BaseDefenseState(*this);
            break;

        case StateMachineManager::SECOND_EXPANSION:
            state_machine.SecondExpansionState(*this);
            break;

        case StateMachineManager::ROACHPOCALYPSE:
            state_machine.RoachpocalypseState(*this);
            break;

        case StateMachineManager::ATTACK:
            state_machine.AttackState(*this);
            attack.RoachRush(*this);
            break;
    }
}

void BasicSc2Bot::OnBuildingConstructionComplete(const sc2::Unit* unit) {
    if (unit->alliance != sc2::Unit::Alliance::Self) return;

    if (unit->unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY) {
        unit_manager.bases.push_back({unit->tag, true});
    }

    if (unit->unit_type == sc2::UNIT_TYPEID::ZERG_EXTRACTOR) {
        unit_manager.extractors.push_back({unit->tag, true});
    }
}
