using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SweetHome
{
    class Case
    {
<<<<<<< HEAD
        public Case() { }

        int Id              { get; set; }
        int AgentId         { get; set; }
        House House         { get; set; }
        DateTime StartDate  { get; set; }
        DateTime EndDate    { get; set; }
        bool IsOpened       { get; set; }


=======
        // Constructor
        public Case(MySqlConnection connecion)
        {
            Connection = connecion; 
        }

        MySqlConnection Connection  { get; set; }

        public uint Id             { get; set; }
        public uint AgentId        { get; set; }
        public House House         { get; set; }
        public DateTime StartDate  { get; set; }
        public DateTime EndDate    { get; set; }
        public bool IsOpened       { get; set; }

        public static ObservableCollection<Case> CaseList = new ObservableCollection<Case>();

        /// <summary>
        /// Create a new Case and save it to the database
        /// </summary>
        /// <exception cref="MySqlException">Throws a MySql Exception if one is thrown</exception>
        /// <returns></returns>
        public void Create()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("INSERT INTO Cases (AgentId, HouseId, StartDate) VALUES (@AgentId, @HouseId, NOW())", Connection);
                cmd.Parameters.AddWithValue("@AgentId", AgentId);
                cmd.Parameters.AddWithValue("@HouseId", House.Id);
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
        /// Update an existing case
        /// </summary>
        public void Update()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("UPDATE Cases SET AgentId = @AgentId, HouseId = @HouseId, StartDate = @StartDate, EndDate = @EndDate, IsOpened = @IsOpened WHERE Id = @Id", Connection);
                cmd.Parameters.AddWithValue("@AgentId",   AgentId);
                cmd.Parameters.AddWithValue("@HouseId",   House.Id);
                cmd.Parameters.AddWithValue("@StartDate", StartDate);
                cmd.Parameters.AddWithValue("@EndDate",   EndDate);
                cmd.Parameters.AddWithValue("@IsOpened",  IsOpened);
                cmd.Parameters.AddWithValue("@Id",        Id);
                cmd.ExecuteNonQuery();
                Connection.Close();
            }
            catch (MySqlException e)
            {
                throw e;
            }
        }

        /// <summary>
        /// Delete a case
        /// </summary>
        /// <param name="id">Kundens ID</param>
        public void Delete()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("DELETE FROM Cases WHERE Id = @id", Connection);
                cmd.Parameters.AddWithValue("@id", Id);
                cmd.ExecuteNonQuery();
                Connection.Close();
                CaseList.Remove(this);
            }
            catch (MySqlException e)
            {
                throw e;
            }
        }

        public void LoadCases()
        {
            Connection.Open();

            MySqlCommand cmd = new MySqlCommand("SELECT Id, AgentId, HouseId, StartDate, EndDate, IsOpened FROM Cases ORDER BY Id ASC", Connection);
            MySqlDataReader Reader = cmd.ExecuteReader();

            while (Reader.Read())
            {
                House _house = House.HouseList.FirstOrDefault(h => h.Id == Reader.GetUInt32(2));
                Case _case = new Case(Connection)
                {
                    Id        = Reader.GetUInt32(0),
                    AgentId   = Reader.GetUInt32(1),
                    House     = _house,
                    StartDate = Reader.GetDateTime(3),
                    EndDate   = Reader.GetDateTime(4),
                    IsOpened  = Reader.GetBoolean(5)
                };
                CaseList.Add(_case);
            }

            Reader.Close();
            Connection.Close();
        }
>>>>>>> cbec06a2983c4acaa2b6fae0133d6def524344e1
    }
}
