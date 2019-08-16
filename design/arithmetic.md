# Design for Arithmetic System

The current method of just doing 

I think I should create a completely separate file just for arithmetic. 

I can start this implementation with just the existing numeric types (C integers and doubles), then add more types. 

I will also have to deal with scientific notation at some point. I need to figure out what its representation is (can it be precise? Is it its own type?) and whether its format is preserved. 

## Type Conversions
This number system would deal with type conversions as a part of performing operations. It would take in generic `Values` (of number type, which it must check, perhaps as an assertion) and perform the necessary logic until the operation can be performed. This would save much of the syntactic simplicity of C’a implicit typecasting system for arithmetic, since I could simple call an `add` function and get the desired result instead of going through the whole process of dealing with types every time I do an arithmetic operation. This process is the same in every operation. (This means it would also improve code reuse, since the conversion logic could be reused among all arithmetic operations.)
I would go by a cascading method. It goes by precision (etc) in a well defined order, based on the two values being operated upon. 
The first level is an int. If it’s operated with a double, it is cast to a double and the the operation is performed with two doubles, returning a double. (This is an example. I don’t know if I want to do explicit type casting or leave that to C’s implicit arithmetic system) An int and a fraction casts the int to a fraction and returns a fraction, since an integer is a subset of fractions. A fraction and a double returns a double, since the double is imprecise. 
* Casting pairs
	* Integer takes the type of whatever else
		* Integer, complex becomes complex
		* Integer, float becomes float
		* Integer, fraction becomes fraction
	* Float
		* Given precise numbers, it becomes float
			* Float, integer becomes float
			* Float, fraction becomes float
		* Given complex, it becomes complex
	* Complex
		* Everything becomes complex
			* Integer, complex becomes complex
			* Double, complex becomes complex
			* Fraction, complex becomes complex
		* The real component takes the type of the incoming number
	* Fraction 
		* Integer, fraction becomes fraction
		* Float, fraction becomes float
		* Complex, fraction becomes complex
	* So. 
		* Complex takes priority and can accept any type
		* Imprecise takes next priority, and this could be a stage in its own
		* Precise has two flows, preserving or casting to fraction. 
* If both numbers are of the same type, I can ignore this process altogether!

This gets a little more complicated when operations can cast (back?) up. For example, operating on two complex numbers so their imaginary portions cancel returns a real type of some sort (say, an integer). So `(+ 1+2i 1-2i) => 2`. 
* However, not everything can do this. An imprecise number can _never_ be implicitly cast back up to a precise number. `(- 1.5 .5) => 1.0`. 
	* Possible up-casts:
		* Complex that cancel the imaginary component become the type of the real component
		* Fractions that cancel the denominator become integers
			- [ ] Can you have complex fractions or can you only have fractions as part of complex numbers?

## The Actual Operations
After performing the type matching process, I can then check what type the numbers are and perform the operation. C types would use the built-in arithmetic and struct types would use typed function calls. This part should be very simple, essentially a switch statement on the type of the numbers and a single function call in each. 

With up-casting implemented it could get more complex. However, I should be able to check this at the end of the entire process. Each type that can be up-cast (complex and fraction, which will conveniently already have a bunch of logic and functions associated with them anyway) could have functions to perform up-casting, if applicable (presumably return the input unmodified if not). This should actually be fairly simple, since I only see two paths so far: `a+0i` and `a/1`. 

For the purposes of the interpreter functions like `+` and `-` that take many arguments this should all be transparent. I would call the proper functions in turn on each subsequent pair, building an answer as I go the same way they already do. 