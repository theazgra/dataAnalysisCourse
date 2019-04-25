using Troschuetz.Random;
using Troschuetz.Random.Generators;

namespace NetworkWizardLib.NWRandom
{
    public class RandomChoice
    {
        private readonly TRandom _random;
        private readonly double _trueProbability;

        /// <summary>
        /// Create new random generator, generating random boolean.
        /// </summary>
        /// <param name="generator">Generator to be used. Default: <see cref="MT19937Generator"/></param>
        public RandomChoice(double trueProbability, IGenerator generator = null)
        {
            _trueProbability = trueProbability;
            _random = new TRandom(generator ?? new MT19937Generator());
        }

        /// <summary>
        /// Generate random boolean with probability.
        /// </summary>
        /// <returns>Random bool.</returns>
        public bool NextChoice()
        {
            double rndVal = _random.NextDouble();
            return (rndVal <= _trueProbability); 
        }

        public override string ToString()
        {
            return $"RandomChoice generator, true probability: {_trueProbability}";
        }
    }
}
