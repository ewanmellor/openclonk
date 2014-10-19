#ifndef C4FOWLIGHTSECTION_H
#define C4FOWLIGHTSECTION_H

#include "C4Rect.h"
#include "C4FoWLight.h"

class C4FoWRegion;
class C4FoWBeam;

/** The light section manages the beams for one light for one direction of 90�.
    
	For understanding the internal calculations made in this class, note that this class assumes all of its beams to 
	go into a positive y-direction (="down" in display coordinates). Only after the calculation, it transforms its
	result into the global coordinate system again by applying a transformation matrix. 
	*/
class C4FoWLightSection
{
public:
	C4FoWLightSection(C4FoWLight *pLight, int r, C4FoWLightSection *pNext = NULL);
	~C4FoWLightSection();

private:

	/* Center light */
	C4FoWLight *pLight;

	/* Transformation matrix */
	int iRot;
	int a, b, c, d;
	int ra, rb, rc, rd;

	/* This section's beams */
	class C4FoWBeam *pBeams;

	C4FoWLightSection *pNext;

public:

	C4FoWLightSection *getNext() const { return pNext; }

	/** Recalculate of all light beams within the given rectangle because the landscape changed. */
	void Invalidate(C4Rect r);
	/** Update all light beams within the given rectangle */
	void Update(C4Rect r);

	void Render(C4FoWRegion *pRegion, const class C4TargetFacet *pOnScreen = NULL);

	/** Shorten all light beams to the given reach.
	    Called when the size of the light has decreased to the given value */
	void Prune(int32_t iReach);
	/** Extend all light beams to the given reach.
	    Called when the size of the light has increased to the given value */
	void Dirty(int32_t iReach);

private:

	/** Remove all beams. pBeams is NULL after that. */
	void ClearBeams();

	// Beam coordinate to landscape coordinate. Beam coordinates are relative to the light source.
	template <class T> T transDX(T dx, T dy) const { return T(a) * dx + T(b) * dy; }
	template <class T> T transDY(T dx, T dy) const { return T(c) * dx + T(d) * dy; }
	template <class T> T transX(T x, T y) const { return transDX(x, y) + T(pLight->getX()); }
	template <class T> T transY(T x, T y) const { return transDY(x, y) + T(pLight->getY()); }

	// Landscape coordinate to beam coordinate. Beam coordinates are relative to the light source.
	template <class T> T rtransDX(T dx, T dy) const { return T(ra) * dx + T(rb) * dy; }
	template <class T> T rtransDY(T dx, T dy) const { return T(rc) * dx + T(rd) * dy; }
	template <class T> T rtransX(T x, T y) const { return rtransDX(x-T(pLight->getX()),y-T(pLight->getY())); }
	template <class T> T rtransY(T x, T y) const { return rtransDY(x-T(pLight->getX()),y-T(pLight->getY())); }

	/** Returns a rectangle in beam coordinates */
	C4Rect rtransRect(C4Rect r) const {
		C4Rect Rect(rtransX(r.x, r.y), rtransY(r.x, r.y),
		            rtransDX(r.Wdt, r.Hgt), rtransDY(r.Wdt, r.Hgt));
		Rect.Normalize();
		return Rect;
	}

	bool isConsistent() const;

	/** These methods return the position of the left delimiter point of a beam from the origin that would enclose the 
	    given rectangle. Note that we assume the rect to have a positive Y-position.
		In other words, the given rectangle's right most point when looked at from the origin.
	  */
	int32_t RectLeftMostX(const C4Rect &r) const { return r.x; }
	int32_t RectLeftMostY(const C4Rect &r) const { return Max(0, r.x >= 0 ? r.y + r.Hgt : r.y); }
	/** These methods return the position of the left delimiter point of a beam from the origin that would enclose the 
	    given rectangle. Note that we assume the rect to have a positive Y-position.
		In other words, the given rectangle's right most point when looked at from the origin.
	  */
	int32_t RectRightMostX(const C4Rect &r) const { return r.x + r.Wdt; }
	int32_t RectRightMostY(const C4Rect &r) const { return Max(0, r.x + r.Wdt <= 0 ? r.y + r.Hgt : r.y); }

	/** Find right-most beam left of point */
	C4FoWBeam *FindBeamLeftOf(int32_t x, int32_t y);

	/** Find beams that go through the given rectangle. Returns the number of beams that are in the rectangle and makes
	    pFirst and pLast point to the first and the last beam of these */
	int32_t FindBeamsClipped(const C4Rect &pInRect, C4FoWBeam *&pFirst, C4FoWBeam *&pLast);
};


#endif