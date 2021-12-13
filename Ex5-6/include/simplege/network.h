#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE::Network
{
#ifdef _WIN32
  using NativeSocket = SOCKET;
#else
  using NativeSocket = int;
#endif

  class Subsystem;

  class Serviceable
  {
  public:
    inline Serviceable();
    virtual inline ~Serviceable();

    virtual void Service() = 0;
  };

  class Connection : public Serviceable
  {
  public:
    using ConnectionIDType = int;

    using OnData = std::function<void(gsl::not_null<Connection*>, gsl::span<std::byte>)>;
    using OnDisconnect = std::function<void(gsl::not_null<Connection*>)>;

    static std::shared_ptr<Connection> Create(NativeSocket clientSocket, const OnDisconnect& onDisconnect,
                                              const OnData& onData)
    {
      return std::make_shared<Connection>(clientSocket, onDisconnect, onData);
    }

    static const auto& ListConnections() { return connections; }

    Connection(NativeSocket clientSocket, const OnDisconnect& onDisconnect, const OnData& onData);
    ~Connection() override;

    Connection(const Connection&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection& operator=(Connection&&) = delete;

    [[nodiscard]] ConnectionIDType GetID() const { return id; }

    void Service() override;

    void Send(gsl::span<std::byte> data)
    {
      std::size_t length = data.size_bytes();
      SendRaw(gsl::span<std::byte>(reinterpret_cast<std::byte*>(&length), sizeof(length)));
      SendRaw(data);
    }

  private:
    void RecvRaw(gsl::span<std::byte> buffer) const;
    void SendRaw(gsl::span<std::byte> data) const;

    ConnectionIDType id;
    NativeSocket clientSocket;
    OnDisconnect onDisconnect;
    OnData onData;

    static inline std::unordered_set<gsl::not_null<Connection*>> connections;
  };

  class Server : public Serviceable
  {
  public:
    using OnConnection = std::function<void(const std::shared_ptr<Connection>&)>;

    static std::shared_ptr<Server> Create(NativeSocket listening, const OnConnection& onConnection,
                                          const Connection::OnDisconnect& onDisconnect,
                                          const Connection::OnData& onData)
    {
      return std::make_shared<Server>(listening, onConnection, onDisconnect, onData);
    }

    Server(NativeSocket listening, const OnConnection& onConnection, const Connection::OnDisconnect& onDisconnect,
           const Connection::OnData& onData);
    ~Server() override;

    Server(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;

    void Service() override;

  private:
    NativeSocket listening;
    OnConnection onConnection;
    Connection::OnDisconnect onDisconnect;
    Connection::OnData onData;
  };

  class Subsystem
  {
  public:
    [[nodiscard]] static Subsystem& Instance()
    {
      static Subsystem instance;
      return instance;
    }

    Subsystem(const Subsystem&) = delete;
    Subsystem(Subsystem&&) = delete;
    Subsystem& operator=(const Subsystem&) = delete;
    Subsystem& operator=(Subsystem&&) = delete;

    void Service();
    std::shared_ptr<Server> Listen(std::uint16_t port, const Server::OnConnection& onConnection,
                                   const Connection::OnDisconnect& onDisconnect, const Connection::OnData& onData);
    std::shared_ptr<Connection> Connect(std::string_view host, std::uint16_t port,
                                        const Connection::OnDisconnect& onDisconnect, const Connection::OnData& onData);

    void Register(gsl::not_null<Serviceable*> serviceable) { serviceables.insert(serviceable); }
    void Unregister(gsl::not_null<Serviceable*> serviceable) { toRemove.insert(serviceable); }

  private:
    Subsystem();
    ~Subsystem();

    std::unordered_set<gsl::not_null<Serviceable*>> serviceables;
    std::unordered_set<gsl::not_null<Serviceable*>> toRemove;
  };

  inline Serviceable::Serviceable() { Subsystem::Instance().Register(this); }

  inline Serviceable::~Serviceable() { Subsystem::Instance().Unregister(this); }

  inline void Service() { Subsystem::Instance().Service(); }

  inline std::shared_ptr<Server> Listen(std::uint16_t port, const Server::OnConnection& onConnection,
                                        const Connection::OnDisconnect& onDisconnect, const Connection::OnData& onData)
  {
    return Subsystem::Instance().Listen(port, onConnection, onDisconnect, onData);
  }

  inline std::shared_ptr<Connection> Connect(std::string_view host, std::uint16_t port,
                                             const Connection::OnDisconnect& onDisconnect,
                                             const Connection::OnData& onData)
  {
    return Subsystem::Instance().Connect(host, port, onDisconnect, onData);
  }

  inline const auto& ListConnections() { return Connection::ListConnections(); }
} // namespace SimpleGE::Network