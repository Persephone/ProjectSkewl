using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading;

namespace Client
{
    public class Program
    {
        public static NetworkStream stm;
        public static BinaryReader reader;
        public static BinaryWriter writer;

        public static void Main()
        {
            try
            {
                TcpClient tcpclnt = new TcpClient();
                Console.WriteLine("Connecting.....");
                tcpclnt.Connect("127.0.0.1", 8000);     // use the ipaddress as in the server program
                Console.WriteLine("Connected");
                
                stm = tcpclnt.GetStream();
                reader = new BinaryReader(stm);
                writer = new BinaryWriter(stm);

                string info = reader.ReadString();
                Console.WriteLine(info);

                // Lidt Nyt kode til klienten
                /*Thread thread = new Thread(new ThreadStart(VisInput));
                thread.Start();*/
                /*string str = "s";
                while (str != "")
                {
                    Console.Write("Enter the string to be transmitted : ");
                    str = Console.ReadLine();
                    Console.WriteLine("Transmitting.....");
                    writer.Write(str);
                    Console.WriteLine("data sent...");

                    string response = reader.ReadString();
                    Console.WriteLine("Response: " + response);

                }*/

                Console.Write("Waiting for response...");

                string response = reader.ReadString();
                Console.WriteLine("Response: " + response);

                Console.Write("Enter the answer: ");
                string str = Console.ReadLine();
                Console.WriteLine("Transmitting.....");
                writer.Write(str);
                Console.WriteLine("data sent...");

                Console.Write("Waiting for response...");

                string newresponse = reader.ReadString();
                Console.WriteLine("Response: " + newresponse);

                Console.ReadKey();

                //tcpclnt.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine("Error..... " + e.StackTrace);
                Console.ReadKey();
            }
        }

        public static void VisInput()
        {
            string txt = "";
            while (true)
            {
                try
                {
                    txt = reader.ReadString();
                    Console.WriteLine(txt);
                }
                catch
                {
                    Console.WriteLine("Logger af...");
                    break;
                }
            }
        }

    }
}
