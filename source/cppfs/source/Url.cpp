
#include <cppfs/Url.h>

#include <algorithm>


namespace cppfs
{


Url::Url()
: m_url("")
, m_analyzed(false)
, m_protocol("")
, m_location("")
, m_address("")
, m_path("")
, m_login("")
, m_host("")
, m_username("")
, m_password("")
{
}

Url::Url(const Url & url)
: m_url(url.m_url)
, m_analyzed(url.m_analyzed)
, m_protocol(url.m_protocol)
, m_location(url.m_location)
, m_address(url.m_address)
, m_path(url.m_path)
, m_login(url.m_login)
, m_host(url.m_host)
, m_username(url.m_username)
, m_password(url.m_password)
{
}

Url::Url(const Url && url)
: m_url(url.m_url)
, m_analyzed(url.m_analyzed)
, m_protocol(url.m_protocol)
, m_location(url.m_location)
, m_address(url.m_address)
, m_path(url.m_path)
, m_login(url.m_login)
, m_host(url.m_host)
, m_username(url.m_username)
, m_password(url.m_password)
{
}

Url::Url(const std::string & url)
: m_url(url)
, m_analyzed(false)
, m_protocol("")
, m_location("")
, m_address("")
, m_path("")
, m_login("")
, m_host("")
, m_username("")
, m_password("")
{
}

Url::Url(const std::string && url)
: m_url(url)
, m_analyzed(false)
, m_protocol("")
, m_location("")
, m_address("")
, m_path("")
, m_login("")
, m_host("")
, m_username("")
, m_password("")
{
}

Url::Url(const char * url)
: m_url(url)
, m_analyzed(false)
, m_protocol("")
, m_location("")
, m_address("")
, m_path("")
, m_login("")
, m_host("")
, m_username("")
, m_password("")
{
}

Url::~Url()
{
}

Url & Url::operator=(const Url & url)
{
    m_url      = url.m_url;
    m_analyzed = url.m_analyzed;
    m_protocol = url.m_protocol;
    m_location = url.m_location;
    m_address  = url.m_address;
    m_path     = url.m_path;
    m_login    = url.m_login;
    m_host     = url.m_host;
    m_username = url.m_username;
    m_password = url.m_password;

    return *this;
}

Url & Url::operator=(const Url && url)
{
    m_url      = std::move(url.m_url);
    m_analyzed = std::move(url.m_analyzed);
    m_protocol = std::move(url.m_protocol);
    m_location = std::move(url.m_location);
    m_address  = std::move(url.m_address);
    m_path     = std::move(url.m_path);
    m_login    = std::move(url.m_login);
    m_host     = std::move(url.m_host);
    m_username = std::move(url.m_username);
    m_password = std::move(url.m_password);

    return *this;
}

const std::string & Url::toString() const
{
    return m_url;
}

std::string Url::protocol() const
{
    analyze();
    return m_protocol;
}

std::string Url::location() const
{
    analyze();
    return m_location;
}

std::string Url::address() const
{
    analyze();
    return m_address;
}

std::string Url::path() const
{
    analyze();
    return m_path;
}

std::string Url::login() const
{
    analyze();
    return m_login;
}

std::string Url::host() const
{
    analyze();
    return m_host;
}

std::string Url::username() const
{
    analyze();
    return m_username;
}

std::string Url::password() const
{
    analyze();
    return m_password;
}

void Url::analyze() const
{
    // Abort if already analyzed
    if (m_analyzed) return;

    // Reset information
    m_protocol = "";
    m_location = "";
    m_address  = "";
    m_path     = "";
    m_login    = "";
    m_host     = "";
    m_username = "";
    m_password = "";

    // Get protocol and location
    std::size_t pos = m_url.find("://");

    if (pos != std::string::npos)
    {
        m_protocol = m_url.substr(0, pos + 3);
        m_location = m_url.substr(pos+3);
    }
    else
    {
        m_location = m_url;
    }

    // Get address and path
    pos = m_location.find("/");

    if (pos != std::string::npos)
    {
        m_address = m_location.substr(0, pos);
        m_path    = m_location.substr(pos);
    }

    // Get login and host
    pos = m_address.find("@");

    if (pos != std::string::npos)
    {
        m_login = m_address.substr(0, pos);
        m_host  = m_address.substr(pos+1);
    }
    else
    {
        m_login = "";
        m_host  = m_address;
    }

    // Get username and password
    pos = m_login.find(":");

    if (pos != std::string::npos)
    {
        m_username = m_login.substr(0, pos);
        m_password = m_login.substr(pos+1);
    }
    else
    {
        m_username = m_login;
    }

    // Set status
    m_analyzed = true;
}


} // namespace cppfs
