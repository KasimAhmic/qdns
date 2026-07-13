---

## applyTo: "**/*.{c,cc,cpp,cxx,h,hh,hpp,hxx,ipp,tpp}"

# qDNS C++ Code Review Instructions

Perform a detailed, correctness-first review of every C and C++ change in this pull request.

qDNS is a C++23 recursive DNS resolver built with CMake. Assume that it processes untrusted network packets and configuration data. Correctness, memory safety, protocol compliance, predictable resource usage, and operational reliability are more important than cleverness or minor reductions in code size.

Do not limit the review to the changed lines when surrounding code is necessary to understand ownership, lifetime, control flow, synchronization, or API contracts.

## Review priorities

Review findings in this order:

1. Memory safety and undefined behavior
2. Security and untrusted-input handling
3. Functional correctness and control flow
4. Concurrency and asynchronous lifetime safety
5. Resource management and failure handling
6. DNS protocol correctness
7. Performance and scalability
8. API design, maintainability, and readability
9. Test coverage

Prioritize real defects and meaningful risks. Do not overwhelm the review with stylistic preferences, speculative concerns, or issues already enforced by clang-format, compiler warnings, or automated tooling.

## Memory safety and object lifetime

Closely inspect all ownership and lifetime relationships.

Look for:

* Use-after-free and use-after-scope
* Double-free and invalid-free defects
* Memory leaks
* Dangling pointers, references, iterators, views, spans, and callbacks
* References or pointers to temporary objects
* `std::string_view` or `std::span` outliving the referenced storage
* Lambda captures that outlive captured local variables
* Asynchronous callbacks capturing `this` after the object may be destroyed
* Invalidated iterators, references, and pointers after container mutation
* Access beyond array, buffer, string, span, or packet boundaries
* Incorrect pointer arithmetic
* Reads from uninitialized memory
* Returning references or pointers to local variables
* Incorrect move operations or use of moved-from objects
* Object slicing
* Incorrect destruction through a base pointer
* Missing virtual destructors on polymorphic base classes
* Incorrect member initialization order
* Static initialization order problems
* Destruction-order dependencies
* Placement-new, union, aliasing, alignment, and object-lifetime violations
* Undefined behavior caused by signed overflow, invalid shifts, invalid casts, or data races

Trace ownership across function calls rather than judging isolated expressions.

Flag ambiguous raw owning pointers. Raw pointers and references are acceptable for non-owning access when their lifetime requirements are clear.

Prefer:

* Automatic storage duration
* RAII
* Value semantics
* Standard containers
* `std::unique_ptr` for exclusive dynamic ownership
* `std::shared_ptr` only when shared lifetime is genuinely required
* `std::weak_ptr` for breaking ownership cycles or observing asynchronous objects
* `std::span` for bounded non-owning contiguous data
* Explicit lifetime and ownership contracts

Do not recommend smart pointers merely because a raw pointer exists. Determine whether the pointer owns, observes, or optionally references an object first.

## Resource management

Check every acquired resource for deterministic and exception-safe cleanup.

Resources include:

* Heap allocations
* Files and file descriptors
* Sockets
* Timers
* Threads
* Mutexes and locks
* Event-loop registrations
* Asynchronous operations
* Operating-system handles
* Temporary files
* Third-party library resources

Look for:

* Resources leaked on early returns or exceptions
* Manual cleanup that should be represented by an RAII type
* Threads that can remain joinable during destruction
* Sockets, timers, or callbacks that remain active after their owner is destroyed
* Cleanup operations performed in an unsafe order
* Destructors that may throw
* Partial initialization that leaves an object in an unsafe state
* Ownership cycles involving callbacks or `std::shared_ptr`

Verify exception safety where exceptions are enabled. Pay particular attention to mutations performed before an operation that may throw.

## Untrusted input and security

Treat all DNS packets, command-line arguments, configuration files, file contents, environment values, and network responses as untrusted.

Look for:

* Missing bounds checks
* Length fields trusted before validating available input
* Integer overflow or underflow in size and offset calculations
* Narrowing conversions
* Signed-versus-unsigned comparison errors
* Truncation when converting between integer types
* Allocation sizes controlled by untrusted input
* Unbounded loops, recursion, retries, queues, caches, or response growth
* Malformed input causing crashes, excessive CPU usage, or excessive memory usage
* Exceptions escaping network-processing boundaries unexpectedly
* Unsafe assumptions about null termination, encoding, alignment, or byte order
* Logging of secrets or unnecessarily sensitive information
* Log injection through unsanitized attacker-controlled strings
* Predictable or insecure randomness where security-sensitive randomness is required
* Path traversal or unsafe filesystem operations
* Time-of-check/time-of-use filesystem races
* Failure to validate enum values before casting or using them

When reviewing parsers, manually follow each offset and length calculation. Confirm that validation occurs before every read, copy, cast, allocation, and pointer advancement.

## DNS protocol concerns

For DNS-related changes, inspect behavior against the applicable RFC requirements and existing project conventions.

Pay special attention to:

* DNS header field parsing and serialization
* Network byte order
* Truncated packets
* Malformed labels and names
* Label and total-name length limits
* Compression pointers
* Compression-pointer loops
* Excessive compression indirection
* Pointer offsets outside the DNS message
* Recursive parsing without a strict depth or work limit
* Question and resource-record count validation
* Section-boundary validation
* Resource-record length validation
* Unknown record types and classes
* EDNS and extended response codes
* UDP size limitations and truncation behavior
* TCP message framing
* Transaction ID matching
* Query and response association
* Bailiwick and delegation handling
* CNAME and referral loops
* Resolver recursion limits
* Retry, timeout, and fallback behavior
* Cache TTL calculations and expiration
* Negative caching
* Integer overflow in time or TTL calculations
* Case-insensitive DNS-name comparison
* Canonicalization that changes protocol meaning
* Amplification or resource-exhaustion risks

Do not claim RFC noncompliance without identifying the relevant requirement or clearly explaining the protocol invariant being violated.

## Concurrency and asynchronous code

Assume that resolver operations may overlap and that callbacks may run after the initiating function returns.

Look for:

* Data races
* Unsynchronized shared mutable state
* Deadlocks and inconsistent lock ordering
* Locking the wrong mutex
* Access outside the protected critical section
* Calling external or user-provided code while holding a lock
* Blocking operations on event-loop or I/O threads
* Lost wakeups
* Incorrect condition-variable predicates
* Atomic operations with incorrect semantics
* Check-then-act races
* Cancellation races
* Timer and callback lifetime races
* Callbacks invoked more than once
* Completion handlers never invoked on some paths
* Promises or futures left unresolved
* Concurrent container access without synchronization
* Shutdown races
* Detached threads
* Capturing stack variables by reference in asynchronous work
* Capturing raw `this` where object lifetime is not guaranteed
* Holding locks across suspension points or asynchronous boundaries

Verify that shutdown is deterministic and that outstanding work cannot access destroyed state.

## Functional correctness and control flow

Trace all important paths, including failure paths.

Look for:

* Incorrect conditions
* Reversed comparisons
* Off-by-one errors
* Missing or unintended fallthrough
* Incorrect loop termination
* Infinite loops
* Recursion without a valid base case or depth limit
* Early returns that skip required work or cleanup
* Errors that are swallowed or converted into success
* Stale state retained after failure
* Partial updates that violate invariants
* Incorrect default values
* Invalid state transitions
* Functions whose names or comments do not match their behavior
* Boolean expressions that do not encode the intended policy
* Incorrect assumptions about library return values
* Failure to distinguish absence, malformed input, and valid zero or empty values
* Error messages or logs that describe the wrong operation
* Exceptions caught too broadly or at the wrong layer
* Catching exceptions by value
* Missing handling for exceptional or uncommon enum values

When a branch appears unreachable, verify the claim rather than assuming it.

## Numeric correctness and conversions

Review every conversion involving sizes, offsets, counts, ports, timestamps, TTLs, and protocol fields.

Look for:

* Implicit narrowing
* Lossy casts
* Signed/unsigned conversion problems
* Values validated only after narrowing
* Overflow before conversion to a wider type
* Multiplication or addition overflow in allocation and buffer-size calculations
* Negative values converted to large unsigned values
* Incorrect use of `std::numeric_limits<T>::min()` for floating-point lower bounds
* Floating-point comparisons used where integral protocol values are expected
* Invalid casts to enum types
* Assumptions about enum underlying values
* Incorrect units or unit conversions

Prefer validating in a sufficiently wide type before converting to the destination type.

## Error handling and observability

Ensure failures are handled at the correct abstraction boundary.

Look for:

* Ignored return values
* Lost error codes
* Exceptions that unexpectedly cross thread, callback, or library boundaries
* Generic errors that hide actionable context
* Duplicate logging at multiple layers
* Logging and then throwing when an outer layer will log the same failure
* Successful-looking log messages after a failed operation
* Missing context such as operation, endpoint, file, property, or error code
* Inconsistent log levels
* Expensive formatting or computation performed for disabled log levels
* Attacker-controlled values used as format strings
* Logging of entire untrusted packets or oversized values
* Failure paths that silently use unsafe defaults

For structured logs, prefer stable event names and consistently named fields. Logs should support diagnosis without exposing secrets or producing excessive volume.

## Performance and scalability

Identify meaningful performance issues, especially those that affect every DNS query or can be triggered remotely.

Look for:

* Unnecessary heap allocation in hot paths
* Repeated parsing, serialization, normalization, or lookup
* Accidental copies of strings, buffers, containers, packets, or resource records
* Passing large objects by value without a reason
* Missing move opportunities only when they materially matter
* Quadratic or worse algorithms
* Repeated linear searches where indexing is appropriate
* Excessive lock contention
* Global locks in request-processing paths
* Blocking filesystem, DNS, or network calls on asynchronous I/O threads
* Busy waiting
* Unbounded queues or caches
* Cache structures without eviction
* Repeated construction of expensive objects
* Excessive `std::shared_ptr` reference-count traffic
* Logging in high-volume loops
* Exceptions used for ordinary hot-path control flow
* Poor container selection
* Rehashing or reallocating when the required capacity is known
* Excessive timer creation
* Work amplification from malformed or adversarial packets

Do not report trivial micro-optimizations unless the code is demonstrably in a hot path or the change is both clear and materially beneficial.

Correctness and clarity take precedence over speculative optimization.

## API and design quality

Evaluate whether interfaces make invalid usage difficult.

Look for:

* Unclear ownership or lifetime requirements
* Functions with too many responsibilities
* Boolean parameters with unclear meaning
* Excessive coupling
* Global mutable state
* Hidden side effects
* Public APIs exposing unnecessary implementation details
* Invariants not established by constructors
* Objects that can exist in invalid states
* Overuse of inheritance where composition is clearer
* Incorrect `const` usage
* Missing `[[nodiscard]]` where ignoring a result is likely a bug
* Missing `noexcept` only where the non-throwing guarantee is important and supportable
* Unnecessary dynamic allocation
* Inappropriate use of `std::shared_ptr`
* APIs returning references or views with unclear lifetimes
* Duplicate logic likely to diverge
* Abstractions that obscure simple behavior
* Premature generic programming
* Templates that produce poor diagnostics without a meaningful benefit

Respect the existing project architecture unless the current design creates a concrete problem.

## Readability and maintainability

Review whether another engineer can accurately understand and modify the code.

Look for:

* Misleading names
* Excessively long or deeply nested functions
* Dense expressions with hidden side effects
* Repeated complex conditions
* Magic numbers without protocol or domain meaning
* Comments that contradict the implementation
* Comments that merely restate the code
* Missing explanation for non-obvious invariants
* Inconsistent terminology
* Unnecessarily clever code
* Ambiguous precedence
* Control flow that can be simplified without changing behavior
* Broad scopes for variables, locks, or resources
* Public headers containing unnecessary dependencies
* Missing direct includes that rely on transitive includes

Do not request purely cosmetic changes already handled by clang-format. Do not impose a personal style when the existing code is clear and consistent.

## C++ practices

Apply modern C++23 practices pragmatically.

Prefer:

* RAII over manual cleanup
* Scoped enumerations
* Standard-library facilities over custom low-level equivalents
* Explicit ownership
* Narrow scopes
* Direct initialization
* `const` where it communicates and enforces an invariant
* Range-based algorithms when they improve clarity
* Strong types where primitive values are easily confused
* Compile-time constants for true constants
* `std::chrono` types for durations and time points
* `std::byte`, fixed-width integer types, or unsigned-byte types for binary protocol data as appropriate
* Error-return types or exceptions consistently with the surrounding subsystem

Review special member functions using the rule of zero first. When custom copy, move, or destruction behavior exists, verify all affected operations and invariants.

Do not mechanically recommend:

* `std::move`
* `noexcept`
* `constexpr`
* Smart pointers
* Concepts
* Ranges
* Templates
* Exceptions
* Replacing every loop with an algorithm

Recommend these only when they improve correctness, safety, expressiveness, or meaningful performance.

## Build-system and portability review

For CMake and build-related changes, look for:

* Target-global settings that should be target-specific
* Missing `PRIVATE`, `PUBLIC`, or `INTERFACE` visibility
* Reliance on transitive dependencies
* Platform-specific assumptions
* Compiler-specific flags applied without guards
* Hard-coded paths
* Incorrect install or include directories
* Generated files written into the source tree
* Dependencies not associated with the target that uses them
* Build behavior that differs between single-config and multi-config generators
* Debug-only behavior accidentally enabled in release builds
* Architecture assumptions
* Undefined behavior that appears to work only with one compiler
* Missing warning flags or sanitizer compatibility where relevant

The project should remain buildable on supported Windows, Linux, and macOS toolchains unless the changed code is intentionally platform-specific.

## Testing expectations

Check whether tests exercise both the successful behavior and important failure modes.

Request tests when the change affects:

* Packet parsing or serialization
* Length and offset validation
* DNS name compression
* Caching and TTL behavior
* Retry, timeout, or cancellation behavior
* Configuration parsing
* Error handling
* State transitions
* Concurrency
* Ownership or lifetime-sensitive code
* Security boundaries
* Previously reported bugs

Important negative tests include:

* Empty input
* Truncated input at every meaningful boundary
* Oversized values
* Maximum valid values
* Values just above and below valid boundaries
* Invalid enum values
* Duplicate fields
* Unknown fields
* Compression loops
* Out-of-range offsets
* Integer-overflow attempts
* Repeated cancellation
* Shutdown with outstanding operations
* Callback execution after owner destruction
* Malformed packets designed to maximize work

For parser code, favor table-driven tests and fuzz-test compatibility. Recommend fuzzing when a parser accepts complex attacker-controlled binary input.

Do not demand a test for a trivial mechanical change when existing tests already cover the behavior.

## Review comment requirements

Every review comment must:

* Identify a specific defect or meaningful risk.
* Explain the concrete failure mode.
* State the conditions needed to trigger it.
* Describe the likely consequence.
* Suggest a practical correction when one is reasonably clear.
* Distinguish confirmed defects from uncertain concerns.
* Use an appropriate severity.

Use these severity labels:

* **Critical:** Remotely exploitable vulnerability, memory corruption, arbitrary code execution, catastrophic data corruption, or similarly severe behavior.
* **High:** Likely crash, use-after-free, deadlock, major protocol violation, resource exhaustion, or serious security weakness.
* **Medium:** Functional defect, leak, race with limited impact, incorrect error handling, or substantial performance regression.
* **Low:** Maintainability, readability, localized inefficiency, or defensive improvement with a concrete benefit.

Start each finding with its severity:

`[High] Callback can access the resolver after destruction`

Avoid vague comments such as:

* “This may cause issues.”
* “Consider improving this.”
* “This is not best practice.”
* “Could this be optimized?”
* “Use a smart pointer.”
* “Add more tests.”

Instead, explain the exact issue:

`[High] The completion handler captures raw this, but close() can destroy the Resolver while the asynchronous receive remains outstanding. When the handler later executes, it dereferences a freed object. Keep the operation state alive through the handler or cancel and join all outstanding operations before destruction.`

When proposing code, keep the suggestion narrow and compilable. Do not rewrite unrelated parts of the pull request.

## Review behavior

Before commenting:

1. Understand the intent of the pull request.
2. Inspect the complete changed function or type.
3. Trace callers and callees when necessary.
4. Determine ownership and lifetime.
5. Follow success, error, cancellation, and shutdown paths.
6. Check whether another changed file already addresses the concern.
7. Check existing tests and project conventions.
8. Confirm that the issue is introduced or exposed by the pull request.

Do not invent defects merely to produce review comments.

Do not report an issue when:

* The concern is only hypothetical and no plausible failure path exists.
* The behavior is intentional and documented.
* Existing code or tests clearly establish the required invariant.
* The comment is solely a formatting preference.
* The suggested change would merely replace one valid style with another.
* The issue is unrelated to the pull request and is not made materially worse by it.

If no meaningful issues are found, say that no blocking correctness, safety, or performance problems were identified. Briefly mention any areas that could not be fully verified, such as platform behavior, external API contracts, or missing runtime tests.
