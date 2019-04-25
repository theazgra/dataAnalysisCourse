using System.Collections.Generic;
using System.Linq;
using Troschuetz.Random;
using Troschuetz.Random.Generators;

namespace NetworkWizardLib.NWRandom
{
    public class WeightedRandom<T>
    {
        private TRandom _random;
        private IEnumerable<KeyValuePair<double, T>> _items;
        double _upperBound = 1.0;

        /// <summary>
        /// Create generator using probabilities of random items.
        /// </summary>
        /// <param name="weightedItems">Weights with theirs corresponding items.</param>
        /// <param name="generator">Generator to be used. Default: <see cref="MT19937Generator"/></param>
        public WeightedRandom(IEnumerable<KeyValuePair<double, T>> weightedItems, IGenerator generator =null)
        {
            _items = weightedItems.OrderByDescending(pair => pair.Key);
            double cummulativeSum = _items.Sum(x => x.Key);
            if (cummulativeSum < 1.0)
                _upperBound = cummulativeSum;

            _random = new TRandom(generator ?? new MT19937Generator());
        }

        /// <summary>
        /// Create <see cref="WeightedRandom"/> for indexes of weights.
        /// </summary>
        /// <param name="weights">Random probabilities of indexes.</param>
        /// <returns></returns>
        public static WeightedRandom<int> WeightedIndices(IEnumerable<double> weights)
        {
            int count = weights.Count();
            KeyValuePair<double, int>[] items = new KeyValuePair<double, int>[count];

            for (int i = 0; i < count; i++)
            {
                items[i] = new KeyValuePair<double, int>(weights.ElementAt(i), i);
            }

            return new WeightedRandom<int>(items);
        }

        /// <summary>
        /// Return random item.
        /// </summary>
        /// <returns>Random item.</returns>
        public T Next()
        {
            double rndVal = _random.NextDouble(_upperBound);
            double cumulative = 0.0;

            for (int i = 0; i < _items.Count(); i++)
            {
                cumulative += _items.ElementAt(i).Key;
                if (rndVal <= cumulative)
                {
                    return _items.ElementAt(i).Value;
                }
            }

            throw new System.Exception("You should never get here!");
            return default;
        }
    }
}
