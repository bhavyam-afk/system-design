#include <iostream>
#include "TomatoApp.h"
using namespace std;

int main() {
    // Create TomatoApp Object
    TomatoApp* tomato = new TomatoApp();

    // Simulate a user coming in (Happy Flow)
    User* user = new User(101, "Aditya", "Delhi");
    cout << "User: " << user->getName() << " is active." << endl;

    // User searches for restaurants by location
    vector<Restaurant*> restaurantList = tomato->searchRestaurants("Delhi");

    if (restaurantList.empty()) {
        cout << "No restaurants found!" << endl;
        return 0;
    }
    cout << "Found Restaurants:" << endl;
    for (auto restaurant : restaurantList) {
        cout << " - " << restaurant->getName() << endl;
    }

    // User selects a restaurant
    tomato->selectRestaurant(user, restaurantList[0]);

    cout << "Selected restaurant: " << restaurantList[0]->getName() << endl;

    // User adds items to the cart
    tomato->addToCart(user, "P1");
    tomato->addToCart(user, "P2");

    tomato->printUserCart(user);

    // User checkout the cart
    Order* order = tomato->checkoutNow(user, "Delivery", new UpiPaymentStrategy("1234567890"));
    
    // User pay for the cart. If payment is success, notification is sent.
    tomato->payForOrder(user, order);

    // Cleanup Code.
    delete tomato;
    delete user;
    
    return 0;
}