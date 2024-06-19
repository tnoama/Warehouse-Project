#include "../include/Volunteer.h"

////////////////////////////Volunteer  /////////////////////////////////////////////

Volunteer::Volunteer(int id, const string &name) : completedOrderId(-1), activeOrderId(-1), id(id), name(name)
{
}

int Volunteer::getId() const
{
    return id;
}

const string &Volunteer::getName() const
{
    return name;
}

int Volunteer::getActiveOrderId() const
{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const
{
    return completedOrderId;
}

bool Volunteer::isBusy() const
{
    return activeOrderId != -1;
}

//////////////////////////////Collector Volunteer/////////////////////////////////////////////////////////
CollectorVolunteer::CollectorVolunteer(int id, string name, int coolDown) : Volunteer(id, name), coolDown(coolDown), timeLeft(0) {}

CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step()
{
    if (decreaseCoolDown())
    {
        completedOrderId = activeOrderId;
        activeOrderId = -1;
    }
}

int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const
{
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown()
{
    timeLeft = timeLeft - 1;
    return timeLeft == 0;
}

bool CollectorVolunteer::hasOrdersLeft() const
{
    return true;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    if (getTimeLeft() == 0 && order.getStatus() == OrderStatus::PENDING)
    {
        return true;
    }
    return false;
}

void CollectorVolunteer::acceptOrder(const Order &order)
{
    timeLeft = coolDown;
    activeOrderId = order.getId();
}

string CollectorVolunteer::toString() const
{
    std::string orderid;
    std::string busy;
    std::string time;
    if (!isBusy())
    {
        orderid = "None";
        busy = "False";
        time = "None";
    }
    else
    {
        orderid = std::to_string(getActiveOrderId());
        busy = "True";
        time = std::to_string(timeLeft);
    }
    std::string s1 = "VolunteerId: " + std::to_string(getId());
    std::string s2 = "isBusy: " + busy;
    std::string s3 = "OrderId: " + orderid;
    std::string s4 = "timeLeft: " + time;
    std::string s5 = "ordersLeft: No Limit";

    std::string s = s1 + "\n" + s2 + "\n" + s3 + "\n" + s4 + "\n" + s5;

    return s;
}

////////////////////////////Limited Collector Volunteer////////////////////////////////////////////////
LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, string name, int coolDown, int maxOrders) : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(*this);
}

void LimitedCollectorVolunteer::step()
{
    CollectorVolunteer::step();
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const
{
    return ordersLeft != 0;
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{

    if (ordersLeft > 0 && getTimeLeft() == 0 && order.getStatus() == OrderStatus::PENDING)
    {
        return true;
    }
    return false;
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{

    CollectorVolunteer::acceptOrder(order);
    ordersLeft = ordersLeft - 1;
}

string LimitedCollectorVolunteer::toString() const
{
    std::string orderid;
    std::string busy;
    std::string time;
    if (!isBusy())
    {
        orderid = "None";
        busy = "False";
        time = "None";
    }
    else
    {
        orderid = std::to_string(getActiveOrderId());
        busy = "True";
        time = std::to_string(getTimeLeft());
    }
    std::string s1 = "VolunteerId: " + std::to_string(getId());
    std::string s2 = "isBusy: " + busy;
    std::string s3 = "OrderId: " + orderid;
    std::string s4 = "timeLeft: " + time;
    std::string s5 = "ordersLeft: " + std::to_string(ordersLeft);

    std::string s = s1 + "\n" + s2 + "\n" + s3 + "\n" + s4 + "\n" + s5;

    return s;
}
////////////////////////////////Driver Volunteer////////////////////////////////////////////////////
DriverVolunteer::DriverVolunteer(int id, string name, int maxDistance, int distancePerStep) : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}

DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft()
{
    distanceLeft = distanceLeft - distancePerStep;
    if (distanceLeft <= 0)
    {
        distanceLeft = 0;
        return true;
    }
    else
        return false;
}

bool DriverVolunteer::hasOrdersLeft() const
{
    return true;
}

bool DriverVolunteer::canTakeOrder(const Order &order) const
{
    if (order.getDistance() <= getMaxDistance() && getDistanceLeft() <= 0 && order.getStatus() == OrderStatus::COLLECTING)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DriverVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    distanceLeft = order.getDistance();
}

string DriverVolunteer::toString() const
{
    std::string orderid;
    std::string busy;
    if (!isBusy())
    {
        orderid = "None";
        busy = "False";
    }
    else
    {
        orderid = std::to_string(getActiveOrderId());
        busy = "True";
    }
    std::string s1 = "VolunteerId: " + std::to_string(getId());
    std::string s2 = "isBusy: " + busy;
    std::string s3 = "OrderId: " + orderid;
    std::string s4 = "distanceLeft: " + std::to_string(getDistanceLeft());
    std::string s5 = "ordersLeft: None";

    std::string s = s1 + "\n" + s2 + "\n" + s3 + "\n" + s4 + "\n" + s5;

    return s;
}

void DriverVolunteer::step()
{
    if (decreaseDistanceLeft())
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

//////////////////////////////Limited Driver Volunteer/////////////////////////////////////////////
LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders) : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    return ordersLeft != 0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    if (ordersLeft > 0 && order.getDistance() <= getMaxDistance() && getDistanceLeft() <= 0 && order.getStatus() == OrderStatus::COLLECTING)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    DriverVolunteer::acceptOrder(order);
    ordersLeft = ordersLeft - 1;
}

string LimitedDriverVolunteer::toString() const
{
    std::string orderid;
    std::string busy;
    if (!isBusy())
    {
        orderid = "None";
        busy = "False";
    }
    else
    {
        orderid = std::to_string(getActiveOrderId());
        busy = "True";
    }
    std::string s1 = "VolunteerId: " + std::to_string(getId());
    std::string s2 = "isBusy: " + busy;
    std::string s3 = "OrderId: " + orderid;
    std::string s4 = "distanceLeft: " + std::to_string(getDistanceLeft());
    std::string s5 = "ordersLeft: " + std::to_string(getNumOrdersLeft());

    std::string s = s1 + "\n" + s2 + "\n" + s3 + "\n" + s4 + "\n" + s5;

    return s;
}

void LimitedDriverVolunteer::step()
{
    DriverVolunteer::step();
}