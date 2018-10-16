using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Threading;
using System.IO;
using System.Data;
using System.Collections.Generic;
using System.Linq;

namespace Server
{
    public class MathHandler
    {
        public MathHandler(string equation, ArrayList clients)
        {
            Equation = equation;
            Answer   = Double.Parse(new DataTable().Compute(Equation, null).ToString());
            Clients  = clients;
            Solved   = false;
        }

        public string Equation   { get; set; }
        public double Answer     { get; set; }
        public ArrayList Clients { get; set; }
        public bool Solved       { get; set; }

        public void HandleMath()
        {
            lock (Program.clientHandlers)
            {
                foreach (ClientHandler ch in Clients)
                {
                    ch.StartTime = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
                    ch.writer.Write("Solve this: " + Equation);
                }
            }

            List<ClientHandler> solvedList = new List<ClientHandler>();
            while (solvedList.Count != Program.clientHandlers.Count)
            {
                foreach (ClientHandler ch in Clients)
                {
                    if (!solvedList.Any(item => item == ch))
                        if (ch.AnswerTime != 0)
                            solvedList.Add(ch);
                }
            }

            var winnerList = solvedList.FindAll(r => r.Answer == Answer).OrderBy(o => o.AnswerTime);
            ClientHandler winner = winnerList.ElementAt(0);

            lock (Program.clientHandlers)
            {
                foreach (ClientHandler ch in Clients)
                {
                    ch.writer.Write($"Winner is: {winner.s.RemoteEndPoint}");
                }
            }

            Program.started = false;
        }
    }

    public class Program
    {
        public static ArrayList clientHandlers = new ArrayList();
        public static bool started = false;
        public static void Main()
        {
            try
            {
                IPAddress ipAd = IPAddress.Parse("127.0.0.1");    //localhost IP address, 
                TcpListener myList = new TcpListener(ipAd, 8000); //and use the same at
                myList.Start();                                   //the client side
                Console.WriteLine($"The server is running at {myList.LocalEndpoint}");

                while (true)
                {
                    Console.WriteLine("Waiting for a connection.....");
                    Socket s = myList.AcceptSocket();
                    ClientHandler ch = new ClientHandler(s);
                    lock (clientHandlers)
                    {
                        clientHandlers.Add(ch);
                    }
                    Thread t = new Thread(new ThreadStart(ch.HandleClient));
                    t.Start();

                    if (clientHandlers.Count >= 2 && !started)
                    {
                        MathHandler mathHandler = new MathHandler("2+2", clientHandlers);
                        Thread mathThread = new Thread(new ThreadStart(mathHandler.HandleMath));
                        mathThread.Start();
                        started = true;
                    }
                }

                //   myList.Stop();
            }
            catch (Exception e)
            {
                Console.WriteLine("Error..... " + e.StackTrace);
            }
        }
    }

    public class ClientHandler
    {
        public Socket s;
        private readonly NetworkStream socketStream;
        public  BinaryWriter writer;
        private BinaryReader reader;

        // Opgave 2
        public long StartTime  { get; set; }
        public long AnswerTime { get; set; }
        public double Answer   { get; set; }

        public ClientHandler(Socket s)
        {
            this.s = s;
            socketStream = new NetworkStream(s);
            writer = new BinaryWriter(socketStream);
            reader = new BinaryReader(socketStream);
            Answer = 0;
            AnswerTime = 0;
        }

        public void HandleClient()
        {
            Console.WriteLine("Connection accepted from " + s.RemoteEndPoint);
            writer.Write("You are: " + s.RemoteEndPoint);
            while (true)
            {
                try
                {
                    string text = reader.ReadString();

                    Console.WriteLine(AnswerTime);
                    Answer = Double.Parse(text);
                    AnswerTime = DateTimeOffset.UtcNow.ToUnixTimeSeconds() - StartTime;
                    Console.WriteLine(AnswerTime);

                    /*try
                    {
                        text = new DataTable().Compute(text, null).ToString();
                    }
                    catch
                    {
                        text = "Invalid expression.";
                    }*/

                    Console.WriteLine(text);
                    //writer.Write(text);


                    /*lock (Program.clientHandlers)
                    {
                        foreach (ClientHandler ch in Program.clientHandlers)
                        {
                            (ch.writer).Write(text);
                        }
                    }*/
                }
                catch (Exception e)
                {
                    Program.clientHandlers.Remove(this);
                    Console.WriteLine($"{s.RemoteEndPoint} has left the building.");
                    return;
                }
            }
        }
    }
}
