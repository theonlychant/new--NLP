# H++ Quick Reference Guide

## Essential Operations

### Creating Identities
```cpp
Identity id("name", "type");              // Named entity
Identity entity("Being", "ontological");  // Ontological entity
Identity concept("Truth", "concept");     // Concept
```

### Identity Chains
```cpp
IdentityChain chain;                      // Create chain
chain.add(Identity("A", "type"));         // Add entity
chain.print();                            // Display analysis
chain.resolve_paradox();                  // Resolve if paradoxical
auto [start, len] = chain.analyze_cycle(); // Cycle analysis
```

### Ontology Graph
```cpp
OntologyGraph graph;
graph.add_entity(Identity("A", "type"));
graph.add_relation("A", "B");             // A → B
auto chain = graph.traverse("A", 100);    // Traverse from A
```

### H++ Engine
```cpp
HPPEngine engine;
engine.register_entity(Identity("X", "t"));
engine.register_relation("X", "Y");
auto result = engine.execute_trace("X", 50);
engine.print_paradox_summary();
```

### Three-Valued Logic
```cpp
using namespace hpp::logic;
hpp_not(TruthValue::TRUE);               // FALSE
hpp_and(TruthValue::TRUE, TruthValue::FALSE); // FALSE
hpp_or(TruthValue::FALSE, TruthValue::TRUE);  // TRUE
hpp_and(TruthValue::TRUE, TruthValue::PARADOX); // PARADOX
```

## State Checking
```cpp
if (chain.get_state() == OntologicalState::PARADOXICAL) {
    switch (chain.get_paradox_type()) {
        case ParadoxType::CIRCULAR:
            // Handle circular reference
            break;
        case ParadoxType::SELF_REFERENCE:
            // Handle self-reference
            break;
        // ...
    }
}
```

## Common Patterns

### Pattern 1: Detect Circular Reference
```cpp
IdentityChain chain;
chain.add(Identity("A", "t"));
chain.add(Identity("B", "t"));
chain.add(Identity("A", "t"));  // Detected!
```

### Pattern 2: Safe Traversal with Depth Limit
```cpp
OntologyGraph graph;
// ... setup graph ...
auto chain = graph.traverse("Start", 1000); // Max 1000 steps
```

### Pattern 3: Paradox Resolution Pipeline
```cpp
auto chain = engine.execute_trace("Entity", 100);
if (chain.get_state() == OntologicalState::PARADOXICAL) {
    chain.resolve_paradox();
    assert(chain.get_state() == OntologicalState::RESOLVED);
}
```

## Build Commands

```bash
make            # Build optimized
make debug      # Build with debug symbols
make run        # Build and execute
make clean      # Remove artifacts
make profile    # Build for profiling
```

## Compiler Flags

```bash
g++ -std=c++17 -Wall -Wextra -O3 -march=native -flto file.cpp
```

## Key Constants

- **Default Max Depth**: 1000
- **Namespace**: `hpp::`
- **Logic Namespace**: `hpp::logic::`

## States & Types

### Ontological States
- `UNDEFINED` - Not defined
- `DEFINED` - Normal
- `PARADOXICAL` - Paradox detected
- `RECURSIVE` - Recursive
- `RESOLVED` - Paradox resolved
- `CONTRADICTORY` - Unresolvable

### Paradox Types
- `NONE` - No paradox
- `SELF_REFERENCE` - A → A
- `CIRCULAR` - A → B → A
- `INFINITE_REGRESS` - Unbounded
- `LIAR` - Liar's paradox
- `SORITES` - Heap paradox
- `RUSSELL` - Set theory

### Truth Values
- `TRUE` - True
- `FALSE` - False
- `PARADOX` - Paradoxical

## Best Practices

1. **Always set max_depth** to prevent infinite loops
2. **Check state** before accessing results
3. **Resolve paradoxes** explicitly
4. **Use const references** for efficiency
5. **Enable optimizations** (-O3 -flto)

## Common Pitfalls

❌ **Don't forget default constructor**
```cpp
struct Identity {
    Identity() : name(""), type("entity") {} // Required!
    // ...
};
```

❌ **Don't ignore paradox state**
```cpp
// BAD
auto chain = graph.traverse("X");
return chain.get_chain(); // Might be paradoxical!

// GOOD
auto chain = graph.traverse("X");
if (chain.get_state() == OntologicalState::PARADOXICAL) {
    chain.resolve_paradox();
}
return chain.get_chain();
```

❌ **Don't use unlimited depth**
```cpp
// BAD
graph.traverse("Start", SIZE_MAX); // Could hang!

// GOOD
graph.traverse("Start", 1000); // Safe limit
```

## Header Include

```cpp
#include "recursive_ontology.hpp"
using namespace hpp;
using namespace hpp::logic;
```

---

*For full documentation, see README.md*
