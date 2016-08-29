BEJOCAMA - A C++ functional extension library
=============================================

For the Impatient Reader
------------------------

The main goal of this activity is to provide a *tool*, that helps to
obtain a linear coding style. The linearity is often broken by

 * if statements
 * loops
 * parallelism

Even if that *tool* has a functional character, the idea is not to make
C++ to be a functional language.

Status
------

 * a compose function that composes at any argument position
 * a variadic composer which composes at position 0
 * a curry functions that is variadic over positions

 Because this approach is composition oriented, restrictions are
 not on the threoretical side - if any - not approved yet.

Preface
-------

The intention of this library is to develop a coding pattern, which
implementation results in a function - the *composer*:

```cpp
template<typename T>
maybe<io> composer(fopen,fstat,ftruncate<T>,mmap<T>,fcopy<T>,fclose)
				  (string filename, size_t size, T* data);
```

or

```cpp
template<typename T>
maybe<bool> composer(fopen,make_file<T>,&file<T>::make_list,print<T>)
                    (string file);
```

This way the motivation for a functional solution is derived from a coding
problem inside the C++ language. 

These two introductory c-file-function examples may show, that the use of
the *composer* function can be simple in use, without knowing anything
about *maybe* or the *list monad*. But to extend such a library, a deeper
look inside is necessary.

The file method *make_list* in the second example doesn't mean to fill up
a list with the content of a file. It means, that a file can be the provider
of a list interface. To work with values and not with pointers,
*value semantics* play well together with functional composition, expressed
in haskell:

```haskell
data list :: std::list  | std::map | bejocama::file
```

The interpretation for C++ is

> *list* is a *primary type* whereas the right side consists of
> *secondary types* or *internal types*.

Next plans are to extend the list operations and the list of list-providers
and a first implementation of continuations. To get a better feeling how powerful
this approach is, a database for filesystem, memory and shared memory use,
may be a playground.


Introduction
------------

C++ privides today many functional elements - the *template system* combined with
*algorithms* and *traits*. The benefits of these functional elements are

  * compact code
  * reuse of well tested code

In existing code, the use of lambdas, help to write small adapters to these
algorithms. Behind these concepts we find coding patterns. Most the time we
are working with containers and the related coding pattern is
*looping over containers*. To go straight forward, let's analyze another
pattern.

```cpp
template<typename F, typename G, typename T>
decltype(auto) calculate(const F& f, const G& g, T t)
{
	auto x = f(t);

	return g(x);
}
```

This is a very clear situation. To write the code in two or one line
isn't very important. We are working with an imperative language, so why
not use the possibility to make something clear by writing two lines.

But we are working often with already existing code and the situation looks
more like this:

```cpp
template<typename F, typename G, typename T>
decltype(auto) calculate(const F& f, const G& g, T t)
{
	auto x = f(t);

	/*
		interpretation of x and extracting a value y.
	*/
	...

	return g(y);
}
```

The dots represent perhaps many lines of code and often very difficult code.
That's the area of the intensive use of *if* statements. Let's express this
situation by an unknown function *transform*:

```cpp
template<typename F, typename G, typename T>
decltype(auto) calculate(const F& f, const G& g, T t)
{
	auto x = f(t);

	auto y = transform(x);

	if (y) {

	   return g(y);
	}

	throw runtime_error("an error occurred");
}
```

Before we can take the value *y* as input parameter for *g*, it's necessary
to check, if the transformation was successful. If not, an exception is
thrown. To make the transformation step more powerful, and we think of *y*
as a container, perhaps a list, and *g* as a function on a container element,
we can write the code in a more general way:

```cpp
template<typename F, typename G, typename T>
decltype(auto) calculate(const F& f, const G& g, T t)
{
	auto z = combine(f,g,transform,t);

	if (z) {

	   return z;
	}

	throw runtime_error("an error occurred");
}
```

So, what we have in general:

  * a function *f* with return type
  * a function *g* with input type
  * a transformation, which generates a argument for *g*
    from the result of *f*

Now, we have a problem with the *transformation*, which cannot be generalized.
But we can ask

> **For which situations is it possible to generalize the transformation.**

An approach could be to make the *transormation* within the *combine* function
and define the *combine* function as

```cpp
template<typename F,typename G> typename result_of<G>::type combine(F&& f, G&& g); 
```

Now it's possible to write

```cpp
template<typename F, typename G, typename T>
decltype(auto) calculate(const F& f, const G& g, T t)
{
	auto z = combine(f,g,t);

	if (z) {

	   return z;
	}

	throw runtime_error("an error occurred");
}
```

For each *return type* - *argument_type* combination it's possible to provide a
generic *combine* function, which can be aligned to special situations by
template specialization.

The next improvment is to wrap the *combine* function into a *compose* function.
The result of *compose* is a function and not a value:

```cpp
template<typename F, typename G, typename T>
decltype(auto) calculate(const F& f, const G& g, T t)
{
	auto z = compose(f,g)(t);

	if (z) {

	   return z;
	}

	throw runtime_error("an error occurred");
}
```

And the last step, which is delivered by this library is the *composer*:

```cpp
template<typename... F, tamplate... A>
decltype(auto) calculate(const F...& f, T... t)
{
	auto z = composer(f...)(t...);

	if (z) {

	   return z;
	}

	throw runtime_error("an error occurred");
}
```
The *composer* allows the composition of a variadic list of functions.


References
----------

> ### Additional information -- still in progress ###

> <http://www.bejocama.de>
