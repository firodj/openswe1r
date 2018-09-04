// StarWars.hpp
#pragma once

#include "Game.hpp"

class StarWars: public Game
{
public:
  StarWars();
  virtual ~StarWars();
  
  virtual int Run();
  
  void CompileShader();
};
