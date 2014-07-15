import SocketServer
import sys
class MyTCPHandler(SocketServer.StreamRequestHandler):
    def handle(self):
        # self.rfile is a file-like object created by the handler;
        # we can now use e.g. readline() instead of raw recv() calls
        while True:
           data = self.request.recv(4096)
           strData=str(data)
           strData = strData.replace('\r','\n')
           # print "%s  %s" % (format(self.client_address[0]) ,repr(strData))
           # print " %s " % repr(strData)
	   print repr(data) 
	   # Likewise, self.wfile is a file-like object used to write back
           # to the client
           reply = 'AAAAAA'
           self.wfile.write(reply)
        print 'socket closed!'

if __name__ == "__main__":
    HOST, PORT = "localhosts", 8010

    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer(('', PORT), MyTCPHandler)
    #server.socket.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()