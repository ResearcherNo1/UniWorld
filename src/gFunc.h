//Файл реализации глобальных функций
//
#include "constants.h"
#include "bot.h"
#include "world.hpp"
#include "graphics.h"

#ifndef GFUNC_H
#define GFUNC_H

void gInit();

void gUnpack(std::string fname);

void gSave(std::string fname);

void gStep();

#endif /* GFUNC_H */