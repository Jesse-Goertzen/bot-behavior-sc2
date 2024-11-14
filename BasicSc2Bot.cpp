#include "BasicSc2Bot.h"

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L62C1-L76C3
struct IsTownHall {
    bool operator()(const sc2::Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case sc2::UNIT_TYPEID::ZERG_HATCHERY: return true;
            case sc2::UNIT_TYPEID::ZERG_LAIR: return true;
            case sc2::UNIT_TYPEID::ZERG_HIVE : return true;
            default: return false;
        }
    }
};


void BasicSc2Bot::OnGameStart() {

    std::cout << "Script starting " << std::endl;

    // Set the current state
    current_state = BUILD_FIRST_DRONE;

    expansions_ = sc2::search::CalculateExpansionLocations(Observation(), Query());

    // https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L153C1-L155C40
    startLocation_ = Observation()->GetStartLocation();
    staging_location_ = startLocation_;
}

void BasicSc2Bot::OnStep() {
    // On each step update the amount of units. This updates larva, overlord and drone vectors in the class
    // Can get the size of each using larva.size() for example
    UpdateUnits();

    // Current supply
    int current_supply = Observation()->GetFoodUsed();

    // Switch case for all the steps we will do
    switch (current_state) {

        // Start by building a drone
        case BUILD_FIRST_DRONE:
            // Attempt to build drone
            if (BuildDrone()) {
                std::cout << "BUILT FIRST DRONE" << std::endl;
                current_state = BUILD_OVERLORD;
            }
            break;

        // Then build a overlord when possible to make room for more units
        case BUILD_OVERLORD:
            // Attempt to build overlord
            if (BuildOverlord()) {
                std::cout << "BUILT FIRST OVERLORD" << std::endl;
                current_state = BUILD_SECOND_DRONE;
            }
            break;

        // Build two more drones
        case BUILD_SECOND_DRONE:
            // Attempt to build drone
            if (BuildDrone()) {
                std::cout << "BUILT SECOND DRONE" << std::endl;
                current_state = BUILD_THIRD_DRONE;
            }
            break;
        case BUILD_THIRD_DRONE:
            // Attempt to build drone
            if (BuildDrone()) {
                std::cout << "BUILT THIRD DRONE" << std::endl;
                current_state = EXPAND;
            }
            break;


        case EXPAND:
            // Attempt to expand
            if (Observation()->GetMinerals() > 300) {
                TryExpand(sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE);
                std::cout << "EXPANDED" << std::endl;
                current_state = IDLE;
            }
            break;

        case IDLE:
            // std::cout << "Idle..." << std::endl;

        break;

    }
}

// // Function to build a hatchery from a drone
// bool BasicSc2Bot::BuildHatchery() {

//     // Find an drone to use
//     auto drones = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
//         return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
//     });
//     if (drones.empty()) {
//         return false;
//     }

//     // Find the nearest expansion location
//     sc2::Point2D hatchery_location = GetNearestExpansion();

//     // Send the drone to build a Hatchery at the nearest location
//     Actions()->UnitCommand(drones.front(), sc2::ABILITY_ID::BUILD_HATCHERY, hatchery_location);
//     std::cout << "Built Hatchery!" << std::endl;
//     return true;
// }

// Takes the current state and finds what the next one will be
// BotState BasicSc2Bot::NextState(BotState current) {
//     // using UnderlyingType = std::underlying_type_t<BotState>;
//     // UnderlyingType nextValue = (static_cast<UnderlyingType>(current) + 1) % static_cast<UnderlyingType>(BotState::Count);
//     // return static_cast<BotState>(nextValue);
// }


// Function that attempts to build a drone. Returns true or false if it succeeds
// Should update this to be a general use case for any unit
bool BasicSc2Bot::BuildDrone() {

    if (Observation()->GetMinerals() < 50) {
        std::cout << "Not enough minerals to train drone! (" << Observation()->GetMinerals() << "/50)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        std::cout << "Not enough larva to train drone!" << std::endl;
        return false;
    }

    if (getAvailableSupply() < 1) {
        std::cout << "Not enough supply to train drone!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}

bool BasicSc2Bot::BuildOverlord() {

    if (Observation()->GetMinerals() < 100) {
        std::cout << "Not enough minerals to train overlord! (" << Observation()->GetMinerals() << "/100)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        std::cout << "Not enough larva to train overlord!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_OVERLORD);

    return true;
}

// Update the vectors of the units we have avalible
void BasicSc2Bot::UpdateUnits() {

    // Update the larva vector
    larva = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    });

    // Update the drones vector
    drones = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    });

    // Update overlords
    overlords = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
    });

}

std::vector<const sc2::Unit*> BasicSc2Bot::getLarva() {
        return larva;
}

std::vector<const sc2::Unit*> BasicSc2Bot::getDrones() {
        return drones;
}

// Get the available supply
float BasicSc2Bot::getAvailableSupply() {

    float total_supply = Observation()->GetFoodCap();
    float used_supply = Observation()->GetFoodUsed();
    return total_supply - used_supply;

}

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L390
// Expands to nearest location and updates the start location to be between the new location and old bases.
bool BasicSc2Bot::TryExpand(sc2::AbilityID build_ability, sc2::UnitTypeID worker_type) {
    const sc2::ObservationInterface* observation = Observation();
    float minimum_distance = std::numeric_limits<float>::max();
    sc2::Point3D closest_expansion;
    for (const auto& expansion : expansions_) {
        float current_distance = Distance2D(startLocation_, expansion);
        if (current_distance < .01f) {
            continue;
        }

        if (current_distance < minimum_distance) {
            if (Query()->Placement(build_ability, expansion)) {
                closest_expansion = expansion;
                minimum_distance = current_distance;
            }
        }
    }
    //only update staging location up till 3 bases.
    if (TryBuildStructure(build_ability, worker_type, closest_expansion, true) && observation->GetUnits(sc2::Unit::Self, IsTownHall()).size() < 4) {
        staging_location_ = sc2::Point3D(((staging_location_.x + closest_expansion.x) / 2), ((staging_location_.y + closest_expansion.y) / 2),
            ((staging_location_.z + closest_expansion.z) / 2));
        return true;
    }
    return false;

}

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L316C1-L356C2
bool BasicSc2Bot::TryBuildStructure(sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion = false) {

    const sc2::ObservationInterface* observation = Observation();
    sc2::Units workers = observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type));

    //if we have no workers Don't build
    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // If no worker is already building one, get a random worker to build one
    const sc2::Unit* unit = GetRandomEntry(workers);

    // Check to see if unit can make it there
    if (Query()->PathingDistance(unit, location) < 0.1f) {
        return false;
    }
    if (!isExpansion) {
        for (const auto& expansion : expansions_) {
            if (Distance2D(location, sc2::Point2D(expansion.x, expansion.y)) < 7) {
                return false;
            }
        }
    }
    // Check to see if unit can build there
    if (Query()->Placement(ability_type_for_structure, location)) {
        Actions()->UnitCommand(unit, ability_type_for_structure, location);
        return true;
    }
    return false;
}