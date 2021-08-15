# Value Design and Planning

## Numbers: Complex

This should be a struct type `Complex`. It would have two fields: the real and imaginary coefficients.`

These could be unions of ints and floats.

Start: pure complex math.
- Add
- Subtract
- Multiply
- Divide

Then: be able to do ints/doubles.
- This could be hard, especially for operations that don't commute. It makes sense to have something like `complexSubtract(one, two)` which would implement something like `one - two`. But because this is C, `one` and `two` are strongly typed.
  - I could do something with function overloading (I think that's a thing in C..). I could have `complexSubtract(int one, Complex two)` and `complexSubtract(Complex one, int two)`.
  - This could get out of hand very quickly. For just integer-complex math, it would be 8 separate functions (4 operations, 2 orders each). Multiply by 2 for doubles, giving 16. Or by 3 to add fractions too (the end goal) and there would be 24 functions just to implement basic operations.
- Alternatively, I could use the fact that every real number is just a complex number with 0 complex component. It wouldn't help with C's strong typing, but I could convert every real to a `Complex` and then just pass it in to the standard functions.
  - This isn't necessarily mutually exclusive. If I use the first method, the `complexSubtract` could create a `Complex` struct of the proper type and pass it along.
- This would get even more fun with fractions, since complex coefficients can be fraction types. So `(+ 1/4 1+2i) = 5/4+2i`.