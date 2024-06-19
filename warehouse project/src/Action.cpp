#include "../include/Action.h"
#include "../include/Volunteer.h"
#include "../include/WareHouse.h"
#include <iostream>

///////////////////////////////////////////Base Action//////////////////////////////////////
BaseAction::BaseAction() : errorMsg(), status(){};

ActionStatus BaseAction::getStatus() const
{
    return status;
}
void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}
void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    std::cout << errorMsg << std::endl;
}
string BaseAction::getErrorMsg() const
{
    return errorMsg;
}

//////////////////////////////////////////SimulateStep/////////////////////////////////////////////////

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps)
{
}

void SimulateStep::act(WareHouse &wareHouse)
{

    for (int k = 0; k < numOfSteps; k++)
    {

        const vector<Order *> &pendingOrders = wareHouse.getPendingOrders();
        const vector<Order *> &inProcessOrders = wareHouse.getInProcessOrders();
        const vector<Volunteer *> &volunteers = wareHouse.getVolunteers();

        vector<int> toRemove;

        for (size_t i = 0; i < pendingOrders.size(); i++)
        {
            if (pendingOrders[i]->getStatus() == OrderStatus::PENDING)
            {
                for (size_t j = 0; j < volunteers.size(); j++)
                {
                    if (volunteers[j]->canTakeOrder(*pendingOrders[i]))
                    {
                        volunteers[j]->acceptOrder(*pendingOrders[i]);
                        pendingOrders[i]->setCollectorId(volunteers[j]->getId());
                        pendingOrders[i]->setStatus(OrderStatus::COLLECTING);
                        wareHouse.addToInProcess(pendingOrders[i]);
                        toRemove.push_back(pendingOrders[i]->getId());
                        break;
                    }
                }
            }
            else if (pendingOrders[i]->getStatus() == OrderStatus::COLLECTING)
            {
                bool isInProcess = false;
                for (size_t z = 0; z < inProcessOrders.size(); z++)
                {

                    if (inProcessOrders[z]->getId() == pendingOrders[i]->getId())
                    {
                        isInProcess = true;
                        break;
                    }
                }
                if (!isInProcess)
                {

                    for (size_t j = 0; j < volunteers.size(); j++)
                    {
                        if (volunteers[j]->canTakeOrder(*pendingOrders[i]))
                        {
                            volunteers[j]->acceptOrder(*pendingOrders[i]);
                            pendingOrders[i]->setDriverId(volunteers[j]->getId());
                            pendingOrders[i]->setStatus(OrderStatus::DELIVERING);
                            wareHouse.addToInProcess(pendingOrders[i]);
                            toRemove.push_back(pendingOrders[i]->getId());
                            break;
                        }
                    }
                }
            }
        }

        for (int i = toRemove.size() - 1; i >= 0; i--)
        {
            wareHouse.removeFromPending(toRemove[i]);
        }
        toRemove.clear();

        for (size_t i = 0; i < volunteers.size(); i++)
        {
            if (volunteers[i]->isBusy())
            {
                volunteers[i]->step();
            }

            if (!volunteers[i]->isBusy())
            {
                if (volunteers[i]->getCompletedOrderId() != -1)
                {
                    for (size_t j = 0; j < inProcessOrders.size(); j++)
                    {
                        if (inProcessOrders[j]->getId() == volunteers[i]->getCompletedOrderId())
                        {
                            if (inProcessOrders[j]->getStatus() == OrderStatus::COLLECTING)
                            {
                                wareHouse.addToPending(inProcessOrders[j]);
                                toRemove.push_back(inProcessOrders[j]->getId());
                            }
                            else if (inProcessOrders[j]->getStatus() == OrderStatus::DELIVERING)
                            {
                                if (!wareHouse.getVolunteer(inProcessOrders[j]->getDriverId()).isBusy())
                                {
                                    wareHouse.addToCompleted(inProcessOrders[j]);
                                    inProcessOrders[j]->setStatus(OrderStatus::COMPLETED);
                                    toRemove.push_back(inProcessOrders[j]->getId());
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int i = toRemove.size() - 1; i >= 0; i--)
        {
            wareHouse.removeFromInProcess(toRemove[i]);
        }

        for (int i = volunteers.size() - 1; i >= 0; i--)
        {
            if (!volunteers[i]->hasOrdersLeft())
            {
                if (!volunteers[i]->isBusy())
                {
                    wareHouse.removeFromVolunteers(volunteers[i]->getId());
                }
            }
        }
    }
}

std::string SimulateStep::toString() const
{
    std::string s = "simulateStep " + std::to_string(numOfSteps);
    return s;
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}

///////////////////////////////////////add order/////////////////////////////////////////////
AddOrder::AddOrder(int id) : customerId(id){};

void AddOrder::act(WareHouse &wareHouse)
{
    if (customerId >= wareHouse.getCustomerCounter())
    {
        error("Error: Customer doesn't exist");
    }
    else
    {
        int dist = wareHouse.getCustomer(customerId).getCustomerDistance();
        Order *order = new Order(wareHouse.getOrderCounter(), customerId, dist);
        int succ = wareHouse.getCustomer(customerId).addOrder(order->getId());
        if (succ != -1)
        {
            wareHouse.addOrder(order);
            complete();
        }
        else
        {
            delete order;
            error("Error: Cannot place this order");
        }
    }
}
AddOrder *AddOrder::clone() const
{
    return new AddOrder(*this);
}
string AddOrder::toString() const
{
    std::string s = "order " + std::to_string(customerId);
    return s;
}

////////////////////////////////////////////Add Customer////////////////////////////////////////
AddCustomer::AddCustomer(string customerName, string customerType, int distance, int maxOrders)
    : customerName(customerName), customerType((customerType == "soldier") ? CustomerType::Soldier : CustomerType::Civilian), distance(distance), maxOrders(maxOrders)
{
}
void AddCustomer::act(WareHouse &wareHouse)
{
    if (customerType == CustomerType::Soldier)
    {
        SoldierCustomer *customer = new SoldierCustomer(wareHouse.getCustomerCounter(), customerName, distance, maxOrders);
        wareHouse.addCustomer(customer);
        complete();
    }
    else
    {
        CivilianCustomer *customer = new CivilianCustomer(wareHouse.getCustomerCounter(), customerName, distance, maxOrders);
        wareHouse.addCustomer(customer);
        complete();
    }
}

AddCustomer *AddCustomer::clone() const
{
    return new AddCustomer(*this);
}
string AddCustomer::toString() const
{
    std::string s1;
    if (customerType == CustomerType::Soldier)
    {
        s1 = "soldier";
    }
    else
    {
        s1 = "civilian";
    }
    std::string s = "customer " + customerName + " " + s1 + " " + std::to_string(distance) + " " + std::to_string(maxOrders);
    return s;
}

////////////////////////////////////////PrintOrderStatus///////////////////////////////////

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse)
{
    if (orderId >= wareHouse.getOrderCounter())
    {
        error("Error: Order doesn't exist");
    }
    else
    {
        std::cout << wareHouse.getOrder(orderId).toString() << std::endl;
        complete();
    }
}
PrintOrderStatus *PrintOrderStatus::clone() const
{
    return new PrintOrderStatus(*this);
}
string PrintOrderStatus::toString() const
{
    std::string s = "orderStatus " + std::to_string(orderId);
    return s;
}

////////////////////////////////////////PrintCustomerStatus///////////////////////////////////
PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse)
{
    if (customerId >= wareHouse.getCustomerCounter())
    {
        error("Error: Customer doesn't exist");
    }
    else
    {
        std::string s = "CustomerID: " + std::to_string(customerId) + "\n";
        std::string s2;
        for (size_t i = 0; i < wareHouse.getCustomer(customerId).getOrdersIds().size(); i++)
        {
            Order order = wareHouse.getOrder((wareHouse.getCustomer(customerId).getOrdersIds())[i]);
            s2 = "OrderId: " + std::to_string(order.getId()) + "\nOrderStatus: ";
            if (order.getStatus() == OrderStatus::PENDING)
            {
                s2 = s2 + "Pending";
            }
            else if (order.getStatus() == OrderStatus::COLLECTING)
            {
                s2 = s2 + "Collecting";
            }
            else if (order.getStatus() == OrderStatus::DELIVERING)
            {
                s2 = s2 + "Delivering";
            }
            else if (order.getStatus() == OrderStatus::COMPLETED)
            {
                s2 = s2 + "Completed";
            }
            s = s + s2 + "\n";
        }
        std::string s3 = "numOrdersLeft: " + std::to_string(wareHouse.getCustomer(customerId).getMaxOrders() - wareHouse.getCustomer(customerId).getNumOrders());

        s = s + s3;
        std::cout << s << std::endl;
        complete();
    }
}
PrintCustomerStatus *PrintCustomerStatus::clone() const
{
    return new PrintCustomerStatus(*this);
}
string PrintCustomerStatus::toString() const
{
    std::string s = "customerStatus " + std::to_string(customerId);
    return s;
}

////////////////////////////////////////PrintVolunteerStatus/////////////////////////////////

PrintVolunteerStatus::PrintVolunteerStatus(int id) : VolunteerId(id)
{
}

void PrintVolunteerStatus::act(WareHouse &wareHouse)
{
    if (!wareHouse.isVolunteerExists(VolunteerId))
    {
        error("Error: Volunteer doesn't exist");
    }
    else
    {
        std::cout << wareHouse.getVolunteer(VolunteerId).toString() << std::endl;
        complete();
    }
}
PrintVolunteerStatus *PrintVolunteerStatus::clone() const
{
    return new PrintVolunteerStatus(*this);
}
string PrintVolunteerStatus::toString() const
{
    std::string s = "volunteerStatus " + std::to_string(VolunteerId);
    return s;
}

/////////////////////////////////////////////////PrintActionsLog/////////////////////////////////////////

PrintActionsLog::PrintActionsLog()
{
}

void PrintActionsLog::act(WareHouse &wareHouse)
{
    const vector<BaseAction *> &actions = wareHouse.getActions();
    std::string s;
    for (const BaseAction *action : actions)
    {
        s = s + action->toString() + " ";
        if (action->getStatus() == ActionStatus::COMPLETED)
        {
            s = s + "COMPLETED \n";
        }
        else if (action->getStatus() == ActionStatus::ERROR)
        {
            s = s + "ERROR \n";
        }
    }
    std::cout << s << std::endl;
    complete();
}
PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}
string PrintActionsLog::toString() const
{
    std::string s = "log";
    return s;
}

//////////////////////////////////////////////Close///////////////////////////////////////////////

Close::Close() {}

void Close::act(WareHouse &wareHouse)
{
    vector<Order *> pendingOrders = wareHouse.getPendingOrders();
    vector<Order *> inProcessOrders = wareHouse.getInProcessOrders();
    vector<Order *> completedOrders = wareHouse.getCompletedOrders();
    for (size_t i = 0; i < pendingOrders.size(); i++)
    {
        std::string s1;
        std::string s2;
        s1 = "OrderID: " + std::to_string(pendingOrders[i]->getId()) + ", CustomerID: " + std::to_string(pendingOrders[i]->getCustomerId()) + ", ";
        if (pendingOrders[i]->getStatus() == OrderStatus::PENDING)
        {
            s2 = "Status: PENDING";
        }
        else if (pendingOrders[i]->getStatus() == OrderStatus::COLLECTING)
        {
            s2 = "Status: COLLECTING";
        }
        else if (pendingOrders[i]->getStatus() == OrderStatus::DELIVERING)
        {
            s2 = "Status: DELIVERING";
        }
        else if (pendingOrders[i]->getStatus() == OrderStatus::COMPLETED)
        {
            s2 = "Status: COMPLETED";
        }
        std::cout << s1 + s2 << std::endl;
    }
    for (size_t i = 0; i < inProcessOrders.size(); i++)
    {
        std::string s1;
        std::string s2;
        s1 = "OrderId: " + std::to_string(inProcessOrders[i]->getId()) + ", CustomerId " + std::to_string(inProcessOrders[i]->getCustomerId()) + ", ";
        if (inProcessOrders[i]->getStatus() == OrderStatus::PENDING)
        {
            s2 = "Status: PENDING";
        }
        else if (inProcessOrders[i]->getStatus() == OrderStatus::COLLECTING)
        {
            s2 = "Status: COLLECTING";
        }
        else if (inProcessOrders[i]->getStatus() == OrderStatus::DELIVERING)
        {
            s2 = "Status: DELIVERING";
        }
        else if (inProcessOrders[i]->getStatus() == OrderStatus::COMPLETED)
        {
            s2 = "Status: COMPLETED";
        }
        std::cout << s1 + s2 << std::endl;
    }
    for (size_t i = 0; i < completedOrders.size(); i++)
    {
        std::string s1;
        std::string s2;
        s1 = "OrderId: " + std::to_string(completedOrders[i]->getId()) + ", CustomerId " + std::to_string(completedOrders[i]->getCustomerId()) + ", ";
        if (completedOrders[i]->getStatus() == OrderStatus::PENDING)
        {
            s2 = "Status: PENDING";
        }
        else if (completedOrders[i]->getStatus() == OrderStatus::COLLECTING)
        {
            s2 = "Status: COLLECTING";
        }
        else if (completedOrders[i]->getStatus() == OrderStatus::DELIVERING)
        {
            s2 = "Status: DELIVERING";
        }
        else if (completedOrders[i]->getStatus() == OrderStatus::COMPLETED)
        {
            s2 = "Status: COMPLETED";
        }
        std::cout << s1 + s2 << std::endl;
    }

    wareHouse.close();
}

Close *Close::clone() const
{
    return new Close(*this);
}

string Close::toString() const
{
    return "Close";
}

//////////////////////////////////////////////BackupWareHouse//////////////////////////////////////////////////////////////
extern WareHouse *backup;
BackupWareHouse::BackupWareHouse() {}

void BackupWareHouse::act(WareHouse &wareHouse)
{
    if (backup != nullptr)
    {
        delete backup;
    }
    backup = new WareHouse(wareHouse);
}

BackupWareHouse *BackupWareHouse::clone() const
{
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const
{

    return "Backup";
}

/////////////////////////////////////////////RestoreWareHouse/////////////////////////////////////////////

RestoreWareHouse::RestoreWareHouse() {}

void RestoreWareHouse::act(WareHouse &wareHouse)
{
    if (backup == nullptr)
    {
        // No backup available
        error("Error: No backup available");
    }
    else
    {
        wareHouse = *backup;
    }
}

RestoreWareHouse *RestoreWareHouse::clone() const
{
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const
{

    return "Restore WareHouse";
}