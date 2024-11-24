#include "utility.h"

// Get the available supply
float getAvailableSupply(const sc2::ObservationInterface* observation) {
    float total_supply = observation->GetFoodCap();
    float used_supply = observation->GetFoodUsed();
    return total_supply - used_supply;
}