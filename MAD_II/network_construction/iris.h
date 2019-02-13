#pragma once
#include <vector>
//sepal_length,sepal_width,petal_length,petal_width,species
struct IrisRecord
{
    float sepalLen;
    float sepalWidth;
    float petalLen;
    float petalWidth;
    const char *species;

    IrisRecord()
    {
        // Emptry constructor
    }
    IrisRecord(float sl, float sw, float pl, float pw, const char *name) : sepalLen(sl),
                                                                           sepalWidth(sw),
                                                                           petalLen(pl),
                                                                           petalWidth(pw),
                                                                           species(name) {}

    float euclid_distance(const IrisRecord &other)
    {
        float result = sqrt((pow((other.sepalLen - sepalLen), 2.0f) +
                             pow((other.sepalWidth - sepalWidth), 2.0f) +
                             pow((other.petalLen - petalLen), 2.0f) +
                             pow((other.petalWidth - petalWidth), 2.0f)));
        return result;
    }

    float gauss_distance(const IrisRecord &other)
    {
        float euclid = euclid_distance(other);
        float result = exp((-euclid / 2.0f));
        return result;
    }
};