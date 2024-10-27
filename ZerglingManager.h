#ifndef ZERGLING_MANAGER_H_
#define ZERGLING_MANAGER_H_
#include <vector>

#include "sc2api/sc2_api.h"
#include "sc2_unit.h"

using namespace sc2;

class ZerglingManager {
    public:
        void onStep();
        void onGameStart();

        void setTargetNumber(int num);
        int getCount() const;
    private:
        std::vector<Unit*> zerglings; 
        int target;
};



#endif