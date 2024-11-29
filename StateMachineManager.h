#ifndef STATE_MACHINE_MANAGER_H
#define STATE_MACHINE_MANAGER_H

class BasicSc2Bot;

class StateMachineManager {
public:

    // Moves us to the next state
    void completeState();

    // States that just build a drone
    void BuildDrone(BasicSc2Bot& bot);

    // Build Overlord
    void BuildOverlord(BasicSc2Bot& bot);

    // First expansion attempt
    void FirstExpand(BasicSc2Bot& bot);

    // What we do while we wait for the first hatchery
    void WaitForHatchery(BasicSc2Bot& bot);

    // States of the bot, in order of completion
	enum BotState {
        START,
        PRE_FIRST_EXPANSION,
        FIRST_EXPANSION,
        POST_FIRST_EXPANSION,
        QUEENING,
        MORE_EXTRACTING,

	};

    void StartingState(BasicSc2Bot& bot);
    void PreFirstExpansionState(BasicSc2Bot& bot);
    void FirstExpansionState(BasicSc2Bot& bot);
    void PostFirstExpansionState(BasicSc2Bot& bot);
    void QueeningState(BasicSc2Bot& bot);
    void MoreExtractingState(BasicSc2Bot& bot);

    void RoachWarrenState(BasicSc2Bot& bot);
    void BaseDefenseState(BasicSc2Bot& bot);

    // What state the bot is currently in
    BotState current_state;

private:


    
};


#endif