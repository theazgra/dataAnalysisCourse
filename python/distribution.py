from math import sqrt,pi,exp
from vec import Vec

def load_iris_dataset(file):
    with open(file, 'r') as irisFile:
        dataset = [Vec(line.strip().split(',')) for line in irisFile]
        return dataset[1:]

def load_iris_dataset_wo_species(file):
    with open(file, 'r') as irisFile:
        dataset = [Vec(line.strip().split(',')[:-1]) for line in irisFile]
        return dataset[1:]

def eucld(a, b):
    return (a - b)
    #return sqrt((a - b)**2)

def get_mean(dataset, col):
    return sum([r[col] for r in dataset]) / float(len(dataset))

def get_var(dataset, col):
    mean = get_mean(dataset, col)
    var = sum([(eucld(r[col], mean)**2) for r in dataset]) / float(len(dataset))
    return var

def normal_distribution_point(x, mean, var):
    result = (1.0 / sqrt(2*pi*var)) * exp(-1*(((x-mean)**2)/(2*var)))
    return result

def get_normal_distribution(dataset, col):
    mean = get_mean(dataset, col)
    var = get_var(dataset, col)
    result = {}
    for vector in dataset:
        x = vector[col]
        result[x] = normal_distribution_point(x, mean, var)
    return result

def ff(val):
    return str(val).replace('.',',')

def main():
    cols = 4
    df = load_iris_dataset_wo_species("../data/iris.csv")
    #distributions = []
    
    for colIndex in range(0, cols):
        distribution = get_normal_distribution(df, colIndex)
        oF = "../results/distribution_{}.csv".format(colIndex)
        with open(oF, 'w') as outFile:
            for x in distribution:
                outFile.write("{0};{1}\n".format(ff(x), ff(distribution[x])))


if __name__ == "__main__":
    main()