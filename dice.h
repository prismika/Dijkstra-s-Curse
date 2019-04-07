#ifndef DICE_H
#define DICE_H

#include <string>

using namespace std;

class Dice{
private:
	int base;
	int count;
	int sides;
public:
	Dice(void);
	Dice(int base, int count, int sides);
	Dice(string s);
	int roll();
	friend std::ostream & operator<<(std::ostream &out, const Dice &d);
};

#endif