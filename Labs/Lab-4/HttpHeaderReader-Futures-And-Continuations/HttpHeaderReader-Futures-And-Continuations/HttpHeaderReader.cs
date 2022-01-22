// See https://aka.ms/new-console-template for more information

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace HttpHeaderReaderWithFuturesAndContinuations
{
    class HttpHeaderReader
    {
        private static List<String> ReadUrlList(String filename)
        {
            List<String> listOfUrls = new List<string>();
            string[] lines = System.IO.File.ReadAllLines(filename);
            
            foreach (string line in lines)
                listOfUrls.Add(line);

            return listOfUrls;
        } 
        
        public static void Main(String[] args)
        {
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
            // The filepath contains the .. pattern for backwards directory navigation, because Jetbrains Rider 
            // apparently reads files from the bin/ directory where the compiled executables are saved and placing the
            // URL file there doesn't make much sense
            List<String> urls = ReadUrlList("../../../URLs.txt");
            // RequestResponseHandlerWithCallbacks requestResponseHandlerWithCallbacks = new RequestResponseHandlerWithCallbacks(urls);
            // RequestResponseHandlerWithTasks requestResponseHandlerWithTasks = new RequestResponseHandlerWithTasks(urls);
            AsynchronousRequestResponseHandler asynchronousRequestResponseHandler = new AsynchronousRequestResponseHandler(urls);
            stopwatch.Stop();            
            
            System.Threading.Thread.Sleep(100);
            Console.WriteLine("Elapsed time: {0} milliseconds", stopwatch.ElapsedMilliseconds);
        }
    }
}