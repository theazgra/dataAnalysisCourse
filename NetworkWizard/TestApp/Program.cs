using NetworkWizardLib.NWNetwork;
using NetworkWizardLib.NWRandom;
using System;
using System.Diagnostics;

namespace TestApp
{
    class Program
    {
        static void Main(string[] args)
        {
            //RangeRandom rangeRandom = new RangeRandom(0, 10);
            //RandomChoice randomChoice = new RandomChoice(0.17);
            //WeightedRandom<int> randInd = WeightedRandom<int>.WeightedIndices(new double[] { 0.1, 0.9 });

            //for (int j = 0; j < 100; j++)
            //{
            //    Console.WriteLine($"{randInd.Next()}");
            //}

            NetworkMatrix nm = new NetworkMatrix(10);
            Console.WriteLine(nm);
        }
    }
}
