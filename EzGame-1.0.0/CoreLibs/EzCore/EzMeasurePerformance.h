#ifndef _EZMEASUREPERFORMANCE_H_INCLUDE_
#define _EZMEASUREPERFORMANCE_H_INCLUDE_

#define EZMEASURE_NAMELENGTH		 256

#include "EzMetricsTimer.h"

//#define EZMETRICS_DX9RENDERER_PREFIX NIMETRICS_PREFIX "dx9."


class EZCORE_ENTRY EzMeasurePerformance
{
public:
	enum EMEASURE
	{
		// DrawPrimitive calls
		PRIMITIVE_COUNT,
		// Per frame, number of tris and verts that were drawn
		DRAW_TRIS,
		DRAW_VERTS,
		// Calls to SetRenderState
		RENDER_STATE_CHANGES,
		// Calls to SetRenderTarget
		RENDER_TARGET_CHANGES,
		// Calls to SetTexture
		TEXTURE_CHANGES,
		// Calls to SetStreamSource
		VERTEX_BUFFER_CHANGES,
		// Calls to SetPixelShader
		PIXEL_SHADER_CHANGES,
		// Calls to SetVertexShader
		VERTEX_SHADER_CHANGES,
		// Number of vertices in vertex buffers.  Updated after
		// CreateVertexBuffer calls.
		VERTEX_BUFFER_SIZE,
		// Number of indices in index buffers.  Updated after
		// CreateIndexBuffer calls.
		INDEX_BUFFER_SIZE,
		// Do_Render* time in seconds
		DRAW_TIME_LINES,
		DRAW_TIME_POINTS,
		DRAW_TIME_SHAPE,
		DRAW_TIME_STRIPS,
		DRAW_TIME_SCREENTEXTURE,
		CULLING_TIME,
		SKINNINGTIME,
		APPRUN_TIME,
		NUM_METRICS
	};

	static float getValue(EMEASURE eMetric);
	static void setValue(EMEASURE eMetric, float fTime);
	static void recordMetrics();
	static void incrementMetric(EMEASURE eMetric, int iInc = 1);
	static void recordTime(EMEASURE eMetric, float fTime);

	const static char ms_acNames[NUM_METRICS][EZMEASURE_NAMELENGTH];

protected:
	static int ms_aiPerFrameMetrics[INDEX_BUFFER_SIZE + 1];
	static float ms_afPerFrameTimes[NUM_METRICS - DRAW_TIME_LINES];
	
	static float ms_afValue[NUM_METRICS];
};

class EzMeasureScopeTimer : public EzPerformanceCounterTimer
{
public:
	EzMeasureScopeTimer(EzMeasurePerformance::EMEASURE eMeasure);
	~EzMeasureScopeTimer();
protected:
	EzMeasurePerformance::EMEASURE m_eMeasure;
};



#if _EZMEASURE_PERFORMANCE
	#define EZMEASURE_PERFORMANCE_ADDVALUE(eMeasure, fValue)		EzMeasurePerformance::setValue(EzMeasurePerformance::eMeasure, fValue)
	#define EZMEASURE_PERFORMANCE_VALUE(eMeasure)					EzMeasurePerformance::getValue(EzMeasurePerformance::eMeasure)
	#define EZMEASURE_PERFORMANCE_SCOPETIMER(eMeasure)				EzMeasureScopeTimer measureTimer(EzMeasurePerformance::eMeasure)
	#define EZMEASURE_PERFORMANCE_RECORDMETRICS()					EzMeasurePerformance::recordMetrics()
	#define EZMEASURE_PERFORMANCE_AGGREGATEVALUE(eMeasure, iSize)	EzMeasurePerformance::incrementMetric(EzMeasurePerformance::eMeasure,(int)iSize)
#else //#if _EZMEASURE_PERFORMANCE
	#define EZMEASURE_PERFORMANCE_ADDVALUE(eMetric, fValue)
	#define EZMEASURE_PERFORMANCE_VALUE(eMeasure)	
	#define EZMEASURE_PERFORMANCE_SCOPETIMER(eMetric)
	#define EZMEASURE_PERFORMANCE_RECORDMETRICS()
	#define EZMEASURE_PERFORMANCE_AGGREGATEVALUE(eMetric, iSize)
#endif //#if _EZMEASURE_PERFORMANCE



inline void EzMeasurePerformance::recordMetrics()
{
	EZMEASURE_PERFORMANCE_ADDVALUE(PRIMITIVE_COUNT, (float) ms_aiPerFrameMetrics[PRIMITIVE_COUNT]);
	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_TRIS, (float)ms_aiPerFrameMetrics[DRAW_TRIS]);
	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_VERTS, (float)ms_aiPerFrameMetrics[DRAW_VERTS]);
	EZMEASURE_PERFORMANCE_ADDVALUE(RENDER_STATE_CHANGES, (float)ms_aiPerFrameMetrics[RENDER_STATE_CHANGES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(RENDER_TARGET_CHANGES, (float)ms_aiPerFrameMetrics[RENDER_TARGET_CHANGES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(VERTEX_BUFFER_CHANGES,(float) ms_aiPerFrameMetrics[VERTEX_BUFFER_CHANGES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(PIXEL_SHADER_CHANGES, (float)ms_aiPerFrameMetrics[PIXEL_SHADER_CHANGES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(VERTEX_SHADER_CHANGES,(float) ms_aiPerFrameMetrics[VERTEX_SHADER_CHANGES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(VERTEX_BUFFER_SIZE, (float)ms_aiPerFrameMetrics[VERTEX_BUFFER_SIZE]);
	EZMEASURE_PERFORMANCE_ADDVALUE(INDEX_BUFFER_SIZE, (float)ms_aiPerFrameMetrics[INDEX_BUFFER_SIZE]);

	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_TIME_LINES, ms_afPerFrameTimes[DRAW_TIME_LINES - DRAW_TIME_LINES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_TIME_POINTS, ms_afPerFrameTimes[DRAW_TIME_POINTS - DRAW_TIME_LINES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_TIME_SHAPE, ms_afPerFrameTimes[DRAW_TIME_SHAPE - DRAW_TIME_LINES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_TIME_STRIPS, ms_afPerFrameTimes[DRAW_TIME_STRIPS - DRAW_TIME_LINES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(DRAW_TIME_SCREENTEXTURE, ms_afPerFrameTimes[DRAW_TIME_SCREENTEXTURE - DRAW_TIME_LINES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(CULLING_TIME, ms_afPerFrameTimes[CULLING_TIME - DRAW_TIME_LINES]);
	EZMEASURE_PERFORMANCE_ADDVALUE(SKINNINGTIME, ms_afPerFrameTimes[SKINNINGTIME - DRAW_TIME_LINES]);

	ms_aiPerFrameMetrics[PRIMITIVE_COUNT] = 0;
	ms_aiPerFrameMetrics[DRAW_TRIS] = 0;
	ms_aiPerFrameMetrics[DRAW_VERTS] = 0;
	ms_aiPerFrameMetrics[RENDER_STATE_CHANGES] = 0;
	ms_aiPerFrameMetrics[RENDER_TARGET_CHANGES] = 0;
	ms_aiPerFrameMetrics[VERTEX_BUFFER_CHANGES] = 0;
	ms_aiPerFrameMetrics[PIXEL_SHADER_CHANGES] = 0;
	ms_aiPerFrameMetrics[VERTEX_SHADER_CHANGES] = 0;
	memset(ms_afPerFrameTimes, 0, sizeof(ms_afPerFrameTimes));
}


inline float EzMeasurePerformance::getValue(EMEASURE eMetric)
{
	return ms_afValue[eMetric];
}

inline void EzMeasurePerformance::setValue(EMEASURE eMetric, float fTime)
{
	ms_afValue[eMetric] = fTime;
}

inline void EzMeasurePerformance::incrementMetric(EzMeasurePerformance::EMEASURE eMetric, int iInc)
{
	ms_aiPerFrameMetrics[eMetric] += iInc;
}


inline void EzMeasurePerformance::recordTime(EMEASURE eMetric, float fTime)
{
	ms_afPerFrameTimes[eMetric - DRAW_TIME_LINES] += fTime;
}

inline EzMeasureScopeTimer::EzMeasureScopeTimer(EzMeasurePerformance::EMEASURE eMeasure)
{
	m_eMeasure = eMeasure;
	start();
}

inline EzMeasureScopeTimer::~EzMeasureScopeTimer()
{
	stop();
	EzMeasurePerformance::recordTime(m_eMeasure, getElapsed());
}



#endif
