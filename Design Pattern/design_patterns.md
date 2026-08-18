
strategy design:
strategy is about separating a family of algorithms (behaviours) behind a common interface — but refine the description and correct terminology.

- Relationship: strategy uses composition (a "has‑a" relationship) to hold a behaviour implementation. The client delegates to the strategy/behaviour via an interface.
- How it works: define a Strategy interface (abstract class or interface) that declares the operation (for example `pay()` for payments). Implement concrete behaviour classes (e.g., `CreditCardPayment`, `UPIPayment`, `WalletPayment`) that implement that interface. The context (a class that needs the behaviour) holds a reference to the Strategy and calls the operation; at runtime the concrete behaviour bound to the strategy reference executes.
- Why use it: you can change the behaviour at runtime, add new behaviours without changing the context, and keep the context class simple and focused.
- Example (this workspace): see `Design Pattern/Strategy Design Pattern/StrategyDesignPattern.cpp` for an implementation sketch showing a payment strategy and concrete payment methods.


factory design pattern:
a Factory encapsulates object creation, letting you decide which concrete class to instantiate from one place.

- Relationship: Factory is not an inheritance relationship — it is a creational helper that returns appropriate concrete objects through a common interface or base type.
- How it works: implement a Factory class (or static factory function) that accepts parameters or keys and returns an object typed to a common interface. Internally the factory chooses the concrete implementation (if/else, switch, map, or registration table) and returns it to the caller. This centralises construction logic and hides instantiation details.
- Variants: Simple Factory (static method), Factory Method (subclass decides which concrete product to create), Abstract Factory (family of related products). See `Design Pattern/Factory Design Pattern/SimpleFactory.cpp`, `FactoryMethod.cpp`, and `AbstractFactory.cpp`.


singleton design pattern:
- Definition: a Singleton ensures only one instance of a class exists and provides global access to it.
- Use cases: logging manager, shared configuration, database connection pool manager (but prefer connection pools over a single global DB client in multi‑threaded environments).
- Implementation notes and cautions:
	- In single‑threaded programs a simple static instance suffices; in multi‑threaded programs you must ensure a thread‑safe initialization (e.g., C++11 magic statics or double‑checked locking with proper memory barriers).
	- Singletons introduce global state, which increases coupling and makes unit testing harder. Consider dependency injection instead of singletons for testability.
	- Prefer explicitly injected, scoped, or factory‑controlled single instances (e.g., create one instance in a composition root and pass it to dependents) rather than sprinkling global singletons.
- Example (this workspace): see `Design Pattern/Singleton Design Pattern/SimpleSingleton.cpp`, `EagerLock.cpp`, `Locking.cpp`, `DoubleLocking.cpp` for different singleton approaches and locking strategies.


---



