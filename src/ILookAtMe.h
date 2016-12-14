#ifndef ILOOKATME_H
#define ILOOKATME_H

/* AUTHOR: Josh Williams
 *   DATE: 2005.08.07
 *
 *	This is an interface that exposes a position vector to the
 *	in order to allow for real-time tracking.
 */
#include "math_main.h"

class ILookAtMe
{
	public:
		virtual matrix4x4 GetWorldMatrix(void) = 0;
};

#endif // ILOOKATME_H