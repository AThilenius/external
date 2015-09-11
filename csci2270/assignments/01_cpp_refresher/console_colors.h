//
//  ConsoleColors.h
//  UTFramework
//
//  Created by Alec Thilenius on 4/20/15.
//  Copyright (c) 2015 Thilenius. All rights reserved.
//
#pragma once

#include <iostream>

enum ConsoleColor { White, Red, Yellow, Green, Blue };

std::ostream& operator<<(std::ostream& stream, ConsoleColor color);
