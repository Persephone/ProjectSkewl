using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SweetHome
{
    class Estimations
    {
        public Estimations(MySqlConnection connection)
        {
            Connection = connection;
        }

        MySqlConnection Connection { get; set; }

        public string City         { get; set; }
        public uint PostalCode     { get; set; }
        public uint Zone           { get; set; }
        public float PriceFactor   { get; set; }

        public static List<Estimations> EstimatorList = new List<Estimations>();

        public void LoadCases()
        {
            Connection.Open();

            MySqlCommand cmd = new MySqlCommand("SELECT City, PostalCode, Zone, PriceFactor FROM AreaPrices", Connection);
            MySqlDataReader Reader = cmd.ExecuteReader();

            while (Reader.Read())
            {
                Estimations _estimator = new Estimations(Connection)
                {
                    City = Reader.GetString(0),
                    PostalCode = Reader.GetUInt32(1),
                    Zone = Reader.GetUInt32(2),
                    PriceFactor = Reader.GetFloat(3)
                };
                EstimatorList.Add(_estimator);
            }

            Reader.Close();
            Connection.Close();
        }
    }
}
