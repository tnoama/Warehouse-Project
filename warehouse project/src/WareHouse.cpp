#include "../include/WareHouse.h"
#include "../include/Volunteer.h"
#include "../include/Action.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace std
{
    template <typename T, typename U = T>
    T exchange(T &obj, U &&new_value)
    {
        T old_value = std::move(obj);
        obj = std::forward<U>(new_value);
        return old_value;
    }
}

/////////////////////////////////////////////////costructor////////////////////////////////////////////////////

WareHouse::WareHouse(const string &configFilePath)
    : isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0), NO_Customer(new CivilianCustomer(-1, "", -1, -1)), NO_Volunteer(new CollectorVolunteer(-1, "", -1)), NO_Order(new Order(-1, -1, -1))
{
    std::cout << "Config File Path: " << configFilePath << std::endl;

    // Open the file
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open())
    {
        std::cerr << "Error: Unable to open the file." << std::endl;
        return;
    }

    // Read and parse the file content
    std::string currentLine;
    while (std::getline(configFile, currentLine))
    {
        // Find the position of '#'
        size_t hashPosition = currentLine.find('#');

        // Extract the part before '#'
        std::string lineBeforeHash = currentLine.substr(0, hashPosition);

        // Find the position of the first space
        size_t firstSpacePosition = lineBeforeHash.find(' ');

        if (!lineBeforeHash.empty() && firstSpacePosition != std::string::npos &&
            firstSpacePosition != 0)
        {

            std::istringstream iss(lineBeforeHash);
            std::vector<std::string> words;
            std::string word;
            while (iss >> word)
            {
                words.push_back(word);
            }

            if (words[0] == "volunteer")
            {
                if (words[2] == "limited_collector")
                {
                    LimitedCollectorVolunteer *lcv = new LimitedCollectorVolunteer(volunteerCounter, words[1], std::stoi(words[3]), std::stoi(words[4]));
                    volunteers.push_back(lcv);

                    // this->volunteers.push_back(new LimitedCollectorVolunteer(volunteerCounter, words[1], std::stoi(words[3]), std::stoi(words[4])));
                    volunteerCounter++;
                }
                else if (words[2] == "collector")
                {
                    CollectorVolunteer *cv = new CollectorVolunteer(volunteerCounter, words[1], std::stoi(words[3]));
                    volunteers.push_back(cv);
                    // this->volunteers.push_back(new CollectorVolunteer(volunteerCounter, words[1], std::stoi(words[3])));
                    volunteerCounter++;
                }
                else if (words[2] == "driver")
                {
                    DriverVolunteer *dv = new DriverVolunteer(volunteerCounter, words[1], std::stoi(words[3]), std::stoi(words[4]));
                    volunteers.push_back(dv);
                    // this->volunteers.push_back(new DriverVolunteer(volunteerCounter, words[1], std::stoi(words[3]), std::stoi(words[4])));
                    volunteerCounter++;
                }
                else if (words[2] == "limited_driver")
                {
                    LimitedDriverVolunteer *ldv = new LimitedDriverVolunteer(volunteerCounter, words[1], std::stoi(words[3]), std::stoi(words[4]), std::stoi(words[5]));
                    volunteers.push_back(ldv);
                    // this->volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, words[1], std::stoi(words[3]), std::stoi(words[4]), std::stoi(words[5])));
                    volunteerCounter++;
                }
            }
            else if (words[0] == "customer")
            {
                if (words[2] == "soldier")
                {
                    // this->customers.push_back(new SoldierCustomer(customerCounter, words[1], std::stoi(words[3]), std::stoi(words[4])));
                    // customerCounter++;
                    addCustomer(new SoldierCustomer(customerCounter, words[1], std::stoi(words[3]), std::stoi(words[4])));
                }
                else if (words[2] == "civilian")
                {
                    // this->customers.push_back(new CivilianCustomer(customerCounter, words[1], std::stoi(words[3]), std::stoi(words[4])));
                    // customerCounter++;
                    addCustomer(new CivilianCustomer(customerCounter, words[1], std::stoi(words[3]), std::stoi(words[4])));
                }
            }
        }
    }

    configFile.close();
}

//////////////////////////////////////////////////////copy constructor//////////////////////////////////////////////
WareHouse::WareHouse(const WareHouse &other)
    : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter), NO_Customer(new CivilianCustomer(-1, "", -1, -1)), NO_Volunteer(new CollectorVolunteer(-1, "", -1)), NO_Order(new Order(-1, -1, -1))
{
    // Copy volunteers
    for (const Volunteer *volunteer : other.volunteers)
    {
        this->volunteers.push_back(volunteer->clone());
    }

    // Copy customers
    for (const Customer *customer : other.customers)
    {
        this->customers.push_back(customer->clone());
    }

    // Copy orders
    for (const Order *order : other.pendingOrders)
    {
        this->pendingOrders.push_back(order->clone());
    }
    for (const Order *order : other.inProcessOrders)
    {
        this->inProcessOrders.push_back(order->clone());
    }
    for (const Order *order : other.completedOrders)
    {
        this->completedOrders.push_back(order->clone());
    }

    // Copy actions
    for (const BaseAction *action : other.actionsLog)
    {
        this->actionsLog.push_back(action->clone());
    }
}

//////////////////////////////////////////////copy assignment operator///////////////////////////////////////////

WareHouse &WareHouse::operator=(const WareHouse &other)
{
    if (this != &other)
    {

        for (Volunteer *volunteer : volunteers)
        {
            delete volunteer;
        }
        volunteers.clear();

        for (BaseAction *action : actionsLog)
        {
            delete action;
        }
        actionsLog.clear();

        for (Order *order : pendingOrders)
        {
            delete order;
        }
        pendingOrders.clear();

        for (Order *order : inProcessOrders)
        {
            delete order;
        }
        inProcessOrders.clear();

        for (Order *order : completedOrders)
        {
            delete order;
        }
        completedOrders.clear();

        for (Customer *customer : customers)
        {
            delete customer;
        }
        customers.clear();

        for (const Volunteer *volunteer : other.volunteers)
        {
            volunteers.push_back(volunteer->clone());
        }

        for (const BaseAction *action : other.actionsLog)
        {
            actionsLog.push_back(action->clone());
        }

        for (const Order *order : other.pendingOrders)
        {
            pendingOrders.push_back(order->clone());
        }

        for (const Order *order : other.inProcessOrders)
        {
            inProcessOrders.push_back(order->clone());
        }

        for (const Order *order : other.completedOrders)
        {
            completedOrders.push_back(order->clone());
        }

        for (const Customer *customer : other.customers)
        {
            customers.push_back(customer->clone());
        }

        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
    }
    return *this;
}
////////////////////////////////////////////////move constructor////////////////////////////////////////////////////

WareHouse::WareHouse(WareHouse &&other) noexcept
    : isOpen(std::exchange(other.isOpen, false)),
      //   actionsLog(std::move(other.actionsLog)),
      //   volunteers(std::move(other.volunteers)),
      //   pendingOrders(std::move(other.pendingOrders)),
      //   inProcessOrders(std::move(other.inProcessOrders)),
      //   completedOrders(std::move(other.completedOrders)),
      //   customers(std::move(other.customers)),
      actionsLog(),
      volunteers(),
      pendingOrders(),
      inProcessOrders(),
      completedOrders(),
      customers(),
      customerCounter(std::exchange(other.customerCounter, 0)),
      volunteerCounter(std::exchange(other.volunteerCounter, 0)),
      orderCounter(std::exchange(other.orderCounter, 0)),
      NO_Customer(other.NO_Customer),
      NO_Volunteer(other.NO_Volunteer),
      NO_Order(other.NO_Order)
{
    other.NO_Customer = nullptr;
    other.NO_Volunteer = nullptr;
    other.NO_Order = nullptr;

    for (BaseAction *action : other.actionsLog)
    {
        this->actionsLog.push_back(std::move(action));
    }
    for (Volunteer *volunteer : other.volunteers)
    {
        this->volunteers.push_back(std::move(volunteer));
    }
    for (Order *order : other.pendingOrders)
    {
        this->pendingOrders.push_back(std::move(order));
    }
    for (Order *order : other.inProcessOrders)
    {
        this->inProcessOrders.push_back(std::move(order));
    }
    for (Order *order : other.completedOrders)
    {
        this->completedOrders.push_back(std::move(order));
    }
    for (Customer *customer : other.customers)
    {
        this->customers.push_back(std::move(customer));
    }
    other.actionsLog.clear();
    other.volunteers.clear();
    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.customers.clear();
}

//////////////////////////////////////////////move assignment operator///////////////////////////////////////////

WareHouse &WareHouse::operator=(WareHouse &&other) noexcept
{
    if (this != &other) // Self-assignment check
    {

        // emptying this vectors information
        for (BaseAction *action : actionsLog)
        {
            delete action;
        }
        for (Volunteer *volunteer : volunteers)
        {
            delete volunteer;
        }
        for (Order *order : pendingOrders)
        {
            delete order;
        }
        for (Order *order : inProcessOrders)
        {
            delete order;
        }
        for (Order *order : completedOrders)
        {
            delete order;
        }
        for (Customer *customer : customers)
        {
            delete customer;
        }

        // moving others information into this
        isOpen = std::move(other.isOpen);
        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = std::move(other.customers);
        customerCounter = std::move(other.customerCounter);
        volunteerCounter = std::move(other.volunteerCounter);
        orderCounter = std::move(other.orderCounter);
        NO_Order = other.NO_Order;
        NO_Volunteer = other.NO_Volunteer;
        NO_Customer = other.NO_Customer;

        // clearing others fields
        other.actionsLog.clear();
        other.volunteers.clear();
        other.pendingOrders.clear();
        other.inProcessOrders.clear();
        other.completedOrders.clear();
        other.customers.clear();
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        other.NO_Order = nullptr;
        other.NO_Volunteer = nullptr;
        other.NO_Customer = nullptr;
    }
    return *this;
}

//////////////////////////////////////////////////distructor//////////////////////////////////////////////////

WareHouse::~WareHouse()
{
    delete NO_Order;
    delete NO_Volunteer;
    delete NO_Customer;

    // Delete dynamically allocated orders
    for (Order *order : pendingOrders)
    {
        delete order;
    }

    for (Order *order : inProcessOrders)
    {
        delete order;
    }

    for (Order *order : completedOrders)
    {
        delete order;
    }

    // Delete dynamically allocated volunteers
    for (Volunteer *volunteer : volunteers)
    {
        delete volunteer;
    }

    // Delete dynamically allocated customers
    for (Customer *customer : customers)
    {
        delete customer;
    }

    // Delete dynamically allocated actions
    for (BaseAction *action : actionsLog)
    {
        delete action;
    }

    pendingOrders.clear();
    inProcessOrders.clear();
    completedOrders.clear();
    volunteers.clear();
    customers.clear();
    actionsLog.clear();
}

void WareHouse::open()
{
    isOpen = true;
}
void WareHouse::close()
{
    isOpen = false;
}
///////////////////////////////////////////////////////////start/////////////////////////////////////////////////////
void WareHouse::start()
{
    std::cout << "Warehouse is open!" << std::endl;

    open();

    while (isOpen)
    {
        std::cout << "Enter an action ('close' to exit): ";
        std::string actionInfo;
        std::getline(std::cin, actionInfo);

        std::istringstream iss(actionInfo);
        std::string action;

        iss >> action;

        if (action == "close")
        {
            std::cout << "Exiting the warehouse simulation. Goodbye!" << std::endl;
            Close *close = new Close();
            actionsLog.push_back(close);
            close->act(*this);
        }
        else if (action == "step")
        {

            int numOfSteps;

            iss >> numOfSteps;

            SimulateStep *simulateStepAction = new SimulateStep(numOfSteps);
            actionsLog.push_back(simulateStepAction);
            simulateStepAction->act(*this);
        }
        else if (action == "order")
        {

            int customerId;

            iss >> customerId;

            AddOrder *addOrderAction = new AddOrder(customerId);
            actionsLog.push_back(addOrderAction);
            addOrderAction->act(*this);
        }
        else if (action == "customer")
        {

            std::string customerName;
            std::string customerType;
            int distance;
            int maxOrders;

            iss >> customerName >> customerType >> distance >> maxOrders;

            AddCustomer *customer = new AddCustomer(customerName, customerType, distance, maxOrders);
            actionsLog.push_back(customer);
            customer->act(*this);
        }

        else if (action == "orderStatus")
        {

            int orderId;
            iss >> orderId;
            PrintOrderStatus *printOrderStatusAction = new PrintOrderStatus(orderId);
            actionsLog.push_back(printOrderStatusAction);
            printOrderStatusAction->act(*this);
        }
        else if (action == "customerStatus")
        {
            int customerId;
            iss >> customerId;

            PrintCustomerStatus *printCustomerStatus = new PrintCustomerStatus(customerId);
            actionsLog.push_back(printCustomerStatus);
            printCustomerStatus->act(*this);
        }
        else if (action == "volunteerStatus")
        {
            int volunteerId;
            iss >> volunteerId;

            PrintVolunteerStatus *printVolunteerStatus = new PrintVolunteerStatus(volunteerId);
            actionsLog.push_back(printVolunteerStatus);
            printVolunteerStatus->act(*this);
        }
        else if (action == "log")
        {
            PrintActionsLog *printActionsLog = new PrintActionsLog();
            printActionsLog->act(*this);
            actionsLog.push_back(printActionsLog);
        }
        else if (action == "backup")
        {
            BackupWareHouse *backupWareHouse = new BackupWareHouse();
            backupWareHouse->act(*this);
            actionsLog.push_back(backupWareHouse);
        }
        else if (action == "restore")
        {
            RestoreWareHouse *restoreWareHouse = new RestoreWareHouse();
            restoreWareHouse->act(*this);
            actionsLog.push_back(restoreWareHouse);
        }
        else
        {
            std::cout << "Unknown action. Please try again." << std::endl;
        }
    }
}

const vector<BaseAction *> &WareHouse::getActions() const
{
    return actionsLog;
}

void WareHouse::addOrder(Order *order)
{
    order->setStatus(OrderStatus::PENDING);
    pendingOrders.push_back(order);
    orderCounter++;
}
void WareHouse::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    for (size_t i = 0; i < volunteers.size(); i++)
    {
        if (volunteers[i]->getId() == volunteerId)
        {
            return *volunteers[i];
        }
    }
    return *NO_Volunteer;
}

Customer &WareHouse::getCustomer(int customerId) const
{
    for (size_t i = 0; i < customers.size(); i++)
    {
        if (customers[i]->getId() == customerId)
        {
            return *customers[i];
        }
    }

    return *NO_Customer;
}

Order &WareHouse::getOrder(int orderId) const
{
    bool found = false;
    for (size_t i = 0; i < pendingOrders.size(); i++)
    {
        if (pendingOrders[i]->getId() == orderId)
        {
            found = true;
            return *pendingOrders[i];
        }
    }
    if (!found)
    {
        for (size_t i = 0; i < inProcessOrders.size(); i++)
        {
            if (inProcessOrders[i]->getId() == orderId)
            {
                found = true;
                return *inProcessOrders[i];
            }
        }
    }
    if (!found)
    {
        for (size_t i = 0; i < completedOrders.size(); i++)
        {
            if (completedOrders[i]->getId() == orderId)
            {
                found = true;
                return *completedOrders[i];
            }
        }
    }

    return *NO_Order;
}

const vector<Order *> &WareHouse::getPendingOrders() const
{
    return pendingOrders;
}
const vector<Order *> &WareHouse::getInProcessOrders() const
{
    return inProcessOrders;
}
const vector<Order *> &WareHouse::getCompletedOrders() const
{
    return completedOrders;
}

const vector<Volunteer *> &WareHouse::getVolunteers() const
{
    return volunteers;
}

void WareHouse::addToPending(Order *order)
{
    pendingOrders.push_back(order);
}
void WareHouse::removeFromPending(int id)
{
    int size = pendingOrders.size();
    for (int i = 0; i < size; i++)
    {
        if (pendingOrders[i]->getId() == id)
        {
            pendingOrders.erase(pendingOrders.begin() + i);
            break;
        }
    }
}
void WareHouse::addToInProcess(Order *order)
{
    inProcessOrders.push_back(order);
}
void WareHouse::removeFromInProcess(int id)
{
    int size = inProcessOrders.size();
    for (int i = 0; i < size; i++)
    {
        if (inProcessOrders[i]->getId() == id)
        {
            inProcessOrders.erase(inProcessOrders.begin() + i);
            break;
        }
    }
}
void WareHouse::addToCompleted(Order *order)
{
    completedOrders.push_back(order);
}

void WareHouse::removeFromVolunteers(int id)
{
    int size = volunteers.size();
    for (int i = 0; i < size; i++)
    {
        if (volunteers[i]->getId() == id)
        {
            delete volunteers[i];
            volunteers.erase(volunteers.begin() + i);
            break;
        }
    }
}

void WareHouse::addCustomer(Customer *customer)
{
    customers.push_back(customer);
    customerCounter++;
}

int WareHouse::getCustomerCounter() const
{
    return customerCounter;
}
int WareHouse::getVolunteerCounter() const
{
    return volunteerCounter;
}
int WareHouse::getOrderCounter() const
{
    return orderCounter;
}
void WareHouse::addToOrderCounter()
{
    orderCounter++;
}
void WareHouse::addToVolunteerCounterr()
{
    volunteerCounter++;
}
void WareHouse::addToCustomerCounter()
{
    customerCounter++;
}
const bool WareHouse::isVolunteerExists(int id)
{
    for (size_t i = 0; i < getVolunteers().size(); i++)
    {
        if (getVolunteers()[i]->getId() == id)
        {
            return true;
        }
    }
    return false;
}
