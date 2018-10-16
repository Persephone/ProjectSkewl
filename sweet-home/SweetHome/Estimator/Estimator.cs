using System.Linq;

namespace SweetHome
{
    class Estimator
    {
        public void Estimate(string City, uint PostalCode, uint Zone, ref uint Price)
        {
            Estimations estimation = Estimations.EstimatorList.FirstOrDefault(e => e.City == City && e.PostalCode == PostalCode && e.Zone == Zone);
            Price = (uint)(Price * estimation.PriceFactor);
        }
    }
}
