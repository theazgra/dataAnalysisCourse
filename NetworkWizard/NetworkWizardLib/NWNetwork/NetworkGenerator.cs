using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NetworkWizardLib.NWRandom;

namespace NetworkWizardLib.NWNetwork
{
    public static class NetworkGenerator
    {
        public static UndirectedNetworkMatrix RandomNetwork(int vertexCount, float edgeProbability)
        {
            return GenerateRandomNetwork(vertexCount, edgeProbability);
        }

        public static UndirectedNetworkMatrix SymmetricRandomNetwork(int vertexCount)
        {

            float probability = (float)(Math.Log(vertexCount) / (double)vertexCount);
            return GenerateRandomNetwork(vertexCount, probability);
        }

        private static UndirectedNetworkMatrix GenerateRandomNetwork(int vertexCount, float edgeProbability)
        {
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(vertexCount);

            Parallel.ForEach(Enumerable.Range(0, vertexCount), row =>
            {
                RandomChoice edgeRandom = new RandomChoice(edgeProbability);
                for (int col = row + 1; col < vertexCount; col++)
                {
                    if (edgeRandom.NextChoice())
                    {
                        result[row, col] = 1.0f;
                        result[col, row] = 1.0f;
                    }
                }
            });

            Console.WriteLine($"RandomNetwork: {result.VertexCount()} vertices {result.EdgeCount()} edges.");
            return result;
        }

        public static UndirectedNetworkMatrix BarabasiAlbert(int initialSize, int finalVertexCount, int newEdgesInStep,
                                                             bool applyAging = false, float ageScaling = 0.0f)
        {
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(finalVertexCount);
            int currentSize = initialSize;

            List<int> vertexList = new List<int>();

            // Construct initial symmetric network _snake_
            for (int v = 0; v < initialSize; v++)
            {
                if (v != (initialSize - 1))
                {
                    result[v, v + 1] = 1.0f;
                    result[v + 1, v] = 1.0f;
                }
                vertexList.Add(v);

                if (v > 0 && v != (initialSize - 1))
                    vertexList.Add(v);
            }

            int newVertexId, neighbor;
            for (int step = 0; step < (finalVertexCount - initialSize); step++)
            {
                newVertexId = initialSize + step;

                double[] weights = new double[currentSize];
                List<int> neighbors = new List<int>(newEdgesInStep);

                for (int vertex = 0; vertex < currentSize; vertex++)
                {
                    double occurenceInVertexList = (float)vertexList.Count(x => x == vertex);
                    double weight = (occurenceInVertexList / (double)vertexList.Count);

                    float age = newVertexId - vertex;
                    double agedWeight = (weight * Math.Pow(age, (-1.0f * ageScaling)));

                    weights[vertex] = applyAging ? agedWeight : weight;
                }

                WeightedRandom<int> neighborRandom = WeightedRandom<int>.WeightedIndices(weights);

                for (int newNeighborStep = 0; newNeighborStep < newEdgesInStep; newNeighborStep++)
                {
                    do
                    {
                        neighbor = neighborRandom.Next();
                    } while (neighbors.Contains(neighbor));
                    neighbors.Add(neighbor);
                }
                Trace.Assert(neighbors.Count == newEdgesInStep);

                vertexList.Add(newVertexId);
                vertexList.Add(newVertexId);

                foreach (int newNeighbor in neighbors)
                {
                    vertexList.Add(newNeighbor);
                    result[newVertexId, newNeighbor] = 1.0f;
                    result[newNeighbor, newVertexId] = 1.0f;
                }
            }

            Console.WriteLine($"Barabasi-Albert: {result.VertexCount()} vertices {result.EdgeCount()} edges");
            return result;
        }

        public static UndirectedNetworkMatrix HolmeKim(int initialSize, int vertexCount, int newEdgesInStep, float probability)
        {
            RandomChoice holmeHimRandom = new RandomChoice(probability);

            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(vertexCount);
            int currentSize = initialSize;

            List<int> vertexList = new List<int>();

            // Construct initial symmetric network _snake_
            for (int v = 0; v < initialSize; v++)
            {
                if (v != (initialSize - 1))
                {
                    result[v, v + 1] = 1.0f;
                    result[v + 1, v] = 1.0f;
                }
                vertexList.Add(v);

                if (v > 0 && v != (initialSize - 1))
                    vertexList.Add(v);
            }

            int newVertexId, neighbor;
            for (int step = 0; step < (vertexCount - initialSize); step++)
            {
                newVertexId = initialSize + step;

                double[] weights = new double[currentSize];
                List<int> neighbors = new List<int>(newEdgesInStep);

                for (int vertex = 0; vertex < currentSize; vertex++)
                {
                    double occurenceInVertexList = (float)vertexList.Count(x => x == vertex);
                    double weight = (occurenceInVertexList / (double)vertexList.Count);
                    weights[vertex] = weight;
                }
                WeightedRandom<int> neighborRandom = WeightedRandom<int>.WeightedIndices(weights);

                bool doPA = false;
                int lastConnectedVertex = 0;

                for (int newNeighborStep = 0; newNeighborStep < newEdgesInStep; newNeighborStep++)
                {
                    doPA = (newNeighborStep == 0) ? true : !holmeHimRandom.NextChoice();

                    if (doPA)
                    {
                        do
                        {
                            neighbor = neighborRandom.Next();
                        } while (neighbors.Contains(neighbor));
                        neighbors.Add(neighbor);
                        lastConnectedVertex = neighbor;
                    }
                    else
                    {
                        var lastConnectedNeighbors = result.GetNeighbors(lastConnectedVertex);
                        RangeRandom rangeRandom = new RangeRandom(0, lastConnectedNeighbors.Count() - 1);
                        do
                        {
                            neighbor = lastConnectedNeighbors.ElementAt(rangeRandom.Next());
                        } while (neighbors.Contains(neighbor));
                        neighbors.Add(neighbor);
                        lastConnectedVertex = neighbor;
                    }
                }
                Trace.Assert(neighbors.Count == newEdgesInStep);

                vertexList.Add(newVertexId);
                vertexList.Add(newVertexId);

                foreach (int newNeighbor in neighbors)
                {
                    vertexList.Add(newNeighbor);
                    result[newVertexId, newNeighbor] = 1.0f;
                    result[newNeighbor, newVertexId] = 1.0f;
                }
            }

            Console.WriteLine($"Holme-Kim: {result.VertexCount()} vertices {result.EdgeCount()} edges");
            return result;
        }

        public static UndirectedNetworkMatrix Bianconi(int initialSize, int vertexCount, int newEdgesInStep, float probability)
        {
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(vertexCount);
            int currentSize = initialSize;

            // Construct initial symmetric network _snake_
            for (int v = 0; v < initialSize; v++)
            {
                if (v != (initialSize - 1))
                {
                    result[v, v + 1] = 1.0f;
                    result[v + 1, v] = 1.0f;
                }
            }

            int newVertexId, neighbor;
            RandomChoice chooseNeighborOfNeighborRandom = new RandomChoice(probability);
            RangeRandom vertexRandom;

            for (int step = 0; step < (vertexCount - initialSize); step++)
            {
                newVertexId = initialSize + step;
                List<int> neighbors = new List<int>(newEdgesInStep);
                vertexRandom = new RangeRandom(0, newVertexId - 1);

                int newRandomNeighbor = vertexRandom.Next();
                neighbors.Add(newRandomNeighbor);
                int lastConnectedVertex = newRandomNeighbor;

                for (int newNeighborStep = 0; newNeighborStep < newEdgesInStep - 1; newNeighborStep++)
                {
                    if (chooseNeighborOfNeighborRandom.NextChoice())
                    {
                        var lastConnectedNeighbors = result.GetNeighbors(lastConnectedVertex);
                        RangeRandom lastConnectedNeighborsRandom = new RangeRandom(0, lastConnectedNeighbors.Count() - 1);

                        do
                        {
                            neighbor = lastConnectedNeighborsRandom.Next();
                        } while (neighbors.Contains(neighbor));
                    }
                    else
                    {
                        do
                        {
                            neighbor = vertexRandom.Next();
                        } while (neighbors.Contains(neighbor));
                    }

                    neighbors.Add(neighbor);
                    lastConnectedVertex = neighbor;
                }

                Trace.Assert(neighbors.Count == newEdgesInStep);

                foreach (int newNeighbor in neighbors)
                {
                    result[newVertexId, newNeighbor] = 1.0f;
                    result[newNeighbor, newVertexId] = 1.0f;
                }
            }

            Console.WriteLine($"Bianconi: {result.VertexCount()} vertices {result.EdgeCount()} edges");
            return result;
        }

        private static UndirectedNetworkMatrix GenerateCopyingModelOrLink(int initialSize, int vertexCount, bool copy, double copyProbability)
        {
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(vertexCount);
            // Construct initial symmetric network _snake_
            for (int v = 0; v < initialSize; v++)
            {
                if (v != (initialSize - 1))
                {
                    result[v, v + 1] = 1.0f;
                    result[v + 1, v] = 1.0f;
                }
            }

            RangeRandom neighRandom;
            RandomChoice linkEndRandom = new RandomChoice(0.5);
            RandomChoice copyRandom = new RandomChoice(copyProbability);

            var edges = result.GetEdges();
            ValueTuple<int, int> edge;

            for (int step = 0; step < (vertexCount-initialSize); step++)
            {
                int newVertexId = initialSize + step;
                RangeRandom edgeRandom = new RangeRandom(0, edges.Count() - 1);
                edge = edges.ElementAt(edgeRandom.Next());

                int partner;
                if (copy)
                {
                    // Copy wth probability select edge.first with (1-probability) select edge.second random neigbor.
                    if (copyRandom.NextChoice())
                    {
                        partner = edge.Item1;
                    }
                    else
                    {
                        var edgeNeigh = result.GetNeighbors(edge.Item1);
                        neighRandom = new RangeRandom(0, edgeNeigh.Count() - 1);
                        partner = edgeNeigh.ElementAt(neighRandom.Next());
                    }
                }
                else
                {
                    partner = linkEndRandom.NextChoice() ? edge.Item1 : edge.Item2;
                }

                result[newVertexId, partner] = 1.0f;
                result[partner, newVertexId] = 1.0f;
            }
            Console.WriteLine($"Model {result.VertexCount()} vertices {result.EdgeCount()} edges");
            return result;
        }

        public static UndirectedNetworkMatrix CopyingModel(int initialSize, int vertexCount, float copyProbability)
        {
            return GenerateCopyingModelOrLink(initialSize, vertexCount, true, copyProbability);
        }

        public static UndirectedNetworkMatrix LinkSelectionModel(int initialSize, int vertexCount)
        {
            return GenerateCopyingModelOrLink(initialSize, vertexCount, false, 0);
        }
    }
}
