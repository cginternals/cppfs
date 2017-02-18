
#include <cppfs/LoginCredentials.h>


namespace cppfs
{


LoginCredentials::LoginCredentials()
{
}

LoginCredentials::LoginCredentials(const LoginCredentials & loginCredentials)
: m_values(loginCredentials.m_values)
{
}

LoginCredentials::LoginCredentials(const LoginCredentials && loginCredentials)
: m_values(loginCredentials.m_values)
{
}

LoginCredentials::~LoginCredentials()
{
}

LoginCredentials & LoginCredentials::operator=(const LoginCredentials & loginCredentials)
{
    m_values = loginCredentials.m_values;

    return *this;
}

LoginCredentials & LoginCredentials::operator=(const LoginCredentials && loginCredentials)
{
    m_values = loginCredentials.m_values;

    return *this;
}

bool LoginCredentials::isSet(const std::string & key) const
{
    return (m_values.find(key) != m_values.end());
}

std::string LoginCredentials::value(const std::string & key) const
{
    auto it = m_values.find(key);

    if (it != m_values.end())
    {
        return it->second;
    }

    return "";
}

void LoginCredentials::setValue(const std::string & key, const std::string & value)
{
    m_values[key] = value;
}


} // namespace cppfs
