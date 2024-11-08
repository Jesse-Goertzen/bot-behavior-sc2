#include "Attack.h"
#include <sc2api/sc2_typeenums.h>

Attack::Attack(sc2::ActionInterface *ai, sc2::ObservationInterface *oi)
    : actions(ai), observation(oi), attacking(false), target(nullptr) {}

void Attack::onStep() {
    if (!shouldAttack()) {
        return;
    }

    if (!attacking && target != nullptr) { // should attack + viable target
        attacking = true;

        actions->UnitCommand(zerglings, sc2::ABILITY_ID::ATTACK, target); // send zergling to atatck
    }
}

bool Attack::shouldAttack() {
    // conditions for attacking here...
    return zerglings.size() >= 10; // example, if we reach 10 zerglings, attack
}

void Attack::setTarget(const sc2::Unit* newTarget) {
    target = newTarget;
}

void Attack::setZerglings(const std::vector<sc2::Unit*>& newZerglings) {
    zerglings = newZerglings;
}
