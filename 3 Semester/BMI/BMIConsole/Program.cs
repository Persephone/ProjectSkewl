using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Threading;

namespace BMIConsole
{
    class Program
    {
        private static readonly HttpClient client = new HttpClient();

        static void Main(string[] args)
        {
            Test();
            Thread.Sleep(1000);
            Console.WriteLine("Hello World!");
        }

        public static async void Test()
        {
            var values = new Dictionary<string, string>
            {
                { "height", "10" },
                { "weight", "10" }
            };

            var content = new FormUrlEncodedContent(values);

            try
            {
                var response = await client.PostAsync("http://localhost:5001/bmi", content);
                var responseString = await response.Content.ReadAsStringAsync();
                Console.WriteLine(responseString);
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message);
            }
        }
    }
}
