#include "loading.h"


namespace DecisionTree
{
    Dataset<float> load_numerical(const char *fileName, const char separator, const size_t numberOfAttributes)
    {
        using namespace azgra::io;
        Dataset<float> dataset = {};
        std::vector<Transaction<float>> transactions =
                parse_by_lines<Transaction<float>>(fileName,
                                                   [separator, numberOfAttributes](
                                                           const azgra::string::SmartStringView<char> &cell)
                                                   {
                                                       Transaction<float> row;
                                                       row.attributeValues.resize(numberOfAttributes);

                                                       auto cells = cell.split(separator);
                                                       assert(cells.size() == numberOfAttributes + 1);
                                                       size_t i;
                                                       for (i = 0; i < numberOfAttributes; ++i)
                                                       {
                                                           row.attributeValues[i] = static_cast<float>(atof(
                                                                   cells[i].data()));
                                                       }
                                                       row.classIndex = atoi(cells[i].data());

                                                       return row;
                                                   });

        for (const Transaction<float> &t : transactions)
        {
            dataset.classes.insert(t.classIndex);
        }
        dataset.transactions = transactions;
        return dataset;
    }

    Dataset<char> load_categorial(const char *fileName, const char separator, const size_t numberOfAttributes)
    {
        using namespace azgra::io;
        Dataset<char> dataset = {};
        std::vector<Transaction<char>> transactions =
                parse_by_lines<Transaction<char>>(fileName,
                                                  [separator, numberOfAttributes](
                                                          const azgra::string::SmartStringView<char> &cell)
                                                  {
                                                      Transaction<char> row;
                                                      row.attributeValues.resize(numberOfAttributes);

                                                      auto cells = cell.split(separator);
                                                      assert(cells.size() == numberOfAttributes + 1);
                                                      size_t i;
                                                      for (i = 0; i < numberOfAttributes; ++i)
                                                      {
                                                          assert(cells[i].length() == 1);
                                                          row.attributeValues[i] = cells[i][0];
                                                      }
                                                      row.classIndex = atoi(cells[i].data());
                                                      return row;
                                                  });
        for (const Transaction<char> &t : transactions)
        {
            dataset.categories.insert(t.attributeValues.begin(), t.attributeValues.end());
            dataset.classes.insert(t.classIndex);
        }
        dataset.transactions = transactions;
        return dataset;
    }

}