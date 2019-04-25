using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using Troschuetz.Random;
using Troschuetz.Random.Generators;

namespace NetworkWizardLib.NWRandom
{
    public class RangeRandom
    {
        private readonly int min;
        private readonly int max;
        private readonly TRandom _random;

        /// <summary>
        /// Create new random generator, generation numbers in given range.
        /// </summary>
        /// <param name="incMin">Inclusive lower boundary.</param>
        /// <param name="incMax">Inclusive upper boundary.</param>
        /// <param name="generator">Generator to be used. Default: <see cref="MT19937Generator"/></param>
        public RangeRandom(int incMin, int incMax, IGenerator generator = null)
        {
            min = incMin;
            max = incMax;

            _random = new TRandom(generator ?? new MT19937Generator());
        }

        /// <summary>
        /// Get random int in set range.
        /// </summary>
        /// <returns>Random integer.</returns>
        public int Next() => _random.Next(min, max + 1);
    }
}
