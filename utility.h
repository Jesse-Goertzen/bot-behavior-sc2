#ifndef UTILITY_H
#define UTILITY_H

#include "header_files.h"

// Forward declaration so we dont use the whole include and have a circular dependancy
class BasicSc2Bot;

// Get available supply
float getAvailableSupply(BasicSc2Bot& bot);

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

struct IsVespeneGeyser {
    bool operator()(const sc2::Unit& unit) {
        switch (unit.unit_type.ToType()) {
            case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER: return true;
            case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER: return true;
            case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER: return true;
            default: return false;
        }
    }
};

#endif