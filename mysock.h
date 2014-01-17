


class ISock
{
	virtual bool SetFamily(const int family) = 0;
	virtual int GetFamily() = 0;
	static bool setnonblock(int fd)
	{
		int opts = fcntl(fd, F_GETFL);
		if (-1 == opts)
		{
			perror("fcntl");
			return false;
		}
		opts |= O_NONBLOCK;
		if (-1 == fcntl(fd, F_SETFL, opts))
		{
			perror("fcntl");
			return false;
		}
		return true;
	}
};

class CSockIn : public ISock
{
public:
	void SetFamily(const int family)
	{
		_family = family;
	}
	void SetType(int type)
	{
		_type = type;
	}
	void SetAddr(const string Addr)
	{
		_addr = addr;
	}
	void SetPort(const unsigned short port)
	{
		_port = port;
	}
	int GetFamily()
	{
		return _family;
	}
	int GetType()
	{
		return _type;
	}
	string GetAddr()
	{
		return _addr;
	}
	unsigned short GetPort()
	{
		return _port;
	}
    CSockIn() : _family(AF_INET),_addr("0.0.0.0"),_port(8000),_type(SOCK_STREAM)
	{
		
	}
    CSockIn(unsigned short port) : _family(AF_INET),_addr("0.0.0.0"),_port(port),_type(SOCK_STREAM)
	{
	}
    CSockIn(string addr, unsigned short port) : _family(AF_INET),_addr(addr),_port(port),_type(SOCK_STREAM)
	{
	}
    CSockIn(int family, string addr, unsigned short port) : _family(family),_addr(addr),_port(port),_type(SOCK_STREAM)
	{
	}
	~CSockIn(){}

	Listen()
	{
	}
	
	
private:
	int _family;
	int _type;
	string _addr;
	unsigned short _port;
	int fd;
	bool createSock()
	{
		socket(family, )
	}
};



















