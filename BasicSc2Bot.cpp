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

    // Get the observation object and store it in a pointer
    observation = Observation();

    // Set the current state
    current_state = BUILD_FIRST_DRONE;

    expansions_ = sc2::search::CalculateExpansionLocations(observation, Query());

    // https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L153C1-L155C40
    startLocation_ = observation->GetStartLocation();
    staging_location_ = startLocation_;

}

void BasicSc2Bot::OnStep() {
    // On each step update the amount of units. This updates larva, overlord and drone vectors in the class
    // Can get the size of each using larva.size() for example
    UpdateUnits();

    // Current supply
    int current_supply = observation->GetFoodUsed();

    sc2::Units town_halls;
    first_expansion = nullptr;

    // Max distance is for case of WAIT_FOR_HATCHERY but will remvoe it and put in own function later
    float max_distance = 0.0f;

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
            if (observation->GetMinerals() > 300) {
                if (TryExpand(sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){
                    std::cout << "CREATING EXPANSION" << std::endl;
                    current_state = WAIT_FOR_HATCHERY;

                }
                else {
                    std::cout << "Expansion failed" << std::endl;
                }
                
            }
            break;

        
        case WAIT_FOR_HATCHERY: 

            // Get all the town halls
            town_halls = observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());

            // Find the Hatchery farthest from the starting location.
            // Will have to change this if we want to select them based of build order or soemthing else
            for (const auto& town_hall : town_halls) {
                float distance = Distance2D(startLocation_, town_hall->pos);
                if (distance > max_distance) {
                    max_distance = distance;
                    first_expansion = town_hall;
                }
            }

            if (first_expansion) {
                // Check the Hatchery is built.
                if (first_expansion->build_progress >= 1.0f) {
                    std::cout << "Constucting hatchery... (100%)" << std::endl;
                    std::cout << "CREATED HATCHERY AT " << first_expansion->pos.x << ", " << first_expansion->pos.y << std::endl;

                    Actions()->UnitCommand(first_expansion, sc2::ABILITY_ID::TRAIN_DRONE);

                    current_state = IDLE;
                } 
                // Progress bar
                else if (first_expansion->build_progress == 0.25f) {
                    std::cout << "Constucting hatchery... (25%)" << std::endl;
                }
                else if (first_expansion->build_progress == 0.5f) {
                    std::cout << "Constucting hatchery... (50%)" << std::endl;
                }
                else if (first_expansion->build_progress == 0.75f) {
                    std::cout << "Constucting hatchery... (75%)" << std::endl;
                }
            }

            // While we wait for hatchery, check for when we reach 200 minerals to make a single spawning pool
            if (CountUnitType(observation, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
                if (TryBuildOnCreep(sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE)) {
                }
            }

            break;


            case IDLE:
            break;


    }
}

// Function that attempts to build a drone. Returns true or false if it succeeds
// Should update this to be a general use case for any unit
bool BasicSc2Bot::BuildDrone() {

    if (observation->GetMinerals() < 50) {
        // std::cout << "Not enough minerals to train drone! (" << observation_->GetMinerals() << "/50)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        // std::cout << "Not enough larva to train drone!" << std::endl;
        return false;
    }

    if (getAvailableSupply() < 1) {
        // std::cout << "Not enough supply to train drone!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}

bool BasicSc2Bot::BuildOverlord() {

    if (observation->GetMinerals() < 100) {
        // std::cout << "Not enough minerals to train overlord! (" << observation_->GetMinerals() << "/100)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        // std::cout << "Not enough larva to train overlord!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_OVERLORD);

    return true;
}

// Attempt to build a structure on creep from a random location on the creep
// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L1363
bool BasicSc2Bot::TryBuildOnCreep(sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type) {
    float rx = sc2::GetRandomScalar();
    float ry = sc2::GetRandomScalar();
    sc2::Point2D build_location = sc2::Point2D(startLocation_.x + rx * 15, startLocation_.y + ry * 15);

    if (observation->HasCreep(build_location)) {
        return TryBuildStructure(ability_type_for_structure, unit_type, build_location, false);
    }
    return false;
}


// Update the vectors of the units we have avalible
void BasicSc2Bot::UpdateUnits() {

    // Update the larva vector
    larva = observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    });

    // Update the drones vector
    drones = observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    });

    // Update overlords
    overlords = observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
    });

}

// Return a list of all the larva
std::vector<const sc2::Unit*> BasicSc2Bot::getLarva() {
        return larva;
}

// Return a list of all the drones
std::vector<const sc2::Unit*> BasicSc2Bot::getDrones() {
        return drones;
}

// Get the available supply
float BasicSc2Bot::getAvailableSupply() {
    float total_supply = observation->GetFoodCap();
    float used_supply = observation->GetFoodUsed();
    return total_supply - used_supply;

}

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L390
// Expands to nearest location and updates the start location to be between the new location and old bases.
bool BasicSc2Bot::TryExpand(sc2::AbilityID build_ability, sc2::UnitTypeID worker_type) {
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

// Attempt to build structure. Return if it fails or not
// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L316C1-L356C2
bool BasicSc2Bot::TryBuildStructure(sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion = false) {
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

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L158
size_t BasicSc2Bot::CountUnitType(const sc2::ObservationInterface* observation, sc2::UnitTypeID unit_type) {
    return observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type)).size();
}