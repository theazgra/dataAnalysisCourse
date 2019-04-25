using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace NetworkWizardLib.NWNetwork
{
    public static class NetworkIO
    {
        private static void Min(ref int src, int test)
        {
            if (test < src)
                src = test;
        }

        private static void Max(ref int src, int test)
        {
            if (test > src)
                src = test;
        }

        internal static IEnumerable<ValueTuple<int, int>> LoadEdgePairs(string file, char delimiter, out int startingAt, out int maxVertexId)
        {
            startingAt = 0;
            int min = int.MaxValue;
            maxVertexId = 0;

            List<ValueTuple<int, int>> edges = new List<(int, int)>();
            using (StreamReader reader = new StreamReader(path: file))
            {
                while (!reader.EndOfStream)
                {
                    int[] vertices = reader.ReadLine().Split(delimiter).Select(s => int.Parse(s)).ToArray();
                    edges.Add((vertices[0], vertices[1]));

                    Min(ref min, vertices[0]);
                    Min(ref min, vertices[1]);

                    Max(ref maxVertexId, vertices[0]);
                    Max(ref maxVertexId, vertices[1]);
                }
            }
            if (min != 0)
            {
                startingAt = -min;
                Console.WriteLine("Offset of {0}", startingAt);
            }
            return edges;
        }

        public static NetworkMatrix LoadNetworkFromEdgePairs(string file, char delimiter)
        {
            var edges = LoadEdgePairs(file, delimiter,  out int offset, out int maxVertexId);
            int dim = maxVertexId + offset;

            NetworkMatrix result = new NetworkMatrix(dim);
            int u, v;
            foreach (var edge in edges)
            {
                u = edge.Item1 + offset;
                v = edge.Item2 + offset;
                result.Data[u, v] = 1.0f;
                result.Data[v, u] = 1.0f;
            }
            return result;
        }

        internal static void SaveEdgePairs(string file, IEnumerable<ValueTuple<int, int>> edges)
        {
            using (StreamWriter writer = new StreamWriter(path: file, append: false, encoding: Encoding.UTF8))
            {
                foreach (var edge in edges)
                {
                    writer.WriteLine($"{edge.Item1};{edge.Item2}");
                }
            }
        }

        public static void SaveNetwork(string file, NetworkMatrix network) => SaveEdgePairs(file, network.GetEdges());

    }
}
