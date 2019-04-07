#include "dice.h"
#include <string>
#include <ostream>

using namespace std;

Dice::Dice(void){
	this->base = 1;
	this->count = 2;
	this->sides = 3;
}

Dice::Dice(int base, int count, int sides){
	this->base = base;
	this->count = count;
	this->sides = sides;
}

Dice::Dice(string s){
	base = stoi(s.substr(0, s.find("+")));
	s.erase(0,s.find("+")+1);
	count = stoi(s.substr(0, s.find("d")));
	s.erase(0,s.find("d")+1);
	sides = stoi(s);
}

int Dice::roll(){
	int sum = base;
	for(int i = 0; i < count; i++){
		sum += rand()%sides + 1;
	}
	return sum;
}

ostream & operator<<(ostream &out, const Dice &d){
	return out << d.base << " + " << d.count << "d" << d.sides;
}