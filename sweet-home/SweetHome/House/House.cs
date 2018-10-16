using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Security.Permissions;
using System.Text;
using System.Threading.Tasks;

namespace SweetHome 
{
    class House 
    {
        public House(MySqlConnection connecion)
        {
            Connection = connecion;
        }

        MySqlConnection Connection { get; set; }

        #region Echonomy
        public uint Id { get; set; }
        public uint Price { get; set; }
        public uint SquarePrice { get; set; }
        public uint OwnerExpense { get; set; }
        public uint Payout { get; set; }
        public uint BruttoNetto { get; set; }
#endregion
#region Facts
        public string Address { get; set; }
        public uint PostalCode { get; set; }
        public string City { get; set; }
        public uint Year { get; set; }
        public uint LandArea { get; set; }
        public ushort Bedrooms { get; set; }
        public uint GarageCarport { get; set; }
        public uint LivingArea { get; set; }
        public ushort Rooms { get; set; }
        public char EnergyMark { get; set; }
        public string CaseNumber { get; set; }
#endregion
#region ArticleStuff
        public string Description { get; set; }
        public string MainTitle { get; set; }
        public string Subtitle { get; set; }
        public string[] ImageLinks { get; set; }
        #endregion

        public static ObservableCollection<House> HouseList = new ObservableCollection<House>();

        /// <summary>
        /// Create a new House and save it to the database
        /// </summary>
        /// <exception cref="MySqlException">Throws a MySql Exception if one is thrown</exception>
        /// <returns></returns>
        public void Create()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("INSERT INTO `Houses` (`Price`, `SquarePrice`, `OwnerExpense`, `Payout`, `BruttoNetto`, `Address`, `PostalCode`, `City`, `Year`, `LandArea`, `Bedrooms`, `GarageCarport`, `LivingArea`, `Rooms`, `EnergyMark`, `CaseNumber`, `Description`, `MainTitle`, `Subtitle`, `ImageLinks`) VALUES" +
                                                                        " (@Price,  @SquarePrice,  @OwnerExpense,  @Payout,  @BruttoNetto,  @Address,  @PostalCode,  @City,  @Year,  @LandArea,  @Bedrooms,  @GarageCarport,  @LivingArea,  @Rooms,  @EnergyMark,  @CaseNumber,  @Description, @MainTitle`,   @Subtitle`,  @ImageLinks)", Connection);
                cmd.Parameters.AddWithValue("@Price", Price);
                cmd.Parameters.AddWithValue("@SquarePrice", SquarePrice);
                cmd.Parameters.AddWithValue("@OwnerExpense", OwnerExpense);
                cmd.Parameters.AddWithValue("@SquarePrice", SquarePrice);
                cmd.Parameters.AddWithValue("@Payout", Payout);
                cmd.Parameters.AddWithValue("@BruttoNetto", BruttoNetto);
                cmd.Parameters.AddWithValue("@Address", Address);
                cmd.Parameters.AddWithValue("@PostalCode", PostalCode);
                cmd.Parameters.AddWithValue("@City", City);
                cmd.Parameters.AddWithValue("@Year", Year);
                cmd.Parameters.AddWithValue("@LandArea", LandArea);
                cmd.Parameters.AddWithValue("@Bedrooms", Bedrooms);
                cmd.Parameters.AddWithValue("@GarageCarport", GarageCarport);
                cmd.Parameters.AddWithValue("@LivingArea", LivingArea);
                cmd.Parameters.AddWithValue("@Rooms", Rooms);
                cmd.Parameters.AddWithValue("@EnergyMark", EnergyMark);
                cmd.Parameters.AddWithValue("@CaseNumber", CaseNumber);
                cmd.Parameters.AddWithValue("@Description", Description);
                cmd.Parameters.AddWithValue("@MainTitle", MainTitle);
                cmd.Parameters.AddWithValue("@EnergyMark", EnergyMark);
                cmd.Parameters.AddWithValue("@Subtitle", Subtitle);
                cmd.Parameters.AddWithValue("@ImageLinks", String.Join(" ", ImageLinks));
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
        /// Update an existing House
        /// </summary>
        public void Update()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("UPDATE `Houses` SET `Price` = @Price, `SquarePrice` = @SquarePrice, `OwnerExpense` = @OwnerExpense, `Payout` = @Payout, `BruttoNetto` = @BruttoNetto, `Address` = @Address, `PostalCode` = @PostalCode, `City` = @City, `Year` = @Year, `LandArea` = @LandArea, `Bedrooms` = @Bedrooms, `GarageCarport` = @GarageCarport, `LivingArea` = @LivingArea, `Rooms` = @Rooms, `EnergyMark` = @EnergyMark, `CaseNumber` = @CaseNumber, `Description` = @Description, `MainTitle` = @MainTitle, `Subtitle` = @SubTitle, `ImageLinks` = @ImageLinks WHERE Id = @Id", Connection);
                cmd.Parameters.AddWithValue("@Price", Price);
                cmd.Parameters.AddWithValue("@SquarePrice", SquarePrice);
                cmd.Parameters.AddWithValue("@OwnerExpense", OwnerExpense);
                cmd.Parameters.AddWithValue("@SquarePrice", SquarePrice);
                cmd.Parameters.AddWithValue("@Payout", Payout);
                cmd.Parameters.AddWithValue("@BruttoNetto", BruttoNetto);
                cmd.Parameters.AddWithValue("@Address", Address);
                cmd.Parameters.AddWithValue("@PostalCode", PostalCode);
                cmd.Parameters.AddWithValue("@City", City);
                cmd.Parameters.AddWithValue("@Year", Year);
                cmd.Parameters.AddWithValue("@LandArea", LandArea);
                cmd.Parameters.AddWithValue("@Bedrooms", Bedrooms);
                cmd.Parameters.AddWithValue("@GarageCarport", GarageCarport);
                cmd.Parameters.AddWithValue("@LivingArea", LivingArea);
                cmd.Parameters.AddWithValue("@Rooms", Rooms);
                cmd.Parameters.AddWithValue("@EnergyMark", EnergyMark);
                cmd.Parameters.AddWithValue("@CaseNumber", CaseNumber);
                cmd.Parameters.AddWithValue("@Description", Description);
                cmd.Parameters.AddWithValue("@MainTitle", MainTitle);
                cmd.Parameters.AddWithValue("@EnergyMark", EnergyMark);
                cmd.Parameters.AddWithValue("@Subtitle", Subtitle);
                cmd.Parameters.AddWithValue("@ImageLinks", String.Join(" ", ImageLinks));
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
        /// Delete a House
        /// </summary>
        /// <param name="id">Kundens ID</param>
        public void Delete()
        {
            Connection.Open();

            try
            {
                MySqlCommand cmd = new MySqlCommand("DELETE FROM Houses WHERE Id = @id", Connection);
                cmd.Parameters.AddWithValue("@id", Id);
                cmd.ExecuteNonQuery();
                Connection.Close();
                HouseList.Remove(this);
            }
            catch (MySqlException e)
            {
                throw e;
            }
        }

        public void LoadCases()
        {
            Connection.Open();

            MySqlCommand cmd = new MySqlCommand("SELECT `Id`, `Price`, `SquarePrice`, `OwnerExpense`, `Payout`, `BruttoNetto`, `Address`, `PostalCode`, `City`, `Year`, `LandArea`, `Bedrooms`, `GarageCarport`, `LivingArea`, `Rooms`, `EnergyMark`, `CaseNumber`, `Description`, `MainTitle`, `Subtitle`, `ImageLinks` FROM Houses ORDER BY Id ASC", Connection);
            MySqlDataReader Reader = cmd.ExecuteReader();

            while (Reader.Read())
            {
                int index = 0;
                House house = new House(Connection)
                {
                    Id = Reader.GetUInt32(index++),
                    Price = Reader.GetUInt32(index++),
                    SquarePrice = Reader.GetUInt32(index++),
                    OwnerExpense = Reader.GetUInt32(index++),
                    Payout = Reader.GetUInt32(index++),
                    BruttoNetto = Reader.GetUInt32(index++),
                    Address = Reader.GetString(index++),
                    PostalCode = Reader.GetUInt32(index++),
                    City = Reader.GetString(index++),
                    Year = Reader.GetUInt32(index++),
                    LandArea = Reader.GetUInt32(index++),
                    Bedrooms = Reader.GetUInt16(index++),
                    GarageCarport = Reader.GetUInt32(index++),
                    LivingArea = Reader.GetUInt32(index++),
                    Rooms = Reader.GetUInt16(index++),
                    EnergyMark = Reader.GetChar(index++),
                    CaseNumber = Reader.GetString(index++),
                    Description = Reader.GetString(index++),
                    MainTitle = Reader.GetString(index++),
                    Subtitle = Reader.GetString(index++),
                    ImageLinks = Reader.GetString(index++).Split(' ')
                };
                HouseList.Add(house);
            }

            Reader.Close();
            Connection.Close();
        }
    }
}
