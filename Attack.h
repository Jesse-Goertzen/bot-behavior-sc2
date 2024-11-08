#ifndef ATTACK_H
#define ATTACK_H

#include <sc2api/sc2_api.h>
#include <vector>

class Attack {
public:
    Attack(sc2::ActionInterface *ai, sc2::ObservationInterface *oi);
    void onStep(); // Main attack logic loop
    void setTarget(const sc2::Unit* target); // Set the target for attack
    void setZerglings(const std::vector<sc2::Unit*>& zerglings); // Set Zerglings for attacking
    bool shouldAttack(); // Determines if we have enough units to attack

private:
    sc2::ActionInterface *actions;
    sc2::ObservationInterface *observation;
    sc2::Units zerglings; // The list of Zerglings available for attacking
    const sc2::Unit* target; // The enemy target to attack
    bool attacking; // Whether we are currently attacking
};

#endif // ATTACK_H

