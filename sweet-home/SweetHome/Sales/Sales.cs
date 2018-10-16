using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SweetHome
{
    class Sales
    {
        public Sales(MySqlConnection connecion)
        {
            Connection = connecion;
        }

        MySqlConnection Connection { get; set; }

        public uint Id             { get; set; }
        public House House         { get; set; }
        public uint AgentId        { get; set; }
        public Case Case           { get; set; }
        public uint SalesPrice     { get; set; }
        public DateTime SalesDate  { get; set; }

        public static ObservableCollection<Sales> SalesList = new ObservableCollection<Sales>();

        /// <summary>
        /// Create a new Sale and save it to the database
        /// </summary>
        /// <exception cref="MySqlException">Throws a MySql Exception if one is thrown</exception>
        /// <returns></returns>
        public void Create()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("INSERT INTO Sales (HouseId, AgentId, CaseId, SalesPrice, SalesDate) VALUES (@HouseId, @AgentId, @CaseId, @SalesPrice, NOW())", Connection);
                cmd.Parameters.AddWithValue("@HouseId", House.Id);
                cmd.Parameters.AddWithValue("@AgentId", AgentId);
                cmd.Parameters.AddWithValue("@CaseId",  Case.Id);
                cmd.Parameters.AddWithValue("@SalesPrice", SalesPrice);
                cmd.ExecuteNonQuery();
                Connection.Close();
                Id = Convert.ToUInt32(cmd.LastInsertedId);
            }
            catch (MySqlException e)
            {
                throw e;
            }
        }

        /// <summary>
        /// Update an existing sale
        /// </summary>
        public void Update()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("UPDATE Sales SET AgentId = @AgentId, HouseId = @HouseId, SalesDate = @SalesDate, CaseId = @CaseId, @SalesPrice = @SalesPrice WHERE Id = @Id", Connection);
                cmd.Parameters.AddWithValue("@AgentId", AgentId);
                cmd.Parameters.AddWithValue("@HouseId", House.Id);
                cmd.Parameters.AddWithValue("@SalesDate", SalesDate);
                cmd.Parameters.AddWithValue("@SalesPrice", SalesPrice);
                cmd.Parameters.AddWithValue("@CaseId", Case.Id);
                cmd.Parameters.AddWithValue("@Id", Id);
                cmd.ExecuteNonQuery();
                Connection.Close();
            }
            catch (MySqlException e)
            {
                throw e;
            }
        }

        /// <summary>
        /// Delete a sale
        /// </summary>
        /// <param name="id">Kundens ID</param>
        public void Delete()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("DELETE FROM Sales WHERE Id = @id", Connection);
                cmd.Parameters.AddWithValue("@id", Id);
                cmd.ExecuteNonQuery();
                Connection.Close();
                SalesList.Remove(this);
            }
            catch (MySqlException e)
            {
                throw e;
            }
        }

        public void LoadCases()
        {
            Connection.Open();

            MySqlCommand cmd = new MySqlCommand("SELECT Id, HouseId, AgentId, CaseId, SalesPrice, SalesDate FROM Sales ORDER BY Id ASC", Connection);
            MySqlDataReader Reader = cmd.ExecuteReader();

            while (Reader.Read())
            {
                House _house = House.HouseList.FirstOrDefault(h => h.Id == Reader.GetUInt32(1));
                Case _case   = Case.CaseList.FirstOrDefault(h => h.Id == Reader.GetUInt32(3));

                Sales _sales = new Sales(Connection)
                {
                    Id = Reader.GetUInt32(0),
                    House = _house,
                    Case = _case,
                    AgentId = Reader.GetUInt32(2),
                    SalesPrice = Reader.GetUInt32(4),
                    SalesDate = Reader.GetDateTime(5)
                };
                SalesList.Add(_sales);
            }

            Reader.Close();
            Connection.Close();
        }
    }
}
