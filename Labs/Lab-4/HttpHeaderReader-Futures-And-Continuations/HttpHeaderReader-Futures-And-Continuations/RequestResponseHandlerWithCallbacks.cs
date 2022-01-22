using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;

namespace HttpHeaderReaderWithFuturesAndContinuations
{
    public class RequestResponseHandlerWithCallbacks
    {
        private List<String> listOfUrls; 
        public RequestResponseHandlerWithCallbacks(List<String> listOfUrls)
        {
            this.listOfUrls = listOfUrls;
            foreach (String url in listOfUrls)
            {
                this.MakeRequest(url);
            }
        }

        private static Tuple<String, int> parseResponse(String response)
        {
            String header = response.Substring(0, response.IndexOf("<"));
            int contentLengthStartIndex = header.IndexOf("Content-Length:") + "Content-Length:".Length;
            int contentLengthEndIndex = header.IndexOf("\n");
            String contentLengthSubstring = response.Substring(contentLengthStartIndex, contentLengthEndIndex);
            Regex regexPattern = new Regex(@"[0-9]+");
            String contentLengthValue = Regex.Match(contentLengthSubstring, @"\d+").Value;
            return new Tuple<String, int>(header, int.Parse(contentLengthValue));
        }

        private void RequestSendingCallback(IAsyncResult asyncResult)
        {
            ConnectionState connectionState = (ConnectionState) asyncResult.AsyncState;
            connectionState.Socket.EndSend(asyncResult);

            connectionState.Socket.BeginReceive(connectionState.Buffer, 0, connectionState.Buffer.Length,
                0, ResponseCallback, connectionState);
        }
        
        private void ResponseCallback(IAsyncResult asyncResult)
        {
            ConnectionState connectionState = (ConnectionState) asyncResult.AsyncState;
            int numberOfBytesReceived = connectionState.Socket.EndReceive(asyncResult);
            connectionState.Response = Encoding.ASCII.GetString(connectionState.Buffer, 0, numberOfBytesReceived);
            (String responseHeader, int contentLength) = parseResponse(connectionState.Response);
            connectionState.Socket.Shutdown(SocketShutdown.Both);
            connectionState.Socket.Close();
            Console.WriteLine("URL: {0}\nContent-Length: {1}\n\n", connectionState.OriginalUrl, contentLength);
        }

        private void HandleConnection(IAsyncResult asyncResult)
        {
            
            ConnectionState connectionState = (ConnectionState)asyncResult.AsyncState;
            connectionState.Socket.EndConnect(asyncResult);

            String httpHeader = String.Format("GET {0}/ HTTP/1.1\r\nHost: {1}\r\nContent-Length: 0\r\n\r\n", connectionState.Resource, connectionState.Host);

            byte[] httpHeaderBuffer = Encoding.ASCII.GetBytes(httpHeader);
            connectionState.Socket.BeginSend(httpHeaderBuffer, 0, httpHeaderBuffer.Length,
                0, RequestSendingCallback, connectionState);
        }

        public void MakeRequest(String url)
        {
            List<String> urlTokens = url.Split('/').ToList().Where(x => !String.IsNullOrEmpty(x)).ToList();
            ConnectionState connectionState = new ConnectionState();

            connectionState.Host = urlTokens[0];
            IPHostEntry dnsResolvedAddress = Dns.GetHostEntry(connectionState.Host);
            List<IPAddress> addresses = dnsResolvedAddress.AddressList.ToList();
            IPEndPoint endpoint = new IPEndPoint(dnsResolvedAddress.AddressList[0], 80);
            connectionState.OriginalUrl = url;
            connectionState.Buffer = new byte[1024];
            connectionState.Socket = new Socket(endpoint.Address.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            connectionState.Host = urlTokens[0];
            connectionState.Resource = url.Substring(url.IndexOf("/"));
            connectionState.Address = addresses[0];
            
            IAsyncResult asyncResult = connectionState.Socket.BeginConnect(endpoint, HandleConnection, connectionState);
            System.Threading.Thread.Sleep(100);
        }

    }
}