# Zomato (Tomato) — LLD Interview Cheat Sheet

This cheat sheet explains the low‑level design for the `Zomato` sample app in this workspace. It maps the code to responsibilities, design patterns, runtime flow, tradeoffs, and interview talking points. Use it during interviews to explain components, reason about tradeoffs, and propose improvements.

Files to reference
- `main.cpp` — composition root / example happy flow
- `TomatoApp.h` — application facade and orchestration
- `models/` — data models: `MenuItem.h`, `Restaurant.h`, `User.h`, `Cart.h`, `Order.h`, `DeliveryOrder.h`, `PickupOrder.h`
- `manager/RestaurantManager.h` — singleton registry for restaurants
- `factories/` — `OrderFactory.h`, `NewOrderfactory.h` (NowOrderFactory), `ScheduledOrderFactory.h`
- `strategies/` — `PaymentStrategy.h`, `UpiPaymentStrategy.h`, `CreditCardPaymentStrategy.h`
- `services/NotificationService.h` — notification delivery
- `utils/TimeUtils.h` — helper for timestamps
- `UML.png` — class diagram to refer to while explaining structure

High level flow (happy path)
1. `main.cpp` creates `TomatoApp` and a `User`.
2. User searches restaurants by location: `TomatoApp::searchRestaurants()` delegates to `RestaurantManager::getInstance()->searchByLocation()`.
3. User selects a `Restaurant`. The `Cart` (owned by `User`) stores the selected `Restaurant`.
4. User adds `MenuItem`s to `Cart` using `TomatoApp::addToCart()`.
5. User checks out: `TomatoApp::checkoutNow(user, "Delivery", new UpiPaymentStrategy(...))`.
   - `TomatoApp::checkout()` invokes an `OrderFactory` (Now or Scheduled) to create an `Order` (either `DeliveryOrder` or `PickupOrder`).
   - The `Order` is populated with `User`, `Restaurant`, `items`, `PaymentStrategy`, `total`, and `scheduled` time.
6. `OrderManager::getInstance()->addOrder(order)` is called (note: `OrderManager.h` is referenced but not present in repo — see Known issues below).
7. `TomatoApp::payForOrder()` calls `order->processPayment()`, which delegates to the `PaymentStrategy::pay(amount)` implementation. On success, `NotificationService::notify(order)` is invoked and the `User`'s `Cart` is cleared.

Component responsibilities
- TomatoApp (Facade / Orchestrator)
  - Coordinates the end‑to‑end flow: restaurant initialization, user interactions, checkout, payment and notifications.
  - Contains helper methods used by the app runner (`main.cpp`).

- Models
  - `MenuItem`: simple value object (code, name, price).
  - `Restaurant`: holds a `vector<MenuItem>` and metadata (name, location). `Restaurant::nextRestaurantId` is a simple static counter for IDs.
  - `User`: stores `userId`, `name`, `address` and a heap‑allocated `Cart` (ownership: `User` deletes `Cart` in destructor).
  - `Cart`: holds the currently selected `Restaurant*` and `vector<MenuItem> items`. Responsible for `addItem`, `getTotalCost`, `clear`, and validation that restaurant is set before adding items.
  - `Order` (abstract): encapsulates order data (user, restaurant, items, paymentStrategy, total, schedule). Subclasses: `DeliveryOrder`, `PickupOrder`.

- Managers
  - `RestaurantManager`: singleton that stores `Restaurant*` pointers and supports `searchByLocation`. Implemented with lazy initialization and a `vector<Restaurant*>` store.
  - `OrderManager`: referenced by code but missing in the repo. Expected to be a singleton that stores orders and exposes `addOrder`, `getOrders`, `listOrders`, and lifecycle management.

- Factories
  - `OrderFactory` (interface): `createOrder(...)` signature.
  - `NowOrderFactory` / `ScheduledOrderFactory`: concrete factories that create `DeliveryOrder` or `PickupOrder` depending on `orderType`. They encapsulate creation + initialization (user address, items, payment strategy, scheduled time).

- Strategies
  - `PaymentStrategy` (interface): declares `pay(double amount)`.
  - `UpiPaymentStrategy`, `CreditCardPaymentStrategy`: concrete implementations that print a payment result. Patterns used: Strategy pattern (runtime polymorphism to choose payment method).

- Services
  - `NotificationService`: provides a `notify(Order* order)` static method printing a summary. Invoked after a successful payment.

Design patterns used (explicit)
- Singleton: `RestaurantManager` (and expected `OrderManager`) implemented as singletons with lazy initialization.
- Strategy: `PaymentStrategy` with concrete `UpiPaymentStrategy` and `CreditCardPaymentStrategy`.
- Factory: `OrderFactory` with `NowOrderFactory` and `ScheduledOrderFactory` to abstract order creation.
- Facade / Orchestrator: `TomatoApp` acts as a facade over lower‑level subsystems.

Memory ownership & resource concerns
- Many objects are allocated with `new` and stored as raw pointers. Ownership is distributed (mixed):
  - `User` owns and deletes its `Cart`.
  - `Order` destructor deletes `paymentStrategy` (so `Order` assumes ownership of the PaymentStrategy pointer passed to it).
  - `RestaurantManager` stores `Restaurant*` in a vector; those restaurants are never deleted in the current code (OK for short‑lived test program, but leaks for long processes). Consider using smart pointers (std::unique_ptr or shared_ptr).
  - `NotificationService::notify` is static; nevertheless `TomatoApp::payForOrder` currently constructs a `NotificationService` instance unnecessarily.

Known issues / missing pieces
- `OrderManager.h` is referenced in `TomatoApp.h` but not present in the repository. This will prevent compilation. Implement a minimal `OrderManager` singleton that stores `Order*` or `shared_ptr<Order>` and exposes `addOrder()` and `listOrders()`.
- Several allocations are never freed (restaurants, factory objects created inline like `new NowOrderFactory()`), which is acceptable in short runs but should be cleaned up in production code or replaced by smart pointers.

Complexity & performance notes
- `RestaurantManager::searchByLocation(string loc)` uses an O(N) scan of the restaurants vector, converting strings to lowercase on the fly. For scale:
  - Use an index: unordered_map<string, vector<Restaurant*>> keyed by lowercase location to get O(1) lookups per location.
  - Support partial matching using a spatial index (geohash or R‑tree) or a text index if searching by free text.
- `Cart::getTotalCost()` is O(M) in number of items; acceptable as M is small per user.

Concurrency, scaling and production readiness
- Current app is single‑threaded and not safe for concurrent mutations.
- For a multi‑tenant server:
  - Replace singletons with injected instances (DI) so you can control lifecycle per process and test easily.
  - Make `RestaurantManager` and `OrderManager` thread‑safe (mutex or lock‑free structures), or use a persistent store (DB) as central source of truth.
  - Move business data to a persistent DB and use caches for read heavy operations (e.g., Redis index for restaurants by location).
  - For payment and notifications, integrate with external services via async workers and queues (Kafka/RabbitMQ). Use idempotency keys for retries.

Extensibility & LLD interview talking points
- How to add a new payment method: implement `PaymentStrategy` and pass it into the checkout call (no change to `Order` or `TomatoApp`) — Strategy pattern usage.
- How to add a new order type (e.g., Scheduled + Delivery with time windows): add a new `OrderFactory` or extend `ScheduledOrderFactory` and add a subclass of `Order` if new fields/behaviour needed.
- How to persist orders: plug in an `OrderRepository` (interface) and implement SQL/NoSQL adapters. `OrderManager` should delegate persistence to repository.
- How to make search faster: precompute indices or use an external search service. For geo queries use a spatial index with lat/long coordinates.

Sequence of method calls (call flow) — short form
1. main -> TomatoApp::searchRestaurants -> RestaurantManager::searchByLocation
2. main -> TomatoApp::selectRestaurant -> Cart::setRestaurant
3. main -> TomatoApp::addToCart -> Cart::addItem
4. main -> TomatoApp::checkoutNow -> TomatoApp::checkout -> OrderFactory::createOrder -> OrderManager::addOrder
5. main -> TomatoApp::payForOrder -> Order::processPayment -> PaymentStrategy::pay -> NotificationService::notify -> Cart::clear

Testing and verification
- Unit tests to add:
  - `RestaurantManager`: add/search by location, case‑insensitive behaviour.
  - `Cart`: addItem rejects if restaurant not set; total cost correctness; clear resets state.
  - `OrderFactory`: create correct subtype (`DeliveryOrder` or `PickupOrder`) and ensure fields populated.
  - `Order::processPayment`: verify paymentStrategy invoked (use mock strategy).
  - `TomatoApp` high‑level flow: use fakes for managers/services to assert orchestration flows.

Security and correctness notes
- PaymentStrategy currently prints payments; in real systems payment processing requires secure handling of secrets (card numbers), PCI compliance, encryption in transit and at rest, and proper error handling/rollback.
- Input validation: menu item codes and addresses must be validated and sanitized.

Suggested immediate improvements (quick wins)
1. Add/implement `OrderManager.h` (singleton) or refactor to injected `OrderRepository`.
2. Replace raw pointers with smart pointers (unique_ptr/shared_ptr) to establish clear ownership and avoid leaks.
3. Remove unnecessary `new NotificationService()` — use static method or make it non‑static and inject it.
4. Add thread‑safety guards if moving to multithreaded server (mutex around manager collections).
5. Create unit tests for factory behaviour, payment flow, and cart management.

Interview Q&A / Talking points (short answers)
- Why Factory? To encapsulate object creation that depends on runtime parameters (order type). This isolates creation logic and follows OCP for adding new order creation variants.
- Why Strategy? To support multiple payment methods without changing `Order` or `TomatoApp`. Payment behaviour can be swapped at runtime.
- Why Singleton for managers? Convenience for a single in‑process registry. Tradeoff: global state and testability; prefer DI for production systems.
- How to make the design more testable? Remove singletons, inject dependencies, and use interfaces for persistence and external services.
- How to scale search? Add an index, sharded DB, or external search service (Elasticsearch) and cache hot queries.

How to run (locally)
- Quick compile (note: `OrderManager.h` is missing and will block compilation until implemented):

```bash
g++ -std=c++17 main.cpp -o tomato
./tomato
```

If `OrderManager.h` is missing, either implement a minimal `OrderManager` singleton or temporarily comment out references to `OrderManager::getInstance()` in `TomatoApp.h` to compile the example flow for demonstration.

Followups I can do for you
- Implement `OrderManager.h` with storage and basic lifetime management.
- Replace raw pointers with smart pointers and update owners/consumers.
- Add unit tests (Catch2 or GoogleTest) for Cart, Factory, and PaymentStrategy.
- Add a simple persistence adapter (file or in‑memory) and an `OrderRepository` abstraction.

---

Notes
- This cheat sheet is intentionally concise but covers the essentials you'd want to quickly communicate in an LLD interview. Use the UML screenshot (`UML.png`) while explaining structural relationships — point out factories at the bottom left, the strategy tree on the right, and the orchestrator at the top.

Prepared by: workspace assistant — created on 2026‑08‑19
