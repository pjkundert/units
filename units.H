
// 
// units.H		-- Units Management Framework
// 
// Copyright (C) 2004 Enbridge Inc.
//
// This file is part of the UNITS Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.


// 
// units
// 
///     Implements static compile-time checking of units management.  Unless all units are correctly
/// "solved" for by an expression containing units::... values, assignment is not allowed, and
/// conversion to a unitless scalar value is prevented.  There is no run-time overhead for this
/// checking.
/// 
///     My implementation is distributed under the GPL (with the GCC header exceptions), and is
/// based on Christopher Rettig's implementation, who's original implementation header follows:
/// 

/*   file:             units_t.h                                      */
/*   Creation Date:    2000.12.04                                     */
/*   Original Author:  Christopher Rettig ( rettigcd@bigfoot.com )    */
/*                                                                    */
/*   Distribution:                                                    */
/*                                                                    */
/*      This file may be freely distributed and used for any legal    */
/*      purpose provided the line containing "Original Author:"       */
/*      remains unchanged.                                            */
/*                                                                    */
/*   Modifications:                                                   */
/*                                                                    */
/*      This file may be modified in any legal way.  However, the     */
/*      original author requests that enhancements be e-mailed to     */
/*      the address above.                                            */
/*                                                                    */
/*   Purpose:                                                         */
/*                                                                    */
/*      This file uses templates to create a Units data type.         */
/*      It checks that units are correct at COMPILE time.             */
/*      It hides conversion constants.                                */
/*      It enforces self documenting code.                            */
/*                                                                    */
/*   Compilers                                                        */
/*      Known Compliant:  GNU gcc v2.95.2                             */
/*      Known Non-compliant: VC++ v6.0, Borland C++ v5.02             */


#ifndef _INCLUDE_UNITS_H
#define _INCLUDE_UNITS_H

#include <iostream>
#include <iomanip>

#include <math.h>

// 
// namespace units
// 
/// 
///     Units-managing type over typename T, using units::type<T>.  Specific dimensioned basic and
/// derived types are available within that type:
/// 
///         units::type<double>::Length	len;
///         units::type<int>::Matter	mol;
/// 
///     Specific basic and derived units of these dimensions are available by instantiating an
/// instance of the type:
/// 
/// 	    typedef units::type<double>	ud_t;		// Dimensional SI Base and Derived Unit (and unitless) Types
///         ud_t			base;		// SI Base Units values and Multipliers (Kilo, Milli, ...), if instantiated
///         ud_t::imperial		imp( base );	// Imperial units (Gallon, Inch, ...)
///         ud_t::us		 	us( base );	// US units (Gallon (us), Inch, ...)
///	    ud_t::electrical		elec( base );	// SI Electrical units (use with type<T> with real valued T)
///         ud_t::bin		 	bin( base );	// Binary units and Multipliers (K, G, ...)
///         ud_t::other		 	oth( base );	// Others Derived Unit constants, meaningful over real values types
/// 
///     Use them in units-checked equations:
/// 
/// 	    ud_t::Mass			m	= base.Kilogram;
/// 	    ud_t::Acceleration		a	= base.Milli * base.Meter * 10 / base.Second / base.Second;	// 10 mm/s^2
/// 	    ud_t::Force			f	= m * a;
/// 	    double 			n	= f / base.Newtons;		// Solves for all units and scales to multiples of desired constant
/// 

// 
// Forward declarations
// 
namespace units {
    template < int A, int B, int C, int D, int E, int F, int G, typename T>
    class value;

    template < typename T >
    class type;
};

template < int A, int B, int C, int D, int E, int F, int G, typename T >
inline
std::ostream	       	       &operator<<(
			            std::ostream	       &out,
				    const units::value< A, B, C, D, E, F, G, T >
				    			       &val );

namespace units {

    // 
    // units::value<...,T>	-- General units case
    // 
    ///     Dimensional value with non-zero exponents have all valid operators defined, but no
    /// implicit conversion to T.  There is no default units::value<...,T> constructor, but explicit
    /// constuction from a scalar is allowed.
    /// 
    template< int Mass,
	      int Leng,
	      int Time,
	      int Crnt,
	      int Temp,
	      int Matt,
	      int Lumi,
	      typename T = double >
    class value {
	T			_scalar;

    public:
	// 
	// scalar
	// 
	///     Explicit access to scalar value is allowed, but no implicit conversion -- unit's
	/// dimensions are non-zero.
	/// 
	const T		       &scalar()
	    const
	{
	    return _scalar;
	}

	// 
	// Constructor, scalar: units::value<...T,>( t )
	// 
	///     We don't want "implicit" conversion from scalars to Units, but explicit ones are OK
	/// (eg. for the dimension bases).  This avoids the use of reinterpret_cast to access the
	/// scalar portion of Units with non-zero exponents.  Only allow the matching type T; no
	/// automatic conversion from other scalar types (eg. int to double)!
	/// 
	explicit		value(
				    const T	       &t )
				    : _scalar( t )
	{
	    ;
#if defined( DEBUG )
	    std::cout << *this << " (scalar)" << std::endl;
#endif
	}

	// 
	// Constructor, same dimension, convertible value
	// 
	// units::value<...,T>( units::value<...,V> ) 
	// units::value<...,T> = units::value<...,V>
	// 
	///     Allow initialisation/assignment from any unit<V>::value<...> of identical exponents,
	/// as long as the scalar()'s type V is compatible wth T.  Since scalars are only
	/// automatically convertible into the unitless type, this will NOT indirectly allow
	/// automatic construction of complex (NOT unitless) unit<T>::value<...> types from
	/// scalars!
	/// 
	template <typename V>	value(
				    const value< Mass, Leng, Time, Crnt, Temp, Matt, Lumi, V >
				    		       &v )
	    			    : _scalar( T( v.scalar() ))
	{
	    ;
#if defined( DEBUG )
	    std::cout << *this << "   (copy) from: " << std::setw( 13 ) << v.scalar() << std::endl;
#endif
	}

	template <typename V>
	const value	       &operator=(
			            const value< Mass, Leng, Time, Crnt, Temp, Matt, Lumi, V >
				   			&v )
	{
	    this->_scalar		= T( v.scalar() );
#if defined( DEBUG )
	    std::cout << *this << " (assign) from: " << std::setw( 13 ) << v.scalar() << std::endl;
#endif
	    return *this;
	}

	// 
	// / /= * *=
	// 
	///     Scalar multiplication & division into self.  Scalars can be automatically
	/// converted to unitless values of the SAME (see non-template * *=, below).  Unitless
	/// values of DIFFERING types must be explicitly created, if desired, because too many
	/// levels of templating prevents automatic conversion. The standard
	/// multiplcation/division by Scalars COULD BE handled by the general value<...>
	/// multiplication/division operators, below, but there are too many levels of implicit
	/// conversion.  Therefore, we must implement identical operator* and operator/ methods
	/// here for the unitless Units.
	/// 
	///     We implement all scalar/unitless multiplication in terms of the underlying scalar
	/// values directly, if possible, to retain the precision of the underlying scalars as
	/// long as possible.  However, when we finally DO perform the calculation, we CAST THE
	/// RESULT TO T, discarding any type conversion warnings!  It is a good idea, if you are
	/// mixing types in your calculations, to attempt compilation with UNITS_DISABLED, and
	/// confirm that all automatic type conversion warnings are accounted for.  We do this, to
	/// allow for using real-valued scalar multiples in the <unit> constant definitions,
	/// without warnings.
	/// 
	const value 	       &operator*=(
			            const value< 0, 0, 0, 0, 0, 0, 0, T >
				   		       &t )
	{ 	
	    this->_scalar	       *= t.scalar();
	    return *this;
	}
	value 		        operator*(
			           const value< 0, 0, 0, 0, 0, 0, 0, T >
				   		       &t )
	    const
	{ 
	    return value( *this )      *= t;
	}
	template < typename V >
	const value 	       &operator*=(
			            const value< 0, 0, 0, 0, 0, 0, 0, V >
				   		       &v )
	{ 	
	    this->_scalar	        = T( this->_scalar * v.scalar() );
	    return *this;
	}
	template < typename V >
	value 		        operator*(
			            const value< 0, 0, 0, 0, 0, 0, 0, V >
				   		       &v )
	    const
	{ 
	    return value( *this )      *= v;
	}
	template < typename V >
	value 		        operator*=(
				    const V	       &v )		// Scalar multiplication, any type
	{
	    this->_scalar	        = T( this->_scalar * v );
	    return *this;
	}
	template < typename V >
	value 		        operator*(
				    const V	       &v )
	    const
	{
	    return value( *this )      *= v;
	}

	const value	       &operator/=(
			            const value< 0, 0, 0, 0, 0, 0, 0, T >
				   		       &t )		// Unitless division, same type
	{ 
	    this->_scalar	       /= t.scalar();
	    return *this;
	}
	value		        operator/(
			            const value< 0, 0, 0, 0, 0, 0, 0, T >
				   		       &t )
	    const
	{
	    return value( *this )      /= t;
	}
	template < typename V >
	const value	       &operator/=(
			            const value< 0, 0, 0, 0, 0, 0, 0, V >
				   		       &v )		// Dimensional division, different type
	{ 
	    this->_scalar	        = T( this->_scalar / v.scalar() );
	    return *this;
	}
	template < typename V >
	value		        operator/(
			            const value< 0, 0, 0, 0, 0, 0, 0, V >
				   		       &v )
	    const
	{
	    return value( *this )      /= v;
	}
	template < typename V >
	value 		        operator/=(
				    const V	       &v )		// Scalar division, any type
	{
	    this->_scalar	        = T( this->_scalar / v );
	    return *this;
	}
	template < typename V >
	value 		        operator/(
				    const V	       &v )
	    const
	{
	    return value( *this )      /= v;
	}

	// 
	// + += - -= -'ve 
	//
	///     Dimensional addition, subtraction, and negation.  No scalar or unitless
	/// addition/subtraction sensible.  The same units<...> type must be used (hence direct
	/// access to private members...)!  However, scalar addition/subtraction of same
	/// dimensional units in a differing type is allowed.
	/// 
	value		       &operator+=( 
				    const value	       &t )		// Dimensional addition, same type	
	{
	    this->_scalar 	       += t._scalar;
	    return *this;
	}
	value 			operator+(
				    const value	       &t )
	    const 
	{	
	    return value( *this )      += t;
	}
	template < typename V >
	const value	       &operator+=(
			            const value< Mass, Leng, Time, Crnt, Temp, Matt, Lumi, V >
				   		       &v )		// Dimensional addition, different type
	{ 
	    this->_scalar	        = T( this->_scalar + v.scalar() );
	    return *this;
	}
	template < typename V >
	value		        operator+(
			            const value< Mass, Leng, Time, Crnt, Temp, Matt, Lumi, V >
				   		       &v )
	    const
	{
	    return value( *this )      += v;
	}

	value		       &operator-=( 
				    const value	       &t )		// Dimensional subtraction, same type
	{ 
	    this->_scalar 	       -= t._scalar;
	    return *this;
	}
	value 			operator-(
				    const value	       &t )
	    const 
	{	
	    return value( *this )      -= t;
	}
	template < typename V >
	const value	       &operator-=(
			            const value< Mass, Leng, Time, Crnt, Temp, Matt, Lumi, V >
				   		       &v )		// Dimensional subtraction, different type
	{ 
	    this->_scalar	        = T( this->_scalar + v.scalar() );
	    return *this;
	}
	template < typename V >
	value		        operator-(
			            const value< Mass, Leng, Time, Crnt, Temp, Matt, Lumi, V >
				   		       &v )
	    const
	{
	    return value( *this )      -= v;
	}

	value 			operator-()
	    const
	{ 
	    return value( - this->_scalar );
	}

	// 
	// == != < <= > >=
	// 
	///     Comparison.  Only units::value<...> of the same dimension are comparable.
	/// 
	bool 			operator==(
				    const value	       &t )
	    const
	{
	    return this->_scalar == t._scalar;
	}
	bool			operator!=(
				    const value	       &t )
	    const
	{
	    return this->_scalar != t._scalar;
	}
	bool			operator< (
				    const value	       &t )
	    const
	{
	    return this->_scalar <  t._scalar;
	}
	bool			operator<=(
			            const value	       &t )
	    const
	{
	    return this->_scalar <= t._scalar;
	}
	bool			operator> (
				    const value	       &t )
	    const
	{
	    return this->_scalar >  t._scalar;
	}
	bool			operator>=(
				    const value	       &t )
	    const
	{
	    return this->_scalar >= t._scalar;
	}

	// 
	// units::value<...,T> * units::value<...,V>
	//
	///     Multiplication by unit of another dimension; Adds exponents, multiplies scalar
	/// portion.  Handles units::value<...V> of different underlying types, so long as their
	/// types are multiplicable.  There can be no *= or /= operators between dimensional values,
	/// because the dimensions of the result must change!
	/// 
	/// NOTE
	/// 
	///     The type of the resultant scalar is our own type T.  Thus, the type of the
	/// leftmost operand determines the type, NOT the natural C++ promotion to the greater
	/// type (eg. int / double to double, etc.)
	/// 
	template< int A, int B, int C, int D, int E, int F, int G, typename V>
        value< Mass + A,
	       Leng + B,
	       Time + C,
	       Crnt + D,
	       Temp + E,
	       Matt + F,
	       Lumi + G, T >	operator*(
				    const value< A, B, C, D, E, F, G, V >
				    		      &rhs )
	    const
	{
#if defined( DEBUG )
	    std::cout << *this << "   *      " << rhs << std::endl;
#endif
	    return value< Mass + A,
		          Leng + B,
		          Time + C,
 		          Crnt + D,
		          Temp + E,
		          Matt + F,
		          Lumi + G, T >( T( scalar() * rhs.scalar() ));
	}

	// 
	// units::value<...,T> / unit<V>::value<...,V>
	// 
	///     Division; Subtracts exponents, divides scalar portion.  Any operand scalar type V
	/// which is divisible into our own type T is allowed.  See NOTE on type promotion, above.
	/// Type conversion warnings are discarded.
	/// 
	template< int A, int B, int C, int D, int E, int F, int G, typename V>
	value< Mass - A,
	       Leng - B,
	       Time - C,
	       Crnt - D,
	       Temp - E,
	       Matt - F,
	       Lumi - G, T >	operator/( 
				    const value< A, B, C, D, E, F, G, V >
				    		       &rhs )
	    const
	{
#if defined( DEBUG )
	    std::cout << *this << "   /      " << rhs << std::endl;
#endif
	    return value< Mass - A,
		          Leng - B,
		          Time - C,
		          Crnt - D,
		          Temp - E,
		          Matt - F,
		          Lumi - G, T >( T( scalar() / rhs.scalar() ));
	}
    }; // value (general)


    // 
    // units::value<0,...T>	-- Unitless Units case (partial specialization)
    // 
    ///     Allows automatic construction of Unitless values from scalars, for use in equations
    /// containing dimensional units.
    /// 
    ///     Obtain value of unitless types; remains undefined for Units with remaining non-zero
    /// exponents.  For every new units<...,T>, an explicit specialisation of this method MUST be
    /// defined!  We will create one for the default type units<...,double>.  Write a similar method
    /// for any new type T you want to allow implicit conversion for.
    /// 
    ///     Since there are NOT non-member operators for any units<...> type (too much potential for
    /// ambiguity), we give up the ability to do scalar multiplication/division with a leading scalar,
    /// eg:
    /// 
    ///         Conductance		siemens	= 1 / Ohms;
    /// or,     Data			byte	= 8 * Bits;
    /// 
    ///     Instead, we need to eliminate the constant, or put it last, or
    /// manually convert it to the "Unitless" units<...> type:
    /// 
    ///         Conductance		siemens	= Unitless( 1 ) / Ohms;
    /// or,     Data			byte	= Bits * 8;
    /// 
    /// 
    ///     The bulk of mathematical operations can be achieved by automatic conversion back into
    /// a basic type.  Only those operations that result in different dimensional exponents
    /// (multiplication and division) need to be implemented here.
    /// 
    template < typename T >
    class value< 0, 0, 0, 0, 0, 0, 0, T >
    {
	T			_scalar;

    public:
	
	// 
	// scalar()
	// T = units::value<0,0,0,0,0,0,0,0,T>
	// 
	///     Implicit conversion of unitless unit<T>::value<...> types to T is OK -- all units
	/// have been solved for!  This is where the magic occurs; only expressions that solve to
	/// the unitless type can (automatically) be promoted to simple scalar values!
	/// 
				operator const T &() 
	    const
	{
	    return scalar();
	}

	const T		       &scalar()
	    const
	{
	    return _scalar;
	}

	// 
	// Constructor, default
	// Constructor, copy
	// 
	///     Used only to construct instances of the unitless units::value<...,T> types
	/// (eg. Count, Angle, ...).  All default to 1 unit.
	/// 
				value()					// Default
				    : _scalar( 1 )
	{
	    ;
#if defined( DEBUG )
	    std::cout << *this << "(default)" << std::endl;
#endif
	}
	
 				value(
				    const value	       &t )		// Unitless, same type
				    : _scalar( t.scalar() )
	{
	    ;
#if defined( DEBUG )
	    std::cout << *this << "   (copy) from: " << std::setw( 13 ) << t << std::endl;
#endif
	}
	template < typename V >	value(
				    const value< 0, 0, 0, 0, 0, 0, 0, V >
				    		       &v )		// Unitless, any type
				    : _scalar( T( v.scalar() ))
	{
	    ;
#if defined( DEBUG )
	    std::cout << *this << "   (copy) from: " << std::setw( 13 ) << v << std::endl;
#endif
	}

	// 
	// Constructor, scalar
	// 
	///     Implicit conversions from scalars (of any type) to the unitless type are OK.
	/// There must be a conversion from V to T.  HOWEVER, all type conversion warnings are
	/// discarded!  We do this, to allow for assignment from real-valued dimensional types, to
	/// integer types.
	/// 
	template < typename V >	value(
				    const V	     &v )
				    : _scalar( T( v ))			// Scalar, any type
	{
	    ;
#if defined( DEBUG )
	    std::cout << *this << " (scalar) from: " << std::setw( 13 ) << v << std::endl;
#endif
	}

	// 
	// units::value<0,0,0,0,0,0,0,0,T> * units::value<...,V>
	// 
	///     Scalar (unitless) Multiplication by any units::value<...,V>; Maintains exponents,
	/// multiplies scalar.  Allows operand to be any units<...V> where V is compatible with T.
	/// Resultant scalar() is of our own type T.
	/// 
	template< int A, int B, int C, int D, int E, int F, int G, typename V >
	value< A, B, C, D, E, F, G, T>
				operator*( 
				    const value< A, B, C, D, E, F, G, V >
				    		       &rhs )
	    const
	{
#if defined( DEBUG )
	    std::cout << *this << "   *      " << rhs << std::endl;
#endif
	    return value< A, B, C, D, E, F, G, T >( scalar() * rhs.scalar() );
	}

	// 
	// units::value<0,0,0,0,0,0,0,T> * units::value<..., V>
	// 
	///     Scalar (unitless) Division by unit<V>::value<...>; Inverts exponents, divides
	/// scalar portion.
	/// 
	template< int A, int B, int C, int D, int E, int F, int G, typename V >
	value<-A,-B,-C,-D,-E,-F,-G, T>
				operator/(
				    const value< A, B, C, D, E, F, G, V >
				    		       &rhs )
	    const
	{
#if defined( DEBUG )
	    std::cout << *this << "   /      " << rhs << std::endl;
#endif
	    return value<-A,-B,-C,-D,-E,-F,-G, T>( scalar() / rhs.scalar() );
	}
    }; // value (unitless)

    // 
    // units::type<T>
    // 
    ///     Define the basic unit dimensions, constants and derived types, enumerated in a specific
    /// type.
    /// 
    template < typename T = double >
    struct type {

	// 
	//     Define the basic dimensioned and dimensionless types.  A value may be declared to
	// have any of these types; such a variable must be assigned a value<...> that resolves to
	// the appropriate type:
	// 
	//     Matter		moles	= Mole * 1000; 			// 1000 moles
	//     Velocity		mps	= Meter * 1000 / Second;	// 1000 m/s
	// 
	//     You can disable unit managment by defining UNITS_DISABLED, to replace the complex
	// value<...,T> type with a simple T.
	// 
#if ! defined( UNITS_DISABLED )
	typedef value< 0, 0, 0, 0, 0, 0, 0, T >	Unitless;		// Multipliers, etc.

	// Basic Units
	typedef value< 1, 0, 0, 0, 0, 0, 0, T >	Mass;
	typedef value< 0, 1, 0, 0, 0, 0, 0, T >	Length;
	typedef value< 0, 0, 1, 0, 0, 0, 0, T >	Time;
	typedef value< 0, 0, 0, 1, 0, 0, 0, T >	Current;
	typedef value< 0, 0, 0, 0, 1, 0, 0, T >	Temperature;
	typedef value< 0, 0, 0, 0, 0, 1, 0, T >	Matter;
	typedef value< 0, 0, 0, 0, 0, 0, 1, T >	Luminance;

	// Physical Derived Units.  Useful with integer or real types of identical scale (exponents
	// of numerator(s) are >= exponents of demonimators).
	typedef value< 0, 2, 0, 0, 0, 0, 0, T >	Area;
	typedef value< 0, 3, 0, 0, 0, 0, 0, T >	Volume;
	typedef value< 0, 1,-1, 0, 0, 0, 0, T >	Velocity;
	typedef value< 1, 1,-2, 0, 0, 0, 0, T >	Force;
	typedef value< 0, 3,-1, 0, 0, 0, 0, T >	Flowrate;
	typedef value< 1, 2,-2, 0, 0, 0, 0, T >	Torque;
	typedef value< 1, 2,-2, 0, 0, 0, 0, T >	Energy;
	typedef value< 1, 2,-3, 0, 0, 0, 0, T >	Power;

	// Electrical Derived Units (generally not instantiable using type<T> with integer T, due
	// to high exponent denominators).
	typedef value< 0, 0, 1, 1, 0, 0, 0, T >	Charge;
	typedef value< 1, 2,-3,-1, 0, 0, 0, T >	ElecPotential;
	typedef value< 1, 2,-4,-2, 0, 0, 0, T >	Capacitance;
	typedef value< 1, 2,-3,-2, 0, 0, 0, T >	Resistance;
	typedef value<-1,-2, 3, 2, 0, 0, 0, T >	Conductance;		// Siemens
	typedef value< 1, 2,-2,-1, 0, 0, 0, T >	MagneticFlux;		// Weber  = Volt Seconds
	typedef value< 1, 2,-2,-2, 0, 0, 0, T >	Inductance;		// Henry  = Weber/Ampere

	// Other Units (often not useful with integer types, due to high exponent denominators).
	typedef value< 0, 1,-2, 0, 0, 0, 0, T >	Acceleration;		// m/s^2
	typedef value< 1,-1,-2, 0, 0, 0, 0, T >	Pressure;
	typedef value< 0, 0,-1, 0, 0, 0, 0, T >	Frequency;		// eg. __ per second
	typedef value< 0,-2, 0, 0, 0, 0, 0, T >	Density;		// eg. __ per square meter
	typedef value< 0,-2, 0, 0, 0, 0, 0, T >	Mileage;		// eg. Miles per Gallons
	typedef value< 0, 2, 0, 0, 0, 0, 0, T >	Efficiency;		// eg. Liters per 100km
	typedef value< 1, 0,-1, 0, 0, 0, 0, T >	Consumption;		// eg. Kilograms per Hour (not Pounds, which is a unit of Force!!!)
	typedef value< 0,-1, 1, 0, 0, 0, 0, T >	SpecificConsumption;	// eg. Kilograms per Hour per Newton

#else

	// Use underlying type T instead of fancy unit<T>::value template...

	/*  Define the base categories	*/
	typedef T				Unitless;

	typedef T				Mass;
	typedef T				Length;
	typedef T				Time;
	typedef T				Current;
	typedef T			       	Temperature;
	typedef T				Matter;
	typedef T				Luminance;

	typedef T				Area;
	typedef T				Volume;
	typedef T				Velocity;
	typedef T				Force;
	typedef T				Flowrate;
	typedef T				Torque;
	typedef T				Energy;
	typedef T				Power;

	typedef T				Charge;
	typedef T				ElecPotential;
	typedef T				Capacitance;
	typedef T				Resistance;
	typedef T				Conductance;
	typedef T				MagneticFlux;
	typedef T				Inductance;

	typedef T				Acceleration;
	typedef T				Pressure;
	typedef T			        Frequency;
	typedef T				Density;
	typedef T				Mileage;
	typedef T				Efficiency;
	typedef T				Consumption;
	typedef T				SpecificConsumption;

#endif  // UNITS_DISABLED

	// 
	//     The dimensioned and dimensionless base units.  These constants are defined as the
	// base units for each dimensional type, and must be combined in equations to satisfy the
	// dimensional requirements of the assignee.
	// 
	//     These must be instances of the above type, so that they may be used as bare names in
	// equations.  The default constructor for units::type<T> will initialize them.
	// 
	//     By default, all units in these dimensions are measured in terms of multiples of a
	// single (1) unit in that dimension.  However, any scalar multiple may be provided to the
	// units::type<T> constructor, for each dimension.  This is useful is an integer type is
	// provided for a unit; if precision to the decagram, centimeter, and millisecond (for
	// example) is desired (1 unit per Ampere, Kelvin, and Mole, etc.):
	// 
	//         typedef units::type<int>	ui_t;
	//         ui_t				ui( 100, 1000, 10 );
	// 
	// would provide an adequate set of SI base units, over type int, in multiples of 100,
	// 1000 and 10 per base unit.  All of the base derived units are assured to be meaningful
	// in integer calculations, to a precision equal to that fraction of the base unit.
	// 
	//     When we provide constants in appropriate dimension base and and derived types, we
	// will provide some common non-trivial multiples (not easily produced with multiplication
	// by Kilo, etc.): Minute, Hour, Day.  Mile, Yard, Foot, Inch.
	// 
	const Mass		Kilogram;
	const Length		Meter;
	const Time 		Second;
	const Current		Ampere;
	const Temperature	Kelvin;
	const Matter		Mole;
	const Luminance		Candela;

	const Unitless		Radian;					// Plane angle (triangle)
	const Unitless		Steradian;				// Solid angle (cone)
	const Unitless		Count;					// Bit, Cycle

	// 
	//     The standard SI dimensionless multipliers.  These also must be instances, to be used
	// as bare names in equations.
	// 
	const Unitless		Deci;
	const Unitless		Centi;
	const Unitless		Milli;
	const Unitless		Micro;
	const Unitless		Nano;

	const Unitless		Deca;
	const Unitless		Hecto;
	const Unitless		Kilo;
	const Unitless		Mega;
	const Unitless		Giga;

	const Volume 		Liter;

	const Time 		Minute;
	const Time		Hour;
	const Time		Day;

	const Force 		Newton;

	const Energy 		Joule;

	const Velocity		KM_H;

	const Unitless		Revolution;
	const Unitless		Degree;
	const Unitless		Clock;

	const Frequency		Hertz;

	// Other useful constants, which must be carefully calculated, to avoid division by zero
	// for integer types.  Generally, these constants should only be used with real-valued
	// types!
	const Acceleration	Gravity;		
	const Pressure		Pascal;
	const Power 		Watt;
	const Efficiency	L_100KM;
#if 0
	// 
	// Celsius
	// 
	///     Shifts Temperature units between Kelvin and Celsius degrees.  If used as a
	/// Temperature, then the entire equation will take place in Celsius degress, instead of
	/// Kelvin.  Can be manually converted
	/// 
	/// INCOMPLETE
	/// 
	///    And, probably a dumb idea...
	/// 
	class Celsius
	    : public Temperature {
	public:
	    // 
	    // Constructor, scalar
	    // 
	    ///     An explicit Celsius value is provided.  Use as-is.
	    /// 
	    			Celsius(
				    const T	       &t )
				    : Temperature( t )
	    {
		;
	    }

	    // 
	    // Constructor, Temperature
	    // 
	    ///     An incoming raw Temperature is provided.  Convert to Celsius.
	    // 
	    			Celsius(
				    const Temperature  &kelvin )
				    : Temperature( kelvin.scalar() - 460 )
	    {
		; 
	    }
	    Temperature		Kelvin()
	    {
		return Temperature( this->scaler() + 460 );
	    }
	}; // Celsius
#endif // 0 

				type(
				    T			mass	= 1,
				    T			leng	= 1,
				    T			time	= 1,
				    T			crnt	= 1,
				    T 			temp	= 1,
				    T			matt	= 1,
				    T			lumi	= 1,
				    T			angl	= 1,
				    T			soli	= 1,
				    T			numb	= 1 )
				    : Kilogram(		mass )
				    , Meter( 		leng )
				    , Second( 	        time )
				    , Ampere(		crnt )
				    , Kelvin( 		temp )
				    , Mole( 		matt )
				    , Candela(		lumi )

				    , Radian(		angl )
				    , Steradian(	soli )
				    , Count(		numb )

				    , Deci( 	Unitless( 1 )	/ 10 )
				    , Centi(  	Unitless( 1 )	/ 100 )
				    , Milli(  	Unitless( 1 )	/ 1000 )
				    , Micro(  	Milli		/ 1000 )
				    , Nano(  	Micro		/ 1000 )

				    , Deca(			  10 )
				    , Hecto(	                  100 )
				    , Kilo(	                  1000 )
				    , Mega(	Kilo 		* 1000 )
				    , Giga(	Mega		* 1000 )

				    , Liter(	Meter * Meter * Meter / 1000 )

				    , Minute( 	Second		* 60 )
				    , Hour(	Minute 		* 60 )
				    , Day(	Hour		* 24 )

				    , Newton(	Kilogram 	* Meter / Second / Second )

				    , Joule( 	Newton 		* Meter )

				    , KM_H( 	Kilo * Meter	/ Hour )

				    , Revolution( Radian	* 2 * M_PI )
				    , Degree(	Revolution	/ 360 )
				    , Clock(	Revolution	/ 12 )

				    , Hertz( 	Count 		/ Second )

				    , Gravity(  Meter		* 9.80665  / Second / Second )
				    , Pascal( 	Newton 		/ Meter / Meter )
				    , Watt( 	Joule 		/ Second )

				    , L_100KM( 	Meter * Meter * Meter
								/ ( Kilo * Meter * 100 )
								/ 1000 )
	{
	    ;
	}

	// 
	// units::type<T>::imperial
	// 
	//     Some standard Imperial dimensional units, in terms of each dimensional base.
	// 
	//     All these constants are valid for integer types.
	// 
	struct imperial {
	    const Length 	Mile;
	    const Length 	Yard;
	    const Length 	Feet;
	    const Length 	Inch;
	    const Force		Pound;
	    const Force		OZ;
	    const Volume 	Gallon;
	    const Volume 	Quart;
	    const Volume 	Pint;
	    const Volume 	Ounce;
	    const Velocity	Miles_Hour;
	    const Torque 	FootPound;
	    const Energy	BTU;
	    const Mileage	MPG;

	    			imperial(
				    const type<T>      &base )
				    : Mile( 	base.Meter 	* 5280 * 0.3047999989 )
				    , Yard( 	Mile 		/ 1760 )
				    , Feet( 	Yard		/ 3 )
				    , Inch( 	Feet	        / 12 )
				    , Pound( 	base.Newton	* 4.44822161526 )
				    , OZ( 	Pound		/ 16 )
				    , Gallon( 	base.Meter * base.Meter * base.Meter
								* 4.54609 / 1000 )
				    , Quart( 	Gallon		/ 4 )
				    , Pint( 	Quart		/ 2 )
				    , Ounce( 	Pint		/ 20 )
				    , Miles_Hour( Mile		/ base.Hour )
				    , FootPound( Feet 		* Pound )
				    , BTU( 	base.Kilo * base.Joule
								* 1.05505585262 )
				    , MPG( Gallon / Volume( 1 ) > T( 0 )
					   ? Mile / Gallon
					   : Mileage( 0 ))
	    {
		;
	    }
#if 0
	    // 
	    // Fahrenheit
	    // 
	    /// Can convert directly back and forth between Celsius and Fahrenheit, and indirectly
	    /// (via Celsius) to/from base Kelvin Units.
	    /// 
	    /// INCOMPLETE
	    /// 
	    ///     And, probably a dumb idea...
	    /// 
	    class Fahrenheit
		: public Temperature {
		// 
		// Constructor, scalar
		// 
		///     An explicit Fahrenheit value is provided.  Use as-is.
		/// 
	    			Fahrenheit(
				    const T 	       &t )
				    : Temperature( t )
		{
		    ;
		}

		// 
		// Constructor, Celsius
		// 
	    			Fahrenheit(
				    const Celsius      &celcius )
				    : Temperature( ( celcius.scalar()
						     * 100 
						     / ( 212 + 32 ))
						   - 32 )
		{
		    ; 
		}

				operator Celsius()
		{
		    return Celsius( ( this->_scaler
				      + 32 )
				    * ( 212 + 32 )
				    / 100 );
		}
	    }; // Fahrenheit
#endif // 0
	};

	// 
	// us
	// 
	///     US units.  Same as imperial, except all the volumetric units are adjusted to US
	/// Gallons.  A US Pint is 16 fluid ounces, vs. Imperial Pint of 20 fluid ounces.
	/// 
	struct us 
	    : public imperial {
	    Volume		Gallon;
	    Volume		Quart;
	    Volume		Pint;
	    Mileage		MPG;

	    			us(
				    const type<T>      &base )
				    : imperial( base )
				    , Gallon( base.Meter * base.Meter * base.Meter
								* 3.785411784 / 1000 )
				    , Quart( 	Gallon		/  4 )
				    , Pint( 	Quart		/  2 )			// Ounce same as Imperial!
				    , MPG( Gallon / Volume( 1 ) > T( 0 )
					   ? this->Mile / Gallon
					   : Mileage( 0 ) )
	    {
		;
	    }
	};

	// 
	// electrical
	// 
	///     Electrical constants in the Charge dimension.
	/// 
	struct electrical {
	    const ElecPotential	Volt;
	    const Charge 	Coulomb;
	    const Capacitance 	Farad;
	    const Resistance 	Ohm;
	    const Conductance	Siemens;
	    const MagneticFlux	Weber;
	    const Inductance	Henry;

	    			electrical(
				    const type<T>      &base )
				    : Volt(	base.Watt	/ base.Ampere )
				    , Coulomb(	base.Ampere	* base.Second )
				    , Farad( 	Volt	 	/ Coulomb )
				    , Ohm( 	Volt		/ base.Ampere )
				    , Siemens(	Unitless( 1 )	/ Ohm )
				    , Weber( 	Volt		* base.Second )
				    , Henry( 	Weber		/ base.Ampere )
	    {
		;
	    }
	};

	// 
	// binary
	// 
	///     Constants useful in data and communications.
	/// 
	struct binary {
	    const Unitless	K;
	    const Unitless	Kibi;
	    const Unitless	M;
	    const Unitless	Mibi;
	    const Unitless	G;
	    const Unitless	Gibi;

	    const Frequency	BPS;
	    const Frequency	KBPS;
	    const Frequency	MBPS;

	    const Unitless 	Byte;
	    const Unitless 	KByte;
	    const Unitless 	MByte;

	    			binary(
				    const type<T>      &base )
				    : K( 	1024 )
				    , Kibi( 	K )
				    , M( 	K 		* K )
				    , Mibi( 	M )
				    , G( 	M 		* K )
				    , Gibi( 	G )

				    , BPS( 	base.Count	/ base.Second )
				    , KBPS(	BPS 		* K )
				    , MBPS( 	KBPS		* K )

				    , Byte( 	base.Count	* 8 )
				    , KByte( 	Byte		* K )
				    , MByte( 	KByte		* K )
	    {
		;
	    }
	};

    }; // units::type<T>
}; // unit

template< int A, int B, int C, int D, int E, int F, int G, typename T >
inline
std::ostream	       	       &operator<<(
			            std::ostream	       &out,
				    const units::value< A, B, C, D, E, F, G, T >
				    			       &t )
{
    if ( A || B || C || D || E || F || G ) {
	out << '<' 		<< std::setw(  2 ) << A
	    << ',' 		<< std::setw(  2 ) << B
	    << ',' 		<< std::setw(  2 ) << C
	    << ',' 		<< std::setw(  2 ) << D
	    << ',' 		<< std::setw(  2 ) << E
	    << ',' 		<< std::setw(  2 ) << F
	    << ',' 		<< std::setw(  2 ) << G
	    << ", T>";
    } else {
	out << "<                    , T>";
    }
    return out << std::setw( 13 ) << t.scalar();
}


#endif // _INCLUDE_UNITS_H
