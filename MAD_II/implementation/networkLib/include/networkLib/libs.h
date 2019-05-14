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
#include <networkLib/types.h>

namespace azgra::networkLib
{

enum LinkageType
{
    LinkageType_Complete,
    LinkageType_Single,
    LinkageType_Average
};

};