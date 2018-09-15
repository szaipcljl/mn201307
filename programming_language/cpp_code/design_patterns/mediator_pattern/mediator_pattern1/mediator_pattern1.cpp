//中介者模式
#include<iostream>
#include<string>

using namespace std;

class Country;

class UnitedNations {
public:
    virtual void Declare(string message, Country* country) = 0;
};

class Country {
protected:
    UnitedNations* m_UN;
public:
    Country(UnitedNations* united) :m_UN(united) { };
    virtual void Declare(string message) = 0;
    virtual void getmessage(string message) = 0;
};

class America : public Country {
public:
    America(UnitedNations* united) : Country(united) {};

    void Declare(string message)
    {
        m_UN->Declare(message, this);
    }

    void getmessage(string message)
    {
        cout << "美国收到对方消息：" << message << endl;
    }
};

class Northkorea :public Country {
public:
    Northkorea(UnitedNations* united) : Country(united) { };

    void Declare(string message)
    {
        m_UN->Declare(message, this);
    }

    void getmessage(string message)
    {
        cout << "朝鲜收到对方消息：" <<message<< endl;
    }
};

class UNSecurityCouncil : public UnitedNations {
    Country* m_USA;
    Country* m_Korea;
public:
    void setUSA(Country* USA)
    {
        m_USA = USA;
    }
    void setKorea(Country* Korea)
    {
        m_Korea = Korea;
    }

    void Declare(string message, Country* country)
    {
        if (country == m_USA) {
            m_Korea->getmessage(message);
        } else if (country == m_Korea)
            m_USA->getmessage(message);
    }
};

int main()
{
    UNSecurityCouncil* UN = new UNSecurityCouncil();

    Country* USA = new America(UN);
    Country* Korea = new Northkorea(UN);

    UN->setUSA(USA);
    UN->setKorea(Korea);

    USA->Declare("不准研制核武器，否则发动战争！");
    Korea->Declare("我就研制！");

    system("pause");
    return 0;
}

