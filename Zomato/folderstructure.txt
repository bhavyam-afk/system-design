OnlineFoodOrderingSystem/
│
├── main.cpp                    # Composition root and entry point
├── TomatoApp.h                # Facade class (main orchestrator)
│
├── models/
│   ├── MenuItem.h
│   ├── Restaurant.h
│   ├── User.h
│   ├── Cart.h
│   ├── Order.h                # Abstract Order
│   ├── DeliveryOrder.h
│   ├── PickupOrder.h
│
├── managers/
│   ├── RestaurantManager.h
│   ├── OrderManager.h
│
├── strategies/
│   ├── PaymentStrategy.h      # Base class
│   ├── CreditCardPaymentStrategy.h
│   ├── UpiPaymentStrategy.h
│
├── factories/
│   ├── OrderFactory.h         # Abstract factory
│   ├── NowOrderFactory.h
│   ├── ScheduledOrderFactory.h
│
├── services/
│   └── NotificationService.h
│
├── utils/
│   └── TimeUtils.h