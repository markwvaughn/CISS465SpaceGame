#include <iostream>
#include <string>
#include "SDL_net.h"

typedef char charbuf [256];


class CNet
{
public:
    static bool Init(); // Initialize SDL_net
    static void Quit(); // Exit SDL_net
};


class CNetMessage
{
protected:
    charbuf buffer; // message stored in buffer, limited to 256 bytes
    enum bufstates
    {
        EMPTY,
        READING,
        WRITING,
        FULL
    };
    bufstates state; // the message state

    void reset(); // reset message: fill with zeroes, change state to EMPTY

public:
    CNetMessage(); // constructor

    // Denotes how many bytes to pack for upload
    virtual int NumToLoad();

    // Denotes how many bytes may have to be downloaded
    virtual int NumToUnload();

    void LoadBytes(charbuf & inputbuffer, int n); // load n characters into the buffer
    void UnLoadBytes(charbuf & destbuffer); // Unload n characters from the buffer
    void finish(); // set the state to full.
    
};


class CIpAddress
{
private:
    IPaddress m_Ip; //the IPaddress structure

public:
    CIpAddress(); //constructor
    CIpAdress(Uint16 port); //create and associate a port to an instance
    CIpAddress(std::string host, Uint16 port); // creat and associate a port and a host to an instance
    
    void SetIp(IPaddress sdl_ip); // set a CIpAddress object from tan existing SDL IP
    bool Ok() const; // True if the object has a port and a host associated to it
    
    IPaddress GetIpAddress() const; // return a SDL_net IP structure
    Uint32 GetHost() const; // return the host
    Uint16 GetPort() const; // return the ports
};


class CTcpSocket
{
protected:
    TCPsocket m_Socket; // The TCP socket Structure
    SDLNet_SocketSet set; // A set of sockets. Used here only to check existing packets

public:
    CTcpSocket();
    virtual ~CTcpSocket();
    virtual void SetSocket(TCPsocket the_sdl_socket); // set a CTcpSocket object from a existing SDL socket
    bool Ok() const; //indicate if theres is a TCPsocket associated to the instance
    bool Ready() const; // true if there are bytes ready to be read
    virtual void OnReady(); // pure virtual
};


class CClientSocket : public CTcpSocket;


class CHostSocket: public CTcpSocket
{
public:
    CHostSocket(CIpAddress & the_ip_address); // create and open a socket for an existing IP

    CHostSocket(Uint16 port); // create and open a socket with the port
    bool Accept(CClientSocket &); // Accept a client socket
    virtual void OnReady(); // pure virtual
};

class CClientSocket : public CTcpSocket
{
private:
    CIpAddress m_RemoteIp; // the CIpAddress object corresponding to the remote host

public:
    CClientSocket(); // constructor
    CClientSocket(std::string host, Uint16 port); // Create the object and connect to the host, in a given port
    bool Connect(CIpAddress& remoteip); //make a connection to communicate with a remote host
    bool Connect(CHostSocket& the_listener_socket); //make a connection to communicate with the client
    void SetSocket(TCPsocket the_sdl_socket); //set a CTcpSocket object from an existing SDL net socket
    CIpAddress GetIpAddress() const; // return a CIpAddress object associated to the remote host
    virtual void OnReady(); // pure virtual
    bool Receive(CNetMessage &rData); // receive data and load it into a CNetMessage object
    bool Send(CNetMessage& sData); // send data from a CNetMessage object
};

