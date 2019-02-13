"""
from sys import path
path.append("Directory from which I want to import file")
import file_from_appended_directory
"""

from Table.DataTable import DataTable
from Table.vec import Vec


def main():
    dt = DataTable("../data/iris.csv", delimiter=',')
    cols = (0,1,2,3)
    meanVector =  dt.calculate_mean_vector(cols)
    print("Mean vector: " + str(meanVector))

    dtVariance = dt.calculate_total_variance(meanVector, cols)
    print("Data table variance: " + str(dtVariance))

if __name__ == "__main__":
    main()
