#ifndef STATE_MACHINE_MANAGER_H
#define STATE_MACHINE_MANAGER_H

class BasicSc2Bot;

class StateMachineManager {
public:

    // Moves us to the next state
    void completeState();

    // States of the bot, in order of completion
	enum BotState {
        PRESTART,
        START,
        PRE_FIRST_EXPANSION,
        FIRST_EXPANSION,
        POST_FIRST_EXPANSION,
        QUEENING,
        MORE_EXTRACTING,
        SATURATE_EXTRACTORS,
        ROACH_WARREN,
        BASE_DEFENSE,
        SECOND_EXPANSION,
        ROACHPOCALYPSE,
        ATTACK
	};

    void PrestartState(BasicSc2Bot& bot);
    void StartingState(BasicSc2Bot& bot);
    void PreFirstExpansionState(BasicSc2Bot& bot);
    void FirstExpansionState(BasicSc2Bot& bot);
    void PostFirstExpansionState(BasicSc2Bot& bot);
    void QueeningState(BasicSc2Bot& bot);
    void MoreExtractingState(BasicSc2Bot& bot);
    void SaturateExtractorsState(BasicSc2Bot& bot);
    void RoachWarrenState(BasicSc2Bot& bot);
    void BaseDefenseState(BasicSc2Bot& bot);
    void SecondExpansionState(BasicSc2Bot& bot);
    void RoachpocalypseState(BasicSc2Bot& bot);
    void AttackState(BasicSc2Bot& bot);

    // What state the bot is currently in
    BotState current_state;

private:


    
};


#endif