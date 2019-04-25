#pragma once

#include <assert.h>
#include <random>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <math.h>
#include <queue>
#include <algorithm>

#include "utils.h"

typedef unsigned int uint;

enum LinkageType
{
    LinkageType_Complete,
    LinkageType_Single,
    LinkageType_Average
};