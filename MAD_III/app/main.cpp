#include "project/ecsv_loader.h"

int main(int, char **)
{
    const auto csvData = ECsvLoader::load_csv_columns("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/car_import.csv", true);
    return 0;
}