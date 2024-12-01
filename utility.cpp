#include "utility.h"
#include "BasicSc2Bot.h"

// Get the available supply
float getAvailableSupply(BasicSc2Bot& bot) {
    float total_supply = bot.Observation()->GetFoodCap();
    float used_supply = bot.Observation()->GetFoodUsed();
    return total_supply - used_supply;
}

