#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

/*============================
   Notification & Decorators
=============================*/

class INotification {
public:
    virtual string getContent() const = 0;

    virtual ~INotification() {}
};

// Concrete Notification: simple text notification.
class SimpleNotification : public INotification {
private:
    string text;
public:
    SimpleNotification(const string& msg) {
        text = msg;
    }
    string getContent() const override {
        return text;
    }
};

// Abstract Decorator: wraps a Notification object.
class INotificationDecorator : public INotification {
protected:
    INotification* notification;
public:
    INotificationDecorator(INotification* n) {
        notification = n;
    }
    virtual ~INotificationDecorator() {
        delete notification;
    }
};

// Decorator to add a timestamp to the content.
class TimestampDecorator : public INotificationDecorator {
public:
    TimestampDecorator(INotification* n) : INotificationDecorator(n) { }
    
    string getContent() const override {
        return "[2025-04-13 14:22:00] " + notification->getContent();
    }
};

// Decorator to append a signature to the content.
class SignatureDecorator : public INotificationDecorator {
private:
    string signature;
public:
    SignatureDecorator(INotification* n, const string& sig) : INotificationDecorator(n) {
        signature = sig;
    }
    string getContent() const override {
        return notification->getContent() + "\n-- " + signature + "\n\n";
    }
};

/*============================
  Observer Pattern Components
=============================*/

// Observer interface: each observer gets an update with a Notification pointer.
// update method is called when a notification is sent.
class IObserver {
public:
    virtual void update() = 0;

    virtual ~IObserver() {}
};

// register, unregister, broadcast.
class IObservable {
public:
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
};

// Concrete Observable
// this class's object means this notification is being observed and can be sent ONLY to the observers that are registered to it.
class NotificationObservable :  public IObservable {
private:
    INotification* currentNotification;
    vector<IObserver*> observers;
    
public:
    NotificationObservable() { 
        currentNotification = NULL; 
    }

    void addObserver(IObserver* obs) override {
        observers.push_back(obs);
    }

    void removeObserver(IObserver* obs) override {
        observers.erase(remove(observers.begin(), observers.end(), obs), observers.end());
    }

    void setNotification(INotification* notification) {
        if (currentNotification != nullptr) {
            delete currentNotification;
        }
        currentNotification = notification;

        for (unsigned long long i = 0; i < observers.size(); i++) {
            observers[i]->update();
        }
    }

    INotification* getNotification() const {
        return currentNotification;
    }

    string getNotificationContent() const {
        return currentNotification->getContent();
    }

    ~NotificationObservable() {
        if (currentNotification != NULL) {
            delete currentNotification;
        }
    }
};

/*============================
     NotificationService
=============================*/

// The NotificationService manages notifications. It keeps track of notifications. 
// Any client code will interact with this service.

// Singleton class
class NotificationService {
private:
    NotificationObservable* observable;
    static NotificationService* instance;
    vector<INotification*> notifications; // Keeps track of all notifications sent (maintains only SENT history).
    
    NotificationService() {
        // private constructor
        observable = new NotificationObservable();
    }
    
    public:
    static NotificationService* getInstance() {
        return instance;
    }

    // Expose the observable so observers can attach.
    NotificationObservable* getObservable() {
        return observable;
    }

    // Creates a new Notification and notifies observers.
    void sendNotification(INotification* notification) {
        notifications.push_back(notification);
        observable->setNotification(notification);
    }

    ~NotificationService() {
        delete observable;
    }
};

NotificationService* NotificationService::instance = new NotificationService();

/*==============================
       ConcreteObservers
===============================*/
class Logger : public IObserver {
private:
    NotificationObservable* notificationObservable;

public:
    Logger() {
       this->notificationObservable = NotificationService::getInstance()->getObservable();
       notificationObservable->addObserver(this);
    }

    Logger(NotificationObservable* observable) {
        notificationObservable = observable;
        notificationObservable->addObserver(this);
    }

    // updates (sends) notification to the user via logging mechanism. In this case, it simply prints to the console.
    void update() {
        cout << "Logging Notification : \n" << notificationObservable->getNotificationContent();
    }
};

/*==================================================
  Strategy Pattern Components (Concrete Observer 2)
===================================================*/

// Abstract class for different Notification Strategies.
class INotificationStrategy {
public:    
    virtual void sendNotification(string content) = 0;
};

class EmailStrategy : public INotificationStrategy {
private:
    string emailId;
public:

    EmailStrategy(string emailId) {
        this->emailId = emailId;
    }

    void sendNotification(string content) override {
        // Simulate the process of sending an email notification, 
        // representing the dispatch of messages to users via email.​
        cout << "Sending email Notification to: " << emailId << "\n" << content;
    }
};

class SMSStrategy : public INotificationStrategy {
private:
    string mobileNumber;
public:

    SMSStrategy(string mobileNumber) {
        this->mobileNumber = mobileNumber;
    }

    void sendNotification(string content) override {
        // Simulate the process of sending an SMS notification, 
        // representing the dispatch of messages to users via SMS.​
        cout << "Sending SMS Notification to: " << mobileNumber << "\n" << content;
    }
};

class PopUpStrategy : public INotificationStrategy {
public:
    void sendNotification(string content) override {
        // Simulate the process of sending popup notification.
        cout << "Sending Popup Notification: \n" << content;
    }
};

class NotificationEngine : public IObserver {
private:
    NotificationObservable* notificationObservable;
    vector<INotificationStrategy*> notificationStrategies;

public:
    NotificationEngine() {
        this->notificationObservable = NotificationService::getInstance()->getObservable();
        notificationObservable->addObserver(this);
    }

    NotificationEngine(NotificationObservable* observable) {
        this->notificationObservable = observable;
        notificationObservable->addObserver(this);
    }

    void addNotificationStrategy(INotificationStrategy* ns) {
        this->notificationStrategies.push_back(ns);
    }

    void update() {
        string notificationContent = notificationObservable->getNotificationContent();
        for(const auto notificationStrategy : notificationStrategies) {
            notificationStrategy->sendNotification(notificationContent);
        }
    }
};

int main() {
    // Create NotificationService.
    NotificationService* notificationService = NotificationService::getInstance();
   
    // Create Logger Observer
    Logger* logger = new Logger();

    // Create NotificationEngine observers.
    NotificationEngine* notificationEngine = new NotificationEngine();
    
    logger->update(); // Log the notification to console.
    notificationEngine->addNotificationStrategy(new EmailStrategy("random.person@gmail.com"));
    notificationEngine->addNotificationStrategy(new SMSStrategy("+91 9876543210"));
    notificationEngine->addNotificationStrategy(new PopUpStrategy());
    
    // Create a notification with decorators.
    INotification* notification = new SimpleNotification("Your order has been shipped!");
    notification = new TimestampDecorator(notification);
    notification = new SignatureDecorator(notification, "Customer Care");
    
    notificationService->sendNotification(notification);

    delete logger;
    delete notificationEngine;
    return 0;
}