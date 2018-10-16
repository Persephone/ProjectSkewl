using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Server
{
    class Server
    {
        public static List<Client> clientList = new List<Client>();

        public static int CurrentBid { get; set; }

        static void Main(string[] args)
        {
            IPAddress iPAddress = IPAddress.Parse("127.0.0.1");
            TcpListener tcpListener = new TcpListener(iPAddress, 8050);
            tcpListener.Start();

            Console.WriteLine($"Server is running on: {tcpListener.LocalEndpoint}");

            while (true)
            {
                Console.WriteLine("Waiting for a connection...");
                Socket socket = tcpListener.AcceptSocket();

                Client client = new Client(socket);

                lock (clientList)
                {
                    clientList.Add(client);
                }

                Thread clientThread = new Thread(() => client.HandleClient());
                clientThread.Start();
            }
        }

        public static void SendMessageToClients(string message)
        {
            foreach (Client client in clientList)
                client.Writer.Write(message);
        }
    }

    public class Client
    {
        private readonly Socket m_Socket;
        private readonly NetworkStream m_NetworkStream;
        public  BinaryWriter Writer;
        private readonly BinaryReader Reader;

        public Client(Socket socket)
        {
            m_Socket = socket;
            m_NetworkStream = new NetworkStream(socket);
            Writer = new BinaryWriter(m_NetworkStream);
            Reader = new BinaryReader(m_NetworkStream);
        }

        (bool, string) HandleBid(string bid)
        {
            int.TryParse(bid, out int currentBid);
            if (currentBid == 0)
                return (false, "Invalid bid submitted.");

            if ((currentBid % 100) != 0)
                return (false, "Bid has to be in the hundreds (e.g. 100, 200 etc.)");

            int add = currentBid - Server.CurrentBid;
            if (add < 100)
                return (false, "Bid has to be minimun of 100");

            Server.CurrentBid += add;
            return (true, "Success.");
        }


        public void HandleClient()
        {
            Console.WriteLine($"Connection accepted from: {m_Socket.RemoteEndPoint}");
            while (true)
            {
                try
                {
                    var response = HandleBid(Reader.ReadString());
                    if (!response.Item1)
                        Writer.Write(response.Item2);
                    else
                        Server.SendMessageToClients($"Highest bid: {Server.CurrentBid}");
                }
                catch (Exception)
                {
                    Console.WriteLine($"{m_Socket.RemoteEndPoint} has disconnected.");
                    Server.clientList.Remove(this);
                    break;
                }
            }
        }
    }
}
