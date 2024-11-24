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
        BUILD_FIRST_DRONE,
        BUILD_FIRST_OVERLORD,
        BUILD_SECOND_DRONE,
        BUILD_THIRD_DRONE,
        FIRST_EXPAND,
        WAIT_FOR_HATCHERY,
        IDLE
	};

    // What state the bot is currently in
    BotState current_state;

private:


    
};


#endif