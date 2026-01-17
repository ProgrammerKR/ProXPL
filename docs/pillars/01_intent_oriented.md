# Pillar 1: Intent-Oriented Programming

## Concept
Intent-Oriented Programming (IOP) shifts focus from *how* to execute a task to *what* the outcome should be. It allows developers to define `intent` interfaces that express a desired capability, and `resolver` implementations that fulfill those intents.

## Syntax

### Intent Declaration
```javascript
intent PaymentProcessor(amount, currency) -> bool;
```
Defines an abstract operation `PaymentProcessor` that takes an amount and currency, returning a boolean success status.

### Resolver Implementation
```javascript
resolver StripePayment for PaymentProcessor {
    func resolve(amount, currency) {
        // Implementation for Stripe
        return true;
    }
}
```
Provides a concrete implementation for the intent.

## Usage
The runtime (or compile-time logic) selects the appropriate resolver based on context, configuration, or availability.
