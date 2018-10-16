using System;
using System.Collections.Generic;
<<<<<<< HEAD
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MySql.Data.MySqlClient;

namespace SweetHome.DatabaseConnection
{
    class Connection
    {
        protected Connection() { }


=======
using System.Configuration;
using System.Data;
using System.Windows.Forms;
using MySql.Data.MySqlClient;
using System.Linq;

namespace SweetHome
{
    class Connection
    {
        /// <summary>
        /// Gets the singleton instance of the connection
        /// </summary>
        public static Connection Instance { get; } = new Connection();

        public MySqlConnection Conn { get; protected set; }

        public bool IsOpened { get => Conn != null && Conn.State == ConnectionState.Open; }

        private Connection()
        {
            var connectionString = new MySqlConnectionStringBuilder()
            {
                Server = ConfigurationManager.AppSettings.Get("Host"),
                UserID = ConfigurationManager.AppSettings.Get("Database"),
                Password = ConfigurationManager.AppSettings.Get("User"),
                Database = ConfigurationManager.AppSettings.Get("Password")
            }.ConnectionString;

            using (Conn = new MySqlConnection(connectionString))
            {
                try
                {
                    Conn.Open();
                }
                catch (MySqlException me)
                {
                    MessageBox.Show(me.Message);
                    Application.Exit();
                }
                finally
                {
                    Conn.Close();
                }
            }
        }
    }
}
