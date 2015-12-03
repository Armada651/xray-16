#include "common.h"
#include "iostructs\v_TL.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_TL _main ( v_TL I)
{
	v2p_TL O;

	O.HPos = mul( m_WVP, I.P);
	O.Tex0 = I.Tex0;
	O.Tex0.y = 1 - O.Tex0.y;
	O.Color = I.Color.bgra;	//	swizzle vertex colour

 	return O;
}