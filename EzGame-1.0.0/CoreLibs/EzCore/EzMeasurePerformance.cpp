//

#include "EzMeasurePerformance.h"


const char EzMeasurePerformance::ms_acNames[EzMeasurePerformance::NUM_METRICS][EZMEASURE_NAMELENGTH] =
{
	"DrawCalls",
	"DrawnTris",
	"DrawnVerts",
	"RenderStateChanges",
	"RenderTargetChanges",
	"TextureChanges",
	"VertexBufferChanges",
	"PixelShaderChanges",
	"VertexShaderChanges",
	"VertexBufferUsage",
	"IndexBufferUsage",
	"drawtime.Lines",
	"drawtime.Points",
	"drawtime.Shape",
	"drawtime.Strips",
	"drawtime.ScreenTexture"
};

int EzMeasurePerformance::ms_aiPerFrameMetrics[EzMeasurePerformance::INDEX_BUFFER_SIZE + 1];
float EzMeasurePerformance::ms_afPerFrameTimes[EzMeasurePerformance::NUM_METRICS - EzMeasurePerformance::DRAW_TIME_LINES];

float EzMeasurePerformance::ms_afValue[NUM_METRICS];
