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
            //UndirectedNetworkMatrix random = NetworkGenerator.RandomNetwork(1000,0.005f);
            Stopwatch s = new Stopwatch();
            s.Start();
            UndirectedNetworkMatrix random = NetworkGenerator.LinkSelectionModel(3, 1000);
            s.Stop();

            //545
            //280
            //46 000
            Console.WriteLine(s.ElapsedMilliseconds);
        }
    }
}
