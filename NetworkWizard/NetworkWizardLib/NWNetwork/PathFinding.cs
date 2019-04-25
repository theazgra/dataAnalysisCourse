using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NetworkWizardLib.NWNetwork
{
    internal static class PathFinding
    {
        private class BfsNodeInfo : IEquatable<BfsNodeInfo>
        {
            public int Vertex { get; set; }
            public BfsNodeInfo PrevNode { get; set; }
            public bool Visited { get; set; } = false;

            public override bool Equals(object obj)
            {
                if (obj is BfsNodeInfo node)
                    return Equals(node);
                return base.Equals(obj);
            }
            public bool Equals(BfsNodeInfo other)
            {
                return Vertex == other.Vertex;
            }

            public override int GetHashCode()
            {
                return HashCode.Combine(Vertex);
            }
        }

        private class DijkstraNodeInfo : IEquatable<DijkstraNodeInfo>
        
        {
            public int Vertex { get; set; }
            public DijkstraNodeInfo Previous { get; set; }
            public float BestPathDistance { get; set; } = float.PositiveInfinity;
            public bool Visited { get; set; } = false;

            public override bool Equals(object obj)
            {
                if (obj is DijkstraNodeInfo node)
                    return Equals(node);
                return base.Equals(obj);
            }
            public bool Equals(DijkstraNodeInfo other)
            {
                return (Vertex == other.Vertex);
            }

            public override int GetHashCode()
            {
                return HashCode.Combine(Vertex);
            }
        }


        internal static float BfsPath(UndirectedNetworkMatrix matrix, int fromVertex, int toVertex)
        {
            Dictionary<int, BfsNodeInfo> bfsNodes = new Dictionary<int, BfsNodeInfo>(matrix.Dimension);
            for (int i = 0; i < matrix.Dimension; i++)
                bfsNodes.Add(i, new BfsNodeInfo() { Vertex = i });

            Queue<BfsNodeInfo> queue = new Queue<BfsNodeInfo>();
            queue.Enqueue(bfsNodes[fromVertex]);

            bool found = false;

            while (queue.Count > 0)
            {
                BfsNodeInfo current = queue.Dequeue();
                if (current.Vertex == toVertex)
                {
                    found = true;
                    break;
                }

                IEnumerable<int> neighborhood = matrix.GetNeighbors(current.Vertex);
                foreach (int neighbor in neighborhood)
                {
                    BfsNodeInfo neighborNode = bfsNodes[neighbor];
                    if (!neighborNode.Visited)
                    {
                        queue.Enqueue(neighborNode);
                        neighborNode.PrevNode = current;
                        neighborNode.Visited = true;
                    }
                }
            }
            if (!found)
                return float.PositiveInfinity;
            else
            {
                float result = 0.0f;
                BfsNodeInfo currentNode = bfsNodes[toVertex];
                BfsNodeInfo fromNode = bfsNodes[fromVertex];
                while (currentNode.Vertex != fromNode.Vertex)
                {
                    ++result;
                    currentNode = currentNode.PrevNode;
                }
                return result;
            }
        }

      
        private static void AddNewBestDistance(List<DijkstraNodeInfo> unvisited, DijkstraNodeInfo previous, DijkstraNodeInfo current, float distance)
        {
            current.Previous = previous;
            current.BestPathDistance = distance;

            if (!unvisited.Contains(current))
            {
                unvisited.Add(current);
            }
            unvisited = unvisited.OrderBy(v => v.BestPathDistance).ToList();
        }

        private static DijkstraNodeInfo GetTopUnvisitedVertex(List<DijkstraNodeInfo> unvisited) => unvisited.Count > 0 ? unvisited.First() : null;

        internal static float DijkstraPath(UndirectedNetworkMatrix matrix, int fromVertex, int toVertex)
        {

            Dictionary<int, DijkstraNodeInfo> nodes = new Dictionary<int, DijkstraNodeInfo>(matrix.Dimension);
            for (int i = 0; i < matrix.Dimension; i++)
                nodes.Add(i, new DijkstraNodeInfo() { Vertex = i });

            List<DijkstraNodeInfo> unvisited = new List<DijkstraNodeInfo>();
            DijkstraNodeInfo start = nodes[fromVertex];
            DijkstraNodeInfo end = nodes[toVertex];
            AddNewBestDistance(unvisited, null, start , 0);


            foreach (int neighbor in matrix.GetNeighbors(fromVertex))
            {
                AddNewBestDistance(unvisited, start, nodes[neighbor], float.PositiveInfinity);
            }

            while (!end.Visited)
            {
                DijkstraNodeInfo current = GetTopUnvisitedVertex(unvisited);

                IEnumerable<int> neigborhood = matrix.GetNeighbors(current.Vertex);
                foreach (int neighbor in neigborhood)
                {
                    DijkstraNodeInfo neighborNode = nodes[neighbor];
                    if (neighborNode.Visited)
                        continue;


                    float distanceToVertex = current.BestPathDistance + matrix[current.Vertex, neighborNode.Vertex];
                    if (distanceToVertex < neighborNode.BestPathDistance)
                    {
                        AddNewBestDistance(unvisited, current, neighborNode, distanceToVertex);
                    }
                }
                current.Visited = true;
                unvisited.Remove(current);
            }

            float result = end.BestPathDistance;
            return result;
        }
    }
}
