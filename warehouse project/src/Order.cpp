#include <string>
#include <vector>
#include "../include/Order.h"
using std::string;
using std::vector;

Order::Order(int m_id, int m_customerId, int m_distance) : id(m_id), customerId(m_customerId), distance(m_distance), status(OrderStatus::PENDING), collectorId(-1), driverId(-1) {}

int Order::getId() const
{
    return id;
}

int Order::getCustomerId() const
{
    return customerId;
}

void Order::setStatus(OrderStatus status)
{
    this->status = status;
}

void Order::setCollectorId(int collectorId)
{
    this->collectorId = collectorId;
}

void Order::setDriverId(int driverId)
{
    this->driverId = driverId;
}

int Order::getCollectorId() const
{
    return collectorId;
}

int Order::getDriverId() const
{
    return driverId;
}
OrderStatus Order::getStatus() const
{
    return status;
}

std::string orderStatusToString(OrderStatus status)
{
    switch (status)
    {
    case OrderStatus::PENDING:
        return "PENDING";
    case OrderStatus::COLLECTING:
        return "COLLECTING";
    case OrderStatus::DELIVERING:
        return "DELIVERING";
    case OrderStatus::COMPLETED:
        return "COMPLETED";
    default:
        return "UNKNOWN"; // Handle unknown enum values if needed
    }
}

const string Order::toString() const
{
    std::string s1 = "OrderId: " + std::to_string(Order::getId());
    std::string s2 = "OrderStatus: " + orderStatusToString(Order::getStatus());
    std::string s3 = "CustomerID: " + std::to_string(Order::getCustomerId());
    std::string s4 = "Collector: " + std::to_string(Order::getCollectorId());
    std::string s5 = "Driver: " + std::to_string(Order::getDriverId());

    if (Order::getCollectorId() == -1)
    {
        s4 = "Collector: None";
    }
    if (Order::getDriverId() == -1)
    {
        s5 = "Driver: None";
    }

    std::string s = s1 + "\n" + s2 + "\n" + s3 + "\n" + s4 + "\n" + s5;

    return s;
}

int Order::getDistance() const
{
    return distance;
}

Order *Order::clone() const
{
    return new Order(*this);
}