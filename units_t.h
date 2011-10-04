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


#ifndef UNITS_T_H
#define UNITS_T_H


#ifdef CHECK_UNITS

	//  ---------------------------------  //
	//  Define the Units template and      //
	//  all of its operators				   //

	// All of the reinterpret casts are work-arounds to let us make
	// m_Scalar & Units(double) private without using friends
	template< int iMassExponent
				 ,int iLengthExponent
				 ,int iTimeExponent
				 ,int iCurrentExponent
	> class Units {

		public:

			Units():m_Scalar(1.0){}	// only used to consturct base units
			Units( const Units& u ):m_Scalar( u.m_Scalar ){}
			const Units& operator=( const Units& u ){	m_Scalar=u.m_Scalar; return *this; }

			// scalar mulitplication & division
			Units operator*( double d ) const { return m_Scalar*d; }
			Units operator/( double d ) const { return m_Scalar/d; }
			const Units& operator*=( double d ) { m_Scalar*=d; return *this; }
			const Units& operator/=( double d ) { m_Scalar/=d; return *this; }

			// unit additions & subtraction
			Units operator+( const Units& u ) const {	return m_Scalar+u.m_Scalar; }
			Units operator-( const Units& u ) const {	return m_Scalar-u.m_Scalar; }
			Units& operator+=( const Units& u ) { m_Scalar+=u.m_Scalar; return *this; }
			Units& operator-=( const Units& u ) { m_Scalar-=u.m_Scalar; return *this; }
			Units operator-() const { return -m_Scalar; }

			// comparison
			bool operator==( const Units& u ) const { return m_Scalar==u.m_Scalar; }
			bool operator!=( const Units& u ) const { return m_Scalar!=u.m_Scalar; }
			bool operator< ( const Units& u ) const { return m_Scalar< u.m_Scalar; }
			bool operator<=( const Units& u ) const { return m_Scalar<=u.m_Scalar; }
			bool operator> ( const Units& u ) const { return m_Scalar> u.m_Scalar; }
			bool operator>=( const Units& u ) const { return m_Scalar>=u.m_Scalar; }

			// scalar typecast
			operator double() const;// not defined here because only unitless can cast to double

			// Unit Multiplication
			template< int A, int B, int C, int D >
				Units<iMassExponent+A,iLengthExponent+B,iTimeExponent+C,iCurrentExponent+D >
				operator*( const Units<A,B,C,D>& u2 ) const {
					Units<iMassExponent+A,iLengthExponent+B,iTimeExponent+C,iCurrentExponent+D> r;
					*reinterpret_cast<double*>(&r) = m_Scalar* *reinterpret_cast<const double*>(&u2);
					return r;
				}

			// Unit Division
			template< int A, int B, int C, int D >
				Units<iMassExponent-A,iLengthExponent-B,iTimeExponent-C,iCurrentExponent-D>
				operator/( const Units<A,B,C,D>& u2 ) const {
					Units<iMassExponent-A,iLengthExponent-B,iTimeExponent-C,iCurrentExponent-D> r;
					*reinterpret_cast<double*>(&r) = m_Scalar/ *reinterpret_cast<const double*>(&u2);
					return r;
				}

		private:

			double m_Scalar;

			// used by */+- to make returning values easy
			Units( double val ):m_Scalar(val){}

	};


	// only defined for unitless types
	inline Units<0,0,0,0>::operator double() const {
		return m_Scalar;
	}


	// Scalar Multiplication & Division
	template< int A, int B, int C, int D >
	Units< A, B, C, D > inline
	operator*( double d, const Units< A, B, C, D > &u) {
		return u*d;
	}

	template< int A, int B, int C, int D >
	Units< -A, -B, -C, -D > inline
	operator/( double d, const Units< A, B, C, D >& u) {
		Units< -A, -B, -C, -D > r;
		*reinterpret_cast<double*>(&r) = d / *reinterpret_cast<const double*>(&u);
		return r;
	}



	/*  Define the base categories	*/
	typedef Units< 1,  0,  0,  0 >	Mass;
	typedef Units< 0,  1,  0,  0 >	Length;
	typedef Units< 0,  0,  1,  0 >	Time;
	typedef Units< 0,  0,  0,  1 >	Current;
	typedef Units< 0,  0,  0,  0 >	Angle;

	/*  Define the derived categories  */
	typedef Units< 0,  2,  0,  0 >	Area;
	typedef Units< 0,  3,  0,  0 >	Volume;
	typedef Units< 1,  1, -2,  0 >	Force;
	typedef Units< 1, -1, -2,  0 >	Pressure;
	typedef Units< 0,  1, -1,  0 >	Velocity;
	typedef Units< 0,  0, -1,  0 >	AngularVelocity;
	typedef Units< 0,  3, -1,  0 >	Flowrate;
	typedef Units< 1,  2, -2,  0 >	Torque;
	typedef Units< 1,  2, -2,  0 >	Energy;
	typedef Units< 1,  2, -3,  0 >	Power;

	typedef Units< 0,  0,  1,  1 >	Charge;
	typedef Units< 1,  2, -3, -1 >	ElecPotential;
	typedef Units< 1,  2, -4, -2 >	Capacitance;
	typedef Units< 1,  2, -3, -2 >	Resistance;
	typedef Units<-1, -2,  3,  2 >	Conductance;

	#define CategoryBase( cat, base ) const cat base;

#else

	// Fancy Units template becomes just a scalar
	typedef double Units;

	/*  Define the base categories	*/
	typedef Units	Mass;
	typedef Units	Length;
	typedef Units	Time;
	typedef Units	Current;
	typedef Units	Angle;

	/*  Define the derived categories  */
	typedef Units	Area;
	typedef Units	Volume;
	typedef Units	Force;
	typedef Units	Pressure;
	typedef Units	Velocity;
	typedef Units	AngularVelocity;
	typedef Units	Flowrate;
	typedef Units	Torque;
	typedef Units	Energy;
	typedef Units	Power;

	typedef Units	Charge;
	typedef Units	ElecPotential;
	typedef Units	Capacitance;
	typedef Units	Resistance;
	typedef Units	Conductance;

	#define CategoryBase( cat, base ) const cat base = 1.0;


#endif  // CHECK_UNITS




//  ----------------------------  //
//  Pick the Categories' Base     //

CategoryBase( Mass,		Kilograms	);
CategoryBase( Length,	Meters		);
CategoryBase( Time,		Seconds		);
CategoryBase( Current,	Amperes		);
CategoryBase( Angle,		Radians		);



//  ---------------------------------------------  //
//  define the unit constants in terms of the base //

const Length Feet = 0.3048 * Meters;
const Length Miles = 5280.0 * Feet;
const Length Inches = Feet / 12.0;
const Length Centimeters = Meters / 100.0;
const Length Kilometers = 1000.0 * Meters;

const Volume Meters3 = Meters * Meters * Meters;
const Volume Liters = 0.001 * Meters3;
const Volume Gallons = 3.785411784 * Liters;
const Volume Centimeters3 = Meters3 / 1.0e6;


const Time Minutes = 60.0 * Seconds;
const Time Hours = 60.0 * Minutes;
const Time Days = 24.0 * Hours;

const Velocity Feet_Second = Feet/Seconds;		// '_' indicates division
const Velocity Inches_Second = Inches/Seconds;
const Velocity Miles_Hour = Miles/Hours;

const Force Newtons = Kilograms * Meters / ( Seconds * Seconds );
const Force Pounds = 4.44822161526 * Newtons;

const Pressure Pascals = Newtons / (Meters * Meters );
const Pressure Bars = 100000.0 * Pascals;
const Pressure PSI = Pounds / (Inches * Inches);

const Angle Revolutions = 6.28318530718 * Radians;
const Angle Degrees = Revolutions / 360.0;
const Angle Clocks = Revolutions / 12.0;

const Torque FootPounds = Feet * Pounds;

const Energy Joules = Newtons * Meters;
const Power Watts = Joules / Seconds;

const ElecPotential Volts = Watts / Amperes;
const Charge Coulumbs = Amperes * Seconds;
const Capacitance Farads = Volts / Coulumbs;
const Resistance Ohms = Volts / Amperes;
const Charge Faradays = 96487.0 * Coulumbs;
const Conductance Semiens = 1.0 / Ohms;
const Conductance Mhos = 1.0 / Ohms;

const AngularVelocity RPM = Revolutions / Minutes;
const Flowrate GPM = Gallons / Minutes;


#endif
