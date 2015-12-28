// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_LIGHT_H_INCLUDED__
#define __S_LIGHT_H_INCLUDED__

#include "../core/EzCore.h"

namespace GameEasy
{

//! Enumeration for different types of lights
enum ELIGHT_TYPE
{
	//! point light, it has a position in space and radiates light in all directions
	ELT_POINT = 0,
	//! spot light, it has a position in space, a direction, and a limited cone of influence
	ELT_SPOT,
	//! directional light, coming from a direction from an infinite distance
	ELT_DIRECTIONAL,

	MAX_LIGHT_TYPE
};


//! structure for holding data describing a dynamic point light.
/** Irrlicht supports point lights, spot lights, and directional lights.
*/
struct SLight
{
	SLight() : AmbientColor(0.f,0.f,0.f), DiffuseColor(1.f,1.f,1.f),
		SpecularColor(1.f,1.f,1.f), vAttenuation(1.f,0.f,0.f),
		fOuterCone(45.f), fInnerCone(0.f), fFalloff(2.f),
		vPosition(0.f,0.f,0.f), vDirection(0.f,0.f,1.f),
		fRadius(100.f), eType(ELT_POINT), bCastShadows(true)
		{}

	//! Ambient color emitted by the light
	EzColorf AmbientColor;

	//! Diffuse color emitted by the light.
	/** This is the primary color you want to set. */
	EzColorf DiffuseColor;

	//! Specular color emitted by the light.
	/** For details how to use specular highlights, see SMaterial::Shininess */
	EzColorf SpecularColor;

	//! Attenuation factors (constant, linear, quadratic)
	/** Changes the light strength fading over distance.
	Can also be altered by setting the radius, Attenuation will change to
	(0,1.f/radius,0). Can be overridden after radius was set. */
	EzVector3df vAttenuation;

	//! The angle of the spot's outer cone. Ignored for other lights.
	float fOuterCone;

	//! The angle of the spot's inner cone. Ignored for other lights.
	float fInnerCone;

	//! The light strength's decrease between Outer and Inner cone.
	float fFalloff;

	//! Read-ONLY! Position of the light.
	/** If Type is ELT_DIRECTIONAL, it is ignored. Changed via light scene node's position. */
	EzVector3df vPosition;

	//! Read-ONLY! Direction of the light.
	/** If Type is ELT_POINT, it is ignored. Changed via light scene node's rotation. */
	EzVector3df vDirection;

	//! Read-ONLY! Radius of light. Everything within this radius be be lighted.
	float fRadius;

	//! Read-ONLY! Type of the light. Default: ELT_POINT
	ELIGHT_TYPE eType;

	//! Read-ONLY! Does the light cast shadows?
	bool bCastShadows;
};

} 


#endif

