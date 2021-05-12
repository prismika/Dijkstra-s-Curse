#include "dice.h"
#include <string>
#include <iostream>

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

void Dice::set(int base, int count, int sides){
	this->base = base;
	this->count = count;
	this->sides = sides;
}

int Dice::roll(){
	int sum = base;
	for(int i = 0; i < count; i++){
		sum += rand()%sides + 1;
	}
	return sum;
}

string Dice::toString(){
	string ret;
	ret += to_string(base);
	ret += "+";
	ret += to_string(count);
	ret += "d";
	ret += to_string(sides);

	return ret;
}

ostream & operator<<(ostream &out, const Dice &d){
	return out << d.base << " + " << d.count << "d" << d.sides;
}