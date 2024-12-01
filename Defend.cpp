#include "Defend.h"

// Helper function to calculate distance between two points
float Distance(const sc2::Point3D& p1, const sc2::Point3D& p2) {
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Get all the enemies within 10 spaces of any of our bases
void Defend::GetEnemies(const sc2::ObservationInterface* observation) {
    enemies.clear();

    // Iterate over all enemy units
    for (const auto& enemy : observation->GetUnits(sc2::Unit::Alliance::Enemy)) {
        if (!enemy->is_alive) continue; // Skip dead enemies

        // Check if the enemies close to any of our bases
        for (const auto& base : observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsTownHall())) {
            if (Distance(enemy->pos, base->pos) <= 10.0f) { // Change this 10 value accordingly
                enemies.push_back(enemy);
                break; // Break inner loop, we only need to add this enemy once
            }
        }
    }
}

// Get all of our units that can defend
void Defend::GatherDefenders(const sc2::ObservationInterface* observation) {
    defenders.clear();

    // Collect all queens, zerglings, and roaches
    for (const auto& unit : observation->GetUnits(sc2::Unit::Alliance::Self)) {
        if (unit->unit_type == sc2::UNIT_TYPEID::ZERG_QUEEN || 
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_ZERGLING ||
            unit->unit_type == sc2::UNIT_TYPEID::ZERG_ROACH) {
            defenders.push_back(unit);
        }
    }
}

// Function to be called every step that would defend against any nearby attackers
void Defend::DefendAgainstAttackers(const sc2::ObservationInterface* observation) {
    // Find nearby enemies
    GetEnemies(observation);

    // If no enemies are within range just do nothing and return
    if (enemies.empty()) {
        return;
    }

    // Else if enemies found, gather defenders (queens and zerglings)
    GatherDefenders(observation);

    // Get defenders to attack the enemies
    for (const auto& defender : defenders) {
        Actions()->UnitCommand(defender, sc2::ABILITY_ID::ATTACK, enemies[0]->pos);
    }
}