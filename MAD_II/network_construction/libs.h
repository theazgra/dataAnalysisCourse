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

#include <unordered_set>

#include "custom_types.h"
#include "utils.h"
#include "stack.h"
#include "io.h"

enum LinkageType
{
    LinkageType_Complete,
    LinkageType_Single,
    LinkageType_Average
};