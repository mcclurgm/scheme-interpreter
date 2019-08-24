# Design for Arithmetic System

I can start this implementation with just the existing numeric types (C integers and reals), then add more types.

I will also have to deal with scientific notation at some point. I need to figure out what its representation is (can it be precise? Is it its own type?) and whether its format is preserved.

## Numeric Types

Numeric types follow a hierarchy, in line with mathematics. Scheme implements `integer`, `rational` (fraction), `real` (real), and `complex` types. This list is in order of hierarchy: an `integer` is a type of `rational`, which is a
 * type of `real`, which is a type of `complex`.

* Rationals cannot be complex. Something like `4i/5` is an invalid number in Scheme. (To do something like that, you have to compute `(/ 4i 5)`.)

## Type Conversions
This number system would deal with type conversions as a part of performing operations. It would take in generic `Values` (of number type, which it must check, perhaps as an assertion) and perform the necessary logic until the operation can be performed. This would save much of the syntactic simplicity of C’a implicit typecasting system for arithmetic, since I could simple call an `add` function and get the desired result instead of going through the whole process of dealing with types every time I do an arithmetic operation. This process is the same in every operation. (This means it would also improve code reuse, since the conversion logic could be reused among all arithmetic operations.)
I would go by a cascading method. It goes by precision (etc) in a well defined order, based on the two values being operated upon.
The first level is an integer. If it’s operated with a real, it is cast to a real and the the operation is performed with two reals, returning a real. (This is an example. I don’t know if I want to do explicit type casting or leave that to C’s implicit arithmetic system) An integer and a rational casts the integer to a rational and returns a rational, since an integer is a subset of rationals. A rational and a real returns a real, since the real is imprecise.
* Casting pairs
	* Integer takes the type of whatever else
		* Integer, complex becomes complex
		* Integer, real becomes real
		* Integer, rational becomes rational
	* Rational
		* Integer, rational becomes rational
		* Real, rational becomes real
		* Complex, rational becomes complex
	* Real
		* Given precise numbers, it becomes real
			* Real, integer becomes real
			* Real, rational becomes real
		* Given complex, it becomes complex
	* Complex
		* Everything becomes complex
			* Integer, complex becomes complex
			* Real, complex becomes complex
			* rational, complex becomes complex
		* The real component takes the type of the incoming number
	* So.
		* Complex takes priority and can accept any type
		* Imprecise takes next priority, and this could be a stage in its own
		* Precise has two flows, preserving or casting to rational.
* If both numbers are of the same type, I can ignore this process altogether!

This gets a little more complicated when operations can cast (back?) up. For example, operating on two complex numbers so their imaginary portions cancel returns a real type of some sort (say, an integer). So `(+ 1+2i 1-2i) => 2`.
* However, not everything can do this. An imprecise number can _never_ be implicitly cast back up to a precise number. `(- 1.5 .5) => 1.0`.
	* Possible up-casts:
		* Complex that cancel the imaginary component become the type of the real component
		* Rationals that cancel the denominator become integers

### How to implement this?

- This is going to be a function. C only lets you return a single value from a function. I want to get two values, which have the same type.
  - I could return both values as a `cons` pair. This would be kind of inelegant and a little hard to read. Also a waste of memory and CPU time. But it would get me everything I want, in that I can just call the single `cast` function and get the properly typed values back for immediate use.
  - I could return the resulting type and leave it to the calling function to cast them. This wouldn't be preferable, since it would be a lot of reused (and potentially fragile?) code.
    - I think this may be the way to go. Each other way would also be a little hacky (allocate new `Values` and pass pointers in to be filled, `cons` cell, etc).
    - If I do find a good way, this could also be used there. It doesn't matter what calls this function, so in a sense it's future-proof.
- It makes sense to just call the cast function on both values, and just have the casting function efficiently return the input value if it's already of the correct type.
- I could implement this hierarchy using compiler constants. It may be a little too "clever"/hacky (ie not readable), but it would be easy enough to implement.
  - I could define an `enum` of the numeric types. It would go in order from `integer` to `complex`.
  - If it converts to `complex`, I have to deal with exactness. This can be dealt with after the process of deciding the type, since the exactness of the resulting `complex` doesn't change the fact that the answers will be complex.
    - This could actually be dealt with in the `complex`-typed arithmetic functions. Converting any higher-level number to a `complex` could handle the exactness in the conversion code. Performing the calculation itself would also preserve the exactness.
- I could perform the conversions in the operation functions themselves. For example, the rational `add` function could convert its arguments to rationals.
  - This fits within the current design, where the type of the result is the type of operation that's called. So within that operation, it could simply convert the inputs to the proper types.
  - This doesn't fit with the goal of reducing duplicate code, though. Instead of being in the single primary `add` function, it would have to be in all four types.

## The Actual Operations
After performing the type matching process, I can then check what type the numbers are and perform the operation. The operations would be performed using typed function calls: `intAdd`, `complexAdd`, etc. This part should be very simple, essentially a switch statement on the type of the numbers and a single function call in each.

With up-casting implemented it could get more complex. However, I should be able to check this at the end of the entire process. Each type that can be up-cast (complex and rational, which will conveniently already have a bunch of logic and functions associated with them anyway) could have functions to perform up-casting, if applicable (presumably return the input unmodified if not). This should actually be fairly simple, since I only see two paths so far: `a+0i` and `a/1`.

For the purposes of the interpreter functions like `+` and `-` that take many arguments this should all be transparent. I would call the proper functions in turn on each subsequent pair, building an answer as I go the same way they already do.

## Function Designs

### Basic Arithmetic Operations

Goals:
- I want the implementations of these functions to be independent of what type the inputs are. Essentially, I want to get as close to polymorphism as possible within the restrictions of C. They should have a branching if/else statement at the end that switches based on the casted type of *both* inputs, but before that it shouldn't care. It should also not branch based on the types of the *individual* inputs.

Functions:
- `add`
- `subtract`
- `multiply`
- `divide`
- Accept all types of number: integer, real, rational, complex.
- Take two arguments to apply the operation to
- Return the type of the least exact value. Two exact numbers will yield an exact number.

Implementing the Functions
- I need to pass the values around somehow. I need to know whether to do that as `Values` or as the resulting types. This will affect the design of other functions, since the add functions need to accept the proper type. Their API would be nicer if they took their own type, like `Complex`, instead of a generic `Value`. But it could make using them more difficult.
  - Passing `Values` around would make the code in arithmetic functions like `add` more elegant. I wouldn't have to worry about the types much, and I would never have to pull the value of of the `Value` struct.
    - Method signature: `intAdd(Value *a, Value *b)`
    - Also, if I can find a way to break the conversion process into a function, it would need to return a Value.
    - This might make it possible to use the functions outside of `arithmetic` (although I don't think I want that).
  - It's possible (although probably not necessary) that I could create wrappers around the "elegant" `intAdd` functions that take in `Values`, unwrap them, and then call the actual operation functions that accept only the proper type. Then the type error checking could ensure that the
  - The other option is that I could require the proper type.
    - `intAdd(int a, int b)`
    - It would make the operation functions (`intAdd`) more elegant. They wouldn't have to deal with type checking, because other types couldn't even be passed into them.
	- I also don't want to get `Value` logic into the number functions if I can avoid it at all.
      - One consideration: if I want to support up-casting, then if that's going to be a part of the functions (for encapsulation, I think it should), then I can't guarantee the returning type.
      - Another thing. I probably have to return a `Value` anyway (if I have to put `Value` stuff into these), or else I would have to pack the result into a `Value` in the calling function. Although I'll already by in an if statement to figure out which type, it would be a little less nice.
  - I think the decision is to just pass around `Values`. There are enough advantages that it's probably worth it. And for encapsulation, it doesn't quite make sense to force all the operation functinos to take and return only their own (raw) type.
    - I may do the wrapper functions though. That could add both the advantages, except making the codebase more complicated.

### Helpers

- `cast`
  - Accepts two number type `Values`
  - Casts them to the same type, according to the Scheme type hierarchy, as described above.
  - `get_result_type`
    - If I find a good way of implementing `cast` later, I can still use this code to get the proper type to cast to. It may not be perfect, but it is reasonable and fairly future-proof in terms of adaptibility to other designs.
- Cast up
  - I think I want this to be implemented for all of the types. Even reals, which can't be cast up, and would just return what they're given. Treat it like a Java interface.