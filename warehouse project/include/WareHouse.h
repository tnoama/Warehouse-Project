#pragma once
#include <string>
#include <vector>
using namespace std;

#include "Order.h"
#include "Customer.h"
#include "Volunteer.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers and Actions.

class WareHouse
{

public:
    WareHouse(const string &configFilePath);
    void start();
    const vector<BaseAction *> &getActions() const;
    void addOrder(Order *order);
    void addAction(BaseAction *action);
    Customer &getCustomer(int customerId) const;
    Volunteer &getVolunteer(int volunteerId) const;
    Order &getOrder(int orderId) const;
    void close();
    void open();

    // we added these functions
    void addToPending(Order *order);
    void removeFromPending(int id);
    void addToInProcess(Order *order);
    void removeFromInProcess(int id);
    void addToCompleted(Order *order);
    void removeFromVolunteers(int id);
    void addCustomer(Customer *customer);
    int getCustomerCounter() const;
    int getVolunteerCounter() const;
    int getOrderCounter() const;
    void addToOrderCounter();
    void addToVolunteerCounterr();
    void addToCustomerCounter();
    ~WareHouse();                                     // distructor
    WareHouse(WareHouse &&other) noexcept;            // move constructor
    WareHouse &operator=(WareHouse &&other) noexcept; // move assignment operator
    WareHouse &operator=(const WareHouse &other);     // copy assignment operator
    WareHouse(const WareHouse &other);                // copy constructor

    const vector<Order *> &getPendingOrders() const;
    const vector<Order *> &getInProcessOrders() const;
    const vector<Order *> &getCompletedOrders() const;
    const vector<Volunteer *> &getVolunteers() const;
    const bool isVolunteerExists(int id);

    const Customer &getNoCustomer() const;
    const Volunteer &getNoVolunteer() const;
    const Order &getNoOrder() const;

private:
    bool isOpen;
    vector<BaseAction *> actionsLog;
    vector<Volunteer *> volunteers;
    vector<Order *> pendingOrders;
    vector<Order *> inProcessOrders;
    vector<Order *> completedOrders;
    vector<Customer *> customers;
    int customerCounter;  // For assigning unique customer IDs
    int volunteerCounter; // For assigning unique volunteer IDs
    int orderCounter;

    CivilianCustomer *NO_Customer;
    CollectorVolunteer *NO_Volunteer;
    Order *NO_Order;
};