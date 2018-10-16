using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Client
{
    class Client
    {
        public static NetworkStream networkStream;
        public static BinaryReader Reader;
        public static BinaryWriter Writer;

        static void Main(string[] args)
        {
            TcpClient tcpClient = new TcpClient();
            Console.WriteLine("Connecting...");
            tcpClient.Connect("127.0.0.1", 8050);

            Console.WriteLine("Connected");

            networkStream = tcpClient.GetStream();
            Reader = new BinaryReader(networkStream);
            Writer = new BinaryWriter(networkStream);

            Thread thread = new Thread(() => Listen());
            thread.Start();

            while (true)
            {
                Console.WriteLine("Enter Bid: ");
                string message = Console.ReadLine();

                Writer.Write(message);

                Console.WriteLine("Bid submitted.");
            }
        }

        public static void Listen()
        {
            string response = "";
            while (true)
            {
                try
                {
                    response = Reader.ReadString();
                    Console.WriteLine(response);
                }
                catch
                {
                    break;
                }
            }
        }
    }
}
