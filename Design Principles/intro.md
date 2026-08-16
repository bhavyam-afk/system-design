# SOLID Principles — Detailed Guide

This document explains the five SOLID object‑oriented design principles with concrete explanations, rules, and references to the example code in this workspace. Use the example files in `Design Principles/` (e.g. `SRP.cpp`, `OCP.cpp`, `LSP.cpp`, `ISP.cpp`, `DIP.cpp`) as companion code to experiment with the rules.

## High level: has‑a vs is‑a

- has‑a (composition / aggregation): an object contains or owns another object. Example: a `BankClient` has a collection of accounts (see `LSP.cpp`, class `BankClient`).
- is‑a (inheritance / subtype): a class is a specialized version of another class (subclassing). Example: `SavingAccount` is‑a `WithdrawableAccount` in `LSP.cpp`.

Remember: composition (has‑a) is often preferred for flexible designs; inheritance (is‑a) brings subtype contracts and therefore requires careful adherence to LSP.

---

## Single Responsibility Principle (SRP)

Definition
- A class should have only one reason to change. It should do one thing and do it well.

Why it matters
- Smaller, focused classes are easier to test, maintain, and evolve. When responsibilities are separated, changes in one concern do not ripple unexpectedly into others.

Example (in this workspace)
- See `SRP.cpp`: `ShoppingCart` handles cart business logic (adding items, computing total), `ShoppingCartPrinter` handles printing, and `ShoppingCartStorage` handles persistence. Each class has a single reason to change.

Guidelines
- Identify responsibilities and separate them: UI, business rules, persistence, logging, and formatting usually deserve separate components.
- Prefer composition to share functionality between responsibilities (e.g., a Printer receives a Cart instance).

---

## Open/Closed Principle (OCP)

Definition
- Software entities (classes, modules, functions) should be open for extension but closed for modification.

Why it matters
- Allows behaviour to grow without changing tested and working code. New functionality is added by extending (subclassing or composition) rather than modifying existing code.

Example
- See `OCP.cpp`: `Persistence` is an abstract base and concrete `SQLPersistence`, `MongoPersistence`, `FilePersistence` extend it. The high‑level code uses the `Persistence` abstraction and can work with new persistence implementations without changing existing logic.

Guidelines
- Program to interfaces/abstractions. Use dependency injection so new implementations can be plugged in.
- When you find yourself editing an existing class frequently to add behaviours, extract an abstraction and extend it instead.

---

## Liskov Substitution Principle (LSP)

Definition (short)
- Objects of a superclass should be replaceable with objects of a subclass without breaking the correctness of the program.

Essence: "A subtype must be substitutable for its base type."

Common phrasing you might hear
- "Child should never narrow the scope of the parent." That means a subclass must not introduce stronger preconditions, weaker postconditions, or break invariants expected by clients of the base class.

Has‑a vs Is‑a relevance
- If a subclass cannot satisfy the base contract, prefer composition (has‑a) over inheritance (is‑a). For example, `FixedTermAccount` in `LSP.cpp` is a `DepositOnlyAccount` (it does not implement withdraw) — it should not be substituted where a `WithdrawableAccount` is expected.

Detailed LSP Rules and Guidelines

1) Signature rules
- Method argument rule (precondition / contravariance): A subtype must accept the same arguments the base type accepts. Concretely, the overriding method must not require more specific or stricter argument conditions than the base method. In other words, do not strengthen preconditions in the child.
- Return type rule (covariance): The return type of an overriding method should be compatible with the base method's return type. Many languages allow covariant return types (return a subtype), which is safe because callers expect the base type and will accept a subtype.
- Exception rule: Overriding methods should not introduce new checked exceptions (or, more generally, exceptions that callers of the base class are not prepared to handle). Throwing broader or unexpected exceptions from a subtype can break callers.

2) Property rules
- Class invariant: Any invariant guaranteed by the base class must still hold for the subclass. A subclass may add invariants but they must not violate base invariants when observed through base type operations.
- History constraint: Subtypes must preserve the permissible state transitions of base types. You cannot make previously valid state changes invalid in the subtype if those changes are part of the base contract.

3) Method rules (pre/post conditions)
- Precondition rule: A subtype must not strengthen preconditions (it can only keep them the same or weaken them). If the base method allowed a broader set of inputs, the subtype must also accept them.
- Postcondition rule: A subtype must not weaken postconditions (it must ensure at least the guarantees the base provided — it may strengthen them).

Practical checklist to follow LSP
- Review method contracts: what callers expect (inputs, outputs, side effects, exceptions). Ensure overrides honor those expectations.
- Avoid overriding methods to refuse operations that the base allowed. If a derived class cannot support a base operation, do not inherit — prefer composition or split the interface into smaller pieces (see ISP).
- Use unit tests that operate against base type interfaces; ensure all subclasses pass those tests.

Example from this repo
- `LSP.cpp` contains `DepositOnlyAccount`, `WithdrawableAccount`, `FixedTermAccount`, `SavingAccount`, and `CurrentAccount`. A `BankClient` constructs collections of `WithdrawableAccount*` and `DepositOnlyAccount*`. The design follows LSP: `FixedTermAccount` is not placed in the `withdrawableAccounts` collection because it doesn't support withdraw — instead it only implements `DepositOnlyAccount`.
- If `FixedTermAccount` incorrectly inherited `WithdrawableAccount` and threw on withdraw, client code that expects to call `withdraw` would break — a classic LSP violation.

Corner cases and language notes
- C++ allows different behaviours for exceptions (there's no checked exception mechanism as in Java), but the conceptual rules still apply: do not change the observable contract of a method in a way clients will not expect.
- Variance rules differ across languages; be mindful of your language's overriding/variance guarantees.

---

## Interface Segregation Principle (ISP)

Definition
- Many client‑specific interfaces are better than one general purpose interface. Clients should not be forced to depend on interfaces they do not use.

Why it matters
- Large, fat interfaces force implementers to provide useless methods and lead to brittle code. Small, focused interfaces make implementations and tests simpler.

Example
- See `ISP.cpp`: we separate `TwoDimensionalShape` and `ThreeDimensionalShape`. `Square` and `Rectangle` implement only the 2D shape interface, while `Cube` implements the 3D interface. A 2D shape implementation is not forced to provide `volume()`.

Guidelines
- Split interfaces by client needs. If a method isn't required by all implementers, move it to a smaller specialized interface.
- Use composition to combine interfaces for classes that need multiple behaviours.

---

## Dependency Inversion Principle (DIP)

Definition
- High‑level modules should not depend on low‑level modules. Both should depend on abstractions. Abstractions should not depend on details; details should depend on abstractions.

Why it matters
- Reduces coupling between concrete implementations and business logic. Makes code easier to test and swap implementations.

Example
- See `DIP.cpp`: `Database` is an abstraction, `MySQLDatabase` and `MongoDBDatabase` are low‑level modules that implement it, and `UserService` is a high‑level module that depends on the `Database` abstraction via constructor injection. `UserService` can be used with any `Database` implementation without changes.

Guidelines
- Depend on interfaces or abstract base classes, not concrete classes.
- Inject dependencies (constructor, setter, or interface injection) rather than creating them internally.
- Keep abstractions small and meaningful.

---

## References & next steps

- Example files: `SRP.cpp`, `OCP.cpp`, `LSP.cpp`, `ISP.cpp`, `DIP.cpp` (under `Design Principles/`). Walk through them, run and modify examples to see how violations manifest.
- To check LSP compliance in your codebase: write behavioural tests that call base type methods and assert expected behaviour; ensure all subclasses pass those tests.

If you'd like, I can:
- run or compile the examples and report any runtime output/errors;
- add small unit tests that validate LSP behaviours for the example classes;
- or split any specific interface in the repository that you suspect violates SOLID.

---

Author: Workspace assistant — added detailed SOLID documentation and LSP rules with examples.
