#ifndef __P_Solver_H__
#define __P_Solver_H__
#include <vector>
#include "P_Collision.h"
class P_Solver {
public:
	virtual void Solve(std::vector<Collision>& collisions, float dt) = 0;
};
#endif //!__P_Solver_H__