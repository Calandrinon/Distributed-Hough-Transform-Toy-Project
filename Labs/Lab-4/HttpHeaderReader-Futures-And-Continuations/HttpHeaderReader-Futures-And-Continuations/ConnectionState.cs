using System;
using System.Net;
using System.Net.Sockets;

namespace HttpHeaderReaderWithFuturesAndContinuations
{
    public class ConnectionState
    {
        public int BytesRemaining { get => bytesRemaining; set => bytesRemaining = value; }
        public IPAddress Address { get => address; set => address = value ?? throw new ArgumentNullException(nameof(value)); }
        public byte[] Buffer { get => buffer; set => buffer = value ?? throw new ArgumentNullException(nameof(value)); }
        public Socket Socket { get => socket; set => socket = value ?? throw new ArgumentNullException(nameof(value)); }
        public string Host { get => host; set => host = value ?? throw new ArgumentNullException(nameof(value)); }
        public string Resource { get => resource; set => resource = value ?? throw new ArgumentNullException(nameof(value)); }
        public string Response {get => response; set => response = value ?? throw new ArgumentNullException(nameof(value)); }
        public int ContentLength { get => contentLength; set => contentLength = value; }
        public string OriginalUrl { get => originalUrl; set => originalUrl = value; }
        public IPEndPoint Endpoint { get => endpoint; set => endpoint = value ?? throw new ArgumentNullException(nameof(value)); }
        
        IPAddress address;
        byte[] buffer;
        Socket socket;
        String host, resource, response, originalUrl;
        int bytesRemaining, contentLength;
        IPEndPoint endpoint;
    }
}