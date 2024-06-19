#include "../include/Order.h"
#include "../include/Customer.h"
#include "../include/Volunteer.h"
#include "../include/WareHouse.h"

using std::string;
using std::vector;

Customer::Customer(int m_id, const string &m_name, int m_locationDistance, int m_maxOrders)
    : id(m_id), name(m_name), locationDistance(m_locationDistance), maxOrders(m_maxOrders), ordersId(), ordersMade(0)
{
}

const string &Customer::getName() const
{
    return name;
}

int Customer::getId() const
{
    return id;
}

int Customer::getCustomerDistance() const
{
    return locationDistance;
}

int Customer::getMaxOrders() const
{
    return maxOrders;
}

int Customer::getNumOrders() const
{
    return ordersMade;
}

bool Customer::canMakeOrder() const
{
    return (maxOrders - ordersMade > 0);
}
const vector<int> &Customer::getOrdersIds() const
{
    return ordersId;
}

int Customer::getLocationDistance() const
{
    return locationDistance;
}

void Customer::setOrdersMade(int numOrders)
{
    ordersMade = numOrders;
}
int Customer::addOrder(int orderId)
{
    if (maxOrders == ordersMade)
    {
        return -1;
    }
    ordersId.push_back(orderId);
    ordersMade++;
    return orderId;
}

SoldierCustomer::SoldierCustomer(int id, string name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders)
{
}

SoldierCustomer *SoldierCustomer::clone() const
{
    return new SoldierCustomer(*this);
}

CivilianCustomer::CivilianCustomer(int id, string name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders)
{
}

CivilianCustomer *CivilianCustomer::clone() const
{
    return new CivilianCustomer(*this);
}
