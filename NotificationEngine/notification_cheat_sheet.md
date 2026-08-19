
# Notification Engine — Cheat Sheet

Quick reference for interviews: concise overview, components, flow, and talking points based on `NotificationSystem.cpp` and accompanying diagrams.

## 1. High-level goal
- Build an extensible notification engine supporting multiple channels (Email, SMS, Pop-up), decorator-based message enrichment, observer-based delivery, and centralized management via a singleton service.

## 2. Main components

- `INotification` (abstract)
	- Method: `getContent()`
	- Concrete: `SimpleNotification` — holds plain text message

- Decorator (extends `INotification`)
	- `INotificationDecorator` wraps an `INotification*` and forwards `getContent()`
	- Examples:
		- `TimestampDecorator` — prefixes timestamp
		- `SignatureDecorator` — appends signature/footer

- Observer / Observable
	- `IObserver` (interface): `update()`
	- `IObservable` (interface): `addObserver`, `removeObserver`, `notifyObservers`
	- `NotificationObservable` (concrete): stores observers and the current `INotification*`, notifies all observers when a new notification is set.

- Singleton manager
	- `NotificationService` (singleton)
		- Holds `vector<INotification*> notifications;`
		- Exposes `getObservable()` and `sendNotification(INotification*)`
		- Acts as central entry point for clients

- Observers / Delivery
	- `Logger` — logs new notifications (uses `NotificationObservable->getNotificationContent()`)
	- `NotificationEngine` — observer that holds a list of `INotificationStrategy*` and forwards the notification content to each strategy

- Strategy (delivery channels)
	- `INotificationStrategy` (abstract): `sendNotification(string content)`
	- Implementations: `EmailStrategy`, `SMSStrategy`, `PopUpStrategy`

## 3. Typical flow (sequence)
1. Client constructs a `SimpleNotification` (e.g., "Your order has been shipped!").
2. Optionally wrap with decorators: `new TimestampDecorator(notification)` → `new SignatureDecorator(...)`.
3. Call `NotificationService::getInstance()->sendNotification(notification)`.
	 - `NotificationService` pushes pointer into `notifications` and calls `observable->setNotification(notification)`.
4. `NotificationObservable::setNotification` replaces stored `currentNotification` (deleting previous) and calls `notifyObservers()`.
5. Each `IObserver::update()` is invoked.
	 - `Logger` prints the content.
	 - `NotificationEngine` retrieves content via observable and calls each `INotificationStrategy->sendNotification(content)`.

## 4. Quick code snippets
- Create + decorate + send
```
INotification* n = new SimpleNotification("Your order has been shipped!");
n = new TimestampDecorator(n);
n = new SignatureDecorator(n, "Customer Care");
NotificationService::getInstance()->sendNotification(n);
```

- Add a delivery strategy
```
notificationEngine->addNotificationStrategy(new EmailStrategy("user@example.com"));
notificationEngine->addNotificationStrategy(new SMSStrategy("+1-555-555-5555"));
```

## 5. Ownership, memory & safety notes (important interview points)
- Ownership is manual: raw pointers (`INotification*`) are stored and deleted in `NotificationObservable::setNotification` for the previous `currentNotification` and in its destructor.
- `NotificationService::notifications` stores pointers but is never used to free them — this leads to dangling pointers in the vector (it contains addresses of notifications that may later be deleted by the observable). This is a bug/risk. Talk about safer approaches:
	- Use smart pointers (`unique_ptr` / `shared_ptr`) to make ownership explicit.
	- If keeping a history, store copies or `shared_ptr` and ensure consistent ownership.

- Thread-safety: current implementation is not thread-safe. For multi-threaded or async delivery, add synchronization (mutex) around observer registration, notification dispatch, and shared state.

- Lifetime & double-delete risk: deleting the same raw pointer from multiple places or leaving dangling pointers in `notifications` must be addressed.

## 6. Extensibility & design rationale (how to talk about it)
- Decorator pattern lets you add metadata (timestamp, signature, prioritization) without changing `INotification` implementations.
- Observer pattern decouples producers (service) from consumers (logger, delivery engine) — easy to add/remove observers at runtime.
- Strategy pattern isolates delivery channel implementations; to add a new channel implement `INotificationStrategy` and register it with `NotificationEngine`.
- Singleton `NotificationService` centralizes sending and makes it easy for all parts of app to access the same service instance.

## 7. Interview talking points / improvements to suggest
- Fix ownership: replace raw pointers with `std::unique_ptr` or `std::shared_ptr`.
- Make `NotificationService` and `NotificationObservable` thread-safe: use mutexes and consider lock-free/queue-based dispatch if high throughput.
- Use a message queue or event bus (Kafka/RabbitMQ) for scaling to many recipients and for persistence/retry semantics.
- Add delivery acknowledgements and retry/backoff for unreliable channels (SMS/email providers).
- Persist notifications to a database (append-only log) if audit/history is required — store message, metadata, delivery receipts.
- Improve lifecycle: remove raw `delete` in decorators; prefer RAII and smart pointers.
- Provide unsubscribe / filtering: observers should be able to filter messages (by topic, priority, user id).

## 8. Strengths & trade-offs
- Strengths: clear separation of concerns, easy to extend channels and message enrichment, simple to reason about in small systems.
- Trade-offs: not ready for concurrent production usage; memory/ownership bugs; no persistence, retry, or delivery guarantees.

## 9. One-minute pitch for interviews
"The engine composes messages using the Decorator pattern, then publishes them through a centralized singleton service which notifies registered observers. The delivery engine uses the Strategy pattern to support pluggable channels like Email, SMS, and Popups. This yields a compact, extensible design that cleanly separates message creation, enrichment, and delivery — with obvious next steps being smart-pointer ownership, thread-safety, persistence, and queuing for scale." 

---
File: `NotificationEngine/notification_cheat_sheet.md` — generated from `NotificationSystem.cpp` and the provided diagrams.
