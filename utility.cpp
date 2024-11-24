#include "utility.h"
#include "BasicSc2Bot.h"

// Get the available supply
float getAvailableSupply(BasicSc2Bot& bot) {
    float total_supply = bot.observation->GetFoodCap();
    float used_supply = bot.observation->GetFoodUsed();
    return total_supply - used_supply;
}

