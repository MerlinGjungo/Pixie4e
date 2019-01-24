// Demo application for using the IEC63047 codec dll.
// 
// Copyright, 2016, European Atomic Energy Community
//
// Author: Jan Paepen, jan.paepen@ec.europa.eu




#include "reader.h"

#include "CodecDLL.h"

//#include <random>
#include <math.h>

#define XIA_READER

//#define PERF_MON // comment-out to disable performance monitoring functions



#ifdef PERF_MON
#include "windows.h" 
LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds, Frequency;

#define StartTiming() \
	QueryPerformanceFrequency(&Frequency); \
	QueryPerformanceCounter(&StartingTime);

#define StopTiming(C) \
	QueryPerformanceCounter(&EndingTime); \
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart; \
	ElapsedMicroseconds.QuadPart *= 1000000; \
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart; \
	printf("'%s' took %lld microseconds to execute\n",(C) ,ElapsedMicroseconds.QuadPart); \
	printf("Enter to continue...\n"); \
	getchar();
#else
#define StartTiming()
#define StopTiming(C)
#endif

void ReadEventPulseProperty(CPDU *pdu, unsigned int ChannelID, unsigned int EventPropID)
{
	unsigned int nValues, nFlags;
	char *ValueDescription, *FlagDescription;
	CPDU::FlagType FlTyp;
	nValues = pdu->H_GetChannelEventPulsePropertyNValues(ChannelID, EventPropID);
	for (unsigned int ValueID = 0; ValueID < nValues; ValueID++)
	{
		ValueDescription = pdu->H_GetChannelEventPulsePropertyValueDescription(ChannelID, EventPropID, ValueID);
	}
	nFlags = pdu->H_GetChannelEventPulsePropertyNFlags(ChannelID, EventPropID);
	for (unsigned int FlagID = 0; FlagID < nFlags; FlagID++)
	{
		FlagDescription = pdu->H_GetChannelEventPulsePropertyFlagDescription(ChannelID, EventPropID, FlagID);
		FlTyp = pdu->H_GetChannelEventPulsePropertyFlagType(ChannelID, EventPropID, FlagID);
	}
}

void ReadEventWaveformProperty(CPDU *pdu, unsigned int ChannelID, unsigned int EventPropID)
{
	unsigned int nSignals, nFlags, SignalSamplingRate;
	char *FlagDescr, *SignalDescr, *SignalUnit;
	CPDU::FlagType FlTyp;
	CPDU::RangeType SignalRangeType;
	REAL32 LowerBound32, UpperBound32;
	REAL64 LowerBound64, UpperBound64;
	int LowerBoundInt, UpperBoundInt;
	nFlags = pdu->H_GetChannelEventWaveformPropertyNFlags(ChannelID, EventPropID);
	for (unsigned int FlagID = 0; FlagID < nFlags; FlagID++)
	{
		FlagDescr = pdu->H_GetChannelEventWaveformPropertyFlagDescription(ChannelID, EventPropID, FlagID);
		FlTyp = pdu->H_GetChannelEventWaveformPropertyFlagType(ChannelID, EventPropID, FlagID);
	}
	nSignals = pdu->H_GetChannelEventWaveformNSignals(ChannelID, EventPropID);
	for (unsigned int SignalID = 0; SignalID < nSignals; SignalID++)
	{
		SignalDescr = pdu->H_ChannelEventWaveformGetSignalDescription(ChannelID, EventPropID, SignalID);
		if (pdu->H_ChannelEventWaveformSignalSamplingRatePresent(ChannelID, EventPropID, SignalID))
			SignalSamplingRate = pdu->H_ChannelEventWaveformGetSignalSamplingRate(ChannelID, EventPropID, SignalID);
		SignalUnit = pdu->H_ChannelEventWaveformGetSignalUnit(ChannelID, EventPropID, SignalID); // NULL if not present
		if (pdu->H_ChannelEventWaveformSignalRangePresent(ChannelID, EventPropID, SignalID))
		{
			SignalRangeType = pdu->H_ChannelEventWaveformGetSignalRangeType(ChannelID, EventPropID, SignalID);
			switch (SignalRangeType)
			{
			case CPDU::RangeType::rangeOfReals32_chosen:
				LowerBound32 = pdu->H_ChannelEventWaveformGetSignalRangeLowerboundREAL32(ChannelID, EventPropID, SignalID);
				UpperBound32 = pdu->H_ChannelEventWaveformGetSignalRangeUpperboundREAL32(ChannelID, EventPropID, SignalID);
				break;
			case CPDU::RangeType::rangeOfReals64_chosen:
				LowerBound64 = pdu->H_ChannelEventWaveformGetSignalRangeLowerboundREAL64(ChannelID, EventPropID, SignalID);
				UpperBound64 = pdu->H_ChannelEventWaveformGetSignalRangeUpperboundREAL64(ChannelID, EventPropID, SignalID);
				break;
			case CPDU::RangeType::rangeOfInts_chosen:
				LowerBoundInt = pdu->H_ChannelEventWaveformGetSignalRangeLowerboundInt(ChannelID, EventPropID, SignalID);
				UpperBoundInt = pdu->H_ChannelEventWaveformGetSignalRangeUpperboundInt(ChannelID, EventPropID, SignalID);
				break;
			default:
				break;
			}
		}
	}
}

void ReadEventRolloverProperty(CPDU *pdu, unsigned int ChannelID, unsigned int EventPropID)
{
	unsigned int RolloverTicks;
	RolloverTicks = pdu->H_ChannelEventRolloverGetRolloverticks(ChannelID, EventPropID);
}

void ReadEventLogicProperty(CPDU *pdu, unsigned int ChannelID, unsigned int EventPropID)
{
	unsigned int nFlags;
	char *FlagDescription;
	CPDU::FlagType FlTyp;
	nFlags = pdu->H_GetChannelEventLogicPropertyNFlags(ChannelID, EventPropID);
	for (unsigned int FlagID = 0; FlagID < nFlags; FlagID++)
	{
		FlagDescription = pdu->H_GetChannelEventLogicPropertyFlagDescription(ChannelID, EventPropID, FlagID);
		FlTyp = pdu->H_GetChannelEventLogicPropertyFlagType(ChannelID, EventPropID, FlagID);
	}
}

void ReadEventMeasurementsProperty(CPDU *pdu, unsigned int ChannelID, unsigned int EventPropID)
{
	unsigned int nMeasurements;
	char *MeasDescr, *MeasQty, *MeasUnit;
	CPDU::TypeOfAccuracy MeasAccType;
	CPDU::TypeOfNumeric MeasAccNumType;
	REAL32 MeasAcc32;
	REAL64 MeasAcc64;
	int MeasAccInt;
	nMeasurements = pdu->H_ChannelEventMeasurementGetNMeasurements(ChannelID, EventPropID);
	for (unsigned int MeasurementID = 0; MeasurementID < nMeasurements; MeasurementID++)
	{
		MeasDescr = pdu->H_ChannelEventMeasurementGetMeasurementDescription(ChannelID, EventPropID, MeasurementID);
		MeasQty = pdu->H_ChannelEventMeasurementGetMeasurementQuantity(ChannelID, EventPropID, MeasurementID);
		MeasUnit = pdu->H_ChannelEventMeasurementGetMeasurementUnit(ChannelID, EventPropID, MeasurementID);
		if (pdu->H_ChannelEventMeasurementAccuracyPresent(ChannelID, EventPropID, MeasurementID))
		{
			MeasAccNumType = pdu->H_GetChannelEventMeasurementTypeOfNumeric(ChannelID, EventPropID, MeasurementID);
			switch (MeasAccNumType)
			{
			case CPDU::TypeOfNumeric::real32_chosen:
				MeasAcc32 = pdu->H_GetChannelEventMeasurementAccuracyREAL32(ChannelID, EventPropID, MeasurementID);
				break;
			case CPDU::TypeOfNumeric::real64_chosen:
				MeasAcc64 = pdu->H_GetChannelEventMeasurementAccuracyREAL64(ChannelID, EventPropID, MeasurementID);
				break;
			case CPDU::TypeOfNumeric::Numeric_int_chosen:
				MeasAccInt = pdu->H_GetChannelEventMeasurementAccuracyInt(ChannelID, EventPropID, MeasurementID);
				break;
			default:
				break;
			}
		}
		if (pdu->H_ChannelEventMeasurementAccuracyTypePresent(ChannelID, EventPropID, MeasurementID))
		{
			MeasAccType = pdu->H_GetChannelEventMeasurementAccuracyType(ChannelID, EventPropID, MeasurementID);
		}
	}
}

void ReadEventHistogramsProperty(CPDU *pdu, unsigned int ChannelID, unsigned int EventPropID)
{
	unsigned int nHistograms, HistoNBins;
	char *HistoDescr, *QtyX, *QtyY, *UnitX, *UnitY;
	CPDU::TypeOfHistogram HistoType;
	CPDU::TypeOfAccumulation HistoAccumul;
	CPDU::RangeType RangeTypeX, RangeTypeY;
	REAL32 LowerBound32, UpperBound32;
	REAL64 LowerBound64, UpperBound64;
	int LowerBoundInt, UpperBoundInt;
	nHistograms = pdu->H_ChannelEventHistogramGetNHistograms(ChannelID, EventPropID);
	for (unsigned int HistogramID = 0; HistogramID < nHistograms; HistogramID++)
	{
		// strings are NULL when not present
		HistoDescr = pdu->H_ChannelEventHistogramGetHistogramDescription(ChannelID, EventPropID, HistogramID); 
		QtyX = pdu->H_ChannelEventHistogramGetHistogramQuantityX(ChannelID, EventPropID, HistogramID);
		QtyY = pdu->H_ChannelEventHistogramGetHistogramQuantityY(ChannelID, EventPropID, HistogramID);
		UnitX = pdu->H_ChannelEventHistogramGetHistogramUnitX(ChannelID, EventPropID, HistogramID);
		UnitY = pdu->H_ChannelEventHistogramGetHistogramUnitY(ChannelID, EventPropID, HistogramID);

		HistoType = pdu->H_ChannelEventHistogramGetHistogramType(ChannelID, EventPropID, HistogramID);

		HistoAccumul = pdu->H_ChannelEventHistogramGetHistogramAccumulation(ChannelID, EventPropID, HistogramID);

		HistoNBins  = pdu->H_ChannelEventHistogramGetHistogramnBins(ChannelID, EventPropID, HistogramID);

		if (pdu->H_ChannelEventHistogramRangeXPresent(ChannelID, EventPropID, HistogramID))
		{
			RangeTypeX = pdu->H_ChannelEventHistogramGetRangeXType(ChannelID, EventPropID, HistogramID);
			switch (RangeTypeX)
			{
			case CPDU::RangeType::rangeOfReals32_chosen:
				LowerBound32 = pdu->H_ChannelEventHistogramGetRangeXLowerboundREAL32(ChannelID, EventPropID, HistogramID);
				UpperBound32 = pdu->H_ChannelEventHistogramGetRangeXUpperboundREAL32(ChannelID, EventPropID, HistogramID);
				break;
			case CPDU::RangeType::rangeOfReals64_chosen:
				LowerBound64 = pdu->H_ChannelEventHistogramGetRangeXLowerboundREAL64(ChannelID, EventPropID, HistogramID);
				UpperBound64 = pdu->H_ChannelEventHistogramGetRangeXUpperboundREAL64(ChannelID, EventPropID, HistogramID);
				break;
			case CPDU::RangeType::rangeOfInts_chosen:
				LowerBoundInt = pdu->H_ChannelEventHistogramGetRangeXLowerboundInt(ChannelID, EventPropID, HistogramID);
				UpperBoundInt = pdu->H_ChannelEventHistogramGetRangeXUpperboundInt(ChannelID, EventPropID, HistogramID);
				break;
			default:
				break;
			}
		}

		if (pdu->H_ChannelEventHistogramRangeYPresent(ChannelID, EventPropID, HistogramID))
		{
			RangeTypeY = pdu->H_ChannelEventHistogramGetRangeYType(ChannelID, EventPropID, HistogramID);
			switch (RangeTypeY)
			{
			case CPDU::RangeType::rangeOfReals32_chosen:
				LowerBound32 = pdu->H_ChannelEventHistogramGetRangeYLowerboundREAL32(ChannelID, EventPropID, HistogramID);
				UpperBound32 = pdu->H_ChannelEventHistogramGetRangeYUpperboundREAL32(ChannelID, EventPropID, HistogramID);
				break;
			case CPDU::RangeType::rangeOfReals64_chosen:
				LowerBound64 = pdu->H_ChannelEventHistogramGetRangeYLowerboundREAL64(ChannelID, EventPropID, HistogramID);
				UpperBound64 = pdu->H_ChannelEventHistogramGetRangeYUpperboundREAL64(ChannelID, EventPropID, HistogramID);
				break;
			case CPDU::RangeType::rangeOfInts_chosen:
				LowerBoundInt = pdu->H_ChannelEventHistogramGetRangeYLowerboundInt(ChannelID, EventPropID, HistogramID);
				UpperBoundInt = pdu->H_ChannelEventHistogramGetRangeYUpperboundInt(ChannelID, EventPropID, HistogramID);
				break;
			default:
				break;
			}
		}
	}
}

void ReadHeader(CPDU *pdu) 
{
	// Read some header elements
	//IEC 62755

	// StandardID
	char* stID = pdu->H_GetStandardID();

	// Listmode data ID
	char* ListmodeDataID = pdu->H_GetListmodeDataID(); // NULL if not present

	// MeasSetupID
	char* MeasSetupID = pdu->H_GetMeasSetupID(); // NULL if not present

	// MeasSetupDescription
	char* MeasSetupDescr = pdu->H_GetMeasSetupDescription(); // NULL if not present

	// iec62755
	unsigned short relation = 0;
	unsigned char *IEC62755data = NULL;
	unsigned int datalength = 0;
	pdu->H_GetIec62755(relation, &IEC62755data, datalength);

	// radSource
	char* RadSource = pdu->H_GetRadSource();// NULL if not present
		
	// Start 
	char * Start = pdu->H_GetStartDateTime(); // NULL if not present
	REAL64 StartFracSec = pdu->H_GetStartFractionalSeconds();

	// startAccuracy
	REAL64 StartAcc;
	if (pdu->H_StartAccuracyPresent()) StartAcc = pdu->H_GetStartAccuracy();

	//delay
	int Delay;
	if (pdu->H_DelayPresent()) Delay = pdu->H_GetDelay();

	// devices
	char *DevName, *DevManuf, *DevModel, *DevSerial;
	unsigned int nDevices = pdu->H_GetNDevices(); // number of devices
	for (unsigned int DeviceID = 0; DeviceID < nDevices; DeviceID++)
	{
		DevName = pdu->H_GetDeviceName(DeviceID); // NULL if not present
		DevManuf = pdu->H_GetDeviceManuf(DeviceID); // NULL if not present
		DevModel = pdu->H_GetDeviceModel(DeviceID); // NULL if not present
		DevSerial = pdu->H_GetDeviceSerial(DeviceID); // NULL if not present
	}
	
	// channels
	int ChannelDeviceID;
	CPDU::ChannelType ChType;
	unsigned int PhysicalChannel, ADCSamplingRate, ADCBitResolution, TicksPerSecond;
	char *ChName, *ChDescription, *ChParameters, *ChRefClock, *ChEventPropDescr;
	CPDU::ChannelSyncStatus ChSyncStatus;
	unsigned int nChEventProps;
	CPDU::ChannelEventPropertyType ChEventPropType;
	unsigned int nChannels = pdu->H_GetNChannels(); // number of channels
	for (unsigned int ChannelID = 0; ChannelID < nChannels; ChannelID++)
	{
		ChannelDeviceID = pdu->H_GetChannelDeviceID(ChannelID); // The Device to which the channel belongs
		ChType = pdu->H_GetChannelType(ChannelID);
		if (pdu->H_PhysicalChannelPresent(ChannelID))
			PhysicalChannel = pdu->H_GetPhysicalChannel(ChannelID);
		ChName = pdu->H_GetChannelName(ChannelID); // NULL if not present
		ChDescription = pdu->H_GetChannelDescription(ChannelID);// NULL if not present
		ChParameters = pdu->H_GetChannelParameters(ChannelID);// NULL if not present
		ChRefClock = pdu->H_GetChannelRefClock(ChannelID);// NULL if not present
		if (pdu->H_ChannelSyncStatusPresent(ChannelID))
			ChSyncStatus = pdu->H_GetChannelSyncStatus(ChannelID);
		ADCSamplingRate = pdu->H_GetChannelADCSamplingRate(ChannelID);
		ADCBitResolution = pdu->H_GetChannelADCBitResolution(ChannelID);
		TicksPerSecond = pdu->H_GetChannelTicksPerSecond(ChannelID);

		// Channel event properties
		nChEventProps = pdu->H_GetChannelNEventProperties(ChannelID); // number of event properties
		for (unsigned int EventPropID = 0; EventPropID < nChEventProps; EventPropID++)
		{
			ChEventPropType = pdu->H_GetChannelEventPropertyType(ChannelID, EventPropID);
			ChEventPropDescr = pdu->H_GetChannelEventPropertyDescription(ChannelID, EventPropID);

			switch (ChEventPropType)
			{
			case CPDU::ChannelEventPropertyType::eventpulse:
				ReadEventPulseProperty(pdu, ChannelID, EventPropID);
				break;
			case CPDU::ChannelEventPropertyType::eventwaveform:
				ReadEventWaveformProperty(pdu, ChannelID, EventPropID);
				break;
			case CPDU::ChannelEventPropertyType::eventrollover:
				ReadEventRolloverProperty(pdu, ChannelID, EventPropID);
				break;
			case CPDU::ChannelEventPropertyType::eventtime:
				break;
			case CPDU::ChannelEventPropertyType::eventRTC:
				break;
			case CPDU::ChannelEventPropertyType::eventlogic:
				ReadEventLogicProperty(pdu, ChannelID, EventPropID);
				break;
			case CPDU::ChannelEventPropertyType::eventgeo:
				break;
			case CPDU::ChannelEventPropertyType::eventmeasurements:
				ReadEventMeasurementsProperty(pdu, ChannelID, EventPropID);
				break;
			case CPDU::ChannelEventPropertyType::eventhistograms:
				ReadEventHistogramsProperty(pdu, ChannelID, EventPropID);
				break;
			case CPDU::ChannelEventPropertyType::eventmessage:
				break;
			default:
				break;
			}
		}
	}
}

void ReadEventPulse(CPDU *pdu)
{
	// Read an event of the pulse type
	
	unsigned int channelid;
	TimeStamp timestamp;
	unsigned int _nFlags = 0, _nValues = 0, _ValueID;
	unsigned char *_FlagsBitstring = NULL;
	CPDU::TypeOfNumeric _typeofnumeric;
	REAL32 _real32Value;
	REAL64 _real64Value;
	int _intValue;

	channelid = pdu->E_GetEventPulseChannelID();
	if (pdu->E_EventPulseTimeStampPresent())
		timestamp = pdu->E_GetEventPulseTimeStamp();

	pdu->E_GetEventPulseFlags( _nFlags, &_FlagsBitstring);

	_nValues = pdu->E_GetEventPulseNValues();
	for (_ValueID = 0; _ValueID < _nValues; _ValueID++)
	{
		_typeofnumeric = pdu->E_GetEventPulseTypeOfNumericValue( _ValueID);
		switch (_typeofnumeric)
		{
		case CPDU::TypeOfNumeric::real32_chosen:
			_real32Value = pdu->E_GetEventPulseValueREAL32( _ValueID);
			break;
		case CPDU::TypeOfNumeric::real64_chosen:
			_real64Value = pdu->E_GetEventPulseValueREAL64( _ValueID);
			break;
		case CPDU::TypeOfNumeric::Numeric_int_chosen:
			_intValue = pdu->E_GetEventPulseValueInt( _ValueID);
			break;
		}
	}
}

void ReadEventWaveform(CPDU *pdu)
{
	TimeStamp timestamp;
	unsigned int channelid, _SamplingRate;
	unsigned int _nFlags = 0, _nSignals = 0, _SignalID;
	unsigned char *_FlagsBitstring = NULL;
	REAL32 _lower32 = 0.0, _upper32 = 0.0;
	REAL64 _lower64 = 0.0, _upper64 = 0.0;
	int _lowerint = 0, _upperint = 0;
	CPDU::RangeType signalrangetype;
	REAL32 *signalsamples32;
	REAL64 *signalsamples64;
	int *signalsamplesint;
	unsigned int signalnsamples;

	channelid = pdu->E_GetEventWaveformChannelID();
	if (pdu->E_EventWaveformTimeStampPresent())
		timestamp = pdu->E_GetEventWaveformTimeStamp();

	pdu->E_GetEventWaveformFlags( _nFlags, &_FlagsBitstring);

	_nSignals = pdu->E_GetEventWaveformNSignals();
	for (_SignalID = 0; _SignalID < _nSignals; _SignalID++)
	{
		// Get sampling rate
		if (pdu->E_EventWaveformSignalSamplingRatePresent(_SignalID))
			_SamplingRate = pdu->E_GetEventWaveformSignalSamplingRate(_SignalID);

		// Get singal range
		if (pdu->E_EventWaveformSignalRangePresent(_SignalID))
		{
			signalrangetype = pdu->E_GetEventWaveformSignalRangeType(_SignalID);
			switch (signalrangetype)
			{
			case CPDU::RangeType::rangeOfReals32_chosen:
				pdu->E_GetEventWaveformSignalRangeREAL32(_SignalID, _lower32, _upper32);
				break;
			case CPDU::RangeType::rangeOfReals64_chosen:
				pdu->E_GetEventWaveformSignalRangeREAL64(_SignalID, _lower64, _upper64);
				break;
			case CPDU::RangeType::rangeOfInts_chosen:
				pdu->E_GetEventWaveformSignalRangeInt(_SignalID, _lowerint, _upperint);
				break;
			}
		}

		// Get signal samples
		signalnsamples = pdu->E_GetEventWaveformSignalNSamples(_SignalID);
		switch (pdu->E_GetEventWaveformSignalSampleType(_SignalID))
		{
		case CPDU::TypeOfNumeric::real32_chosen:
			signalsamples32 = new REAL32[signalnsamples];
			pdu->E_GetEventWaveformSignalSamplesREAL32(_SignalID, signalsamples32);
			delete[]signalsamples32;
			break;
		case CPDU::TypeOfNumeric::real64_chosen:
			signalsamples64 = new REAL64[signalnsamples];
			pdu->E_GetEventWaveformSignalSamplesREAL64(_SignalID, signalsamples64);
			delete[]signalsamples64;
			break;
		case CPDU::TypeOfNumeric::Numeric_int_chosen:
			signalsamplesint = new int[signalnsamples];
			pdu->E_GetEventWaveformSignalSamplesInt(_SignalID, signalsamplesint);
			delete[]signalsamplesint;
			break;
		}
	}
}

void ReadEventRollover(CPDU *pdu)
{
	unsigned int _Rollovers = 0;
	unsigned int _ChannelID;
	pdu->E_GetEventRollover(_ChannelID, _Rollovers);
}

void ReadEventTime(CPDU *pdu)
{
	unsigned int _ChannelID;
	unsigned int _TotalDeadTimeSinceStart = 0, _TotalLiveTimeSinceStart = 0;
	unsigned int _TotalDeadTimeSincePrevious = 0, _TotalLiveTimeSincePrevious = 0;
	bool _SinceStart = false, _SincePrevious = false;
	TimeStamp _TimeStamp;

	pdu->E_GetEventTime(_ChannelID, _TimeStamp, _SinceStart, _TotalDeadTimeSinceStart, _TotalLiveTimeSinceStart,
		_SincePrevious, _TotalDeadTimeSincePrevious, _TotalLiveTimeSincePrevious);
}

void ReadEventRTC(CPDU *pdu)
{
	unsigned int _ChannelID;
	TimeStamp _TimeStamp;
	char *_RealTimeClock = NULL;
	REAL64 _FractionalSeconds = 0.0;
	CPDU::ChannelSyncStatus _SyncStatus;

	pdu->E_GetEventRTC(_ChannelID, _TimeStamp, &_RealTimeClock, _FractionalSeconds, _SyncStatus);
}

void ReadEventLogic(CPDU *pdu)
{
	unsigned int _ChannelID;
	TimeStamp _TimeStamp;
	unsigned int _nFlags = 0;
	unsigned char *_FlagsBitstring = NULL;
	pdu->E_GetEventLogic(_ChannelID, _TimeStamp, _nFlags, &_FlagsBitstring);
}

void ReadEventGeo(CPDU *pdu)
{
	REAL64 latitude, longitude, elevation, elevationoffset, geopointaccuracy, elevationaccuracy, offsetaccuracy;
	REAL64 speed, azimuth, inclination, distance, originlatitude, originlongitude;
	REAL64 relativeelevation, relativeelevationoffset, relativegeopointaccuracy, relativeelevationaccuracy, relativeelevationoffsetaccuracy;
	REAL64 orientationazimuth, orientationinclination, orientationroll;
	unsigned char *datum, *locationdescription, *origindescription, *relativedatum;
	unsigned int ChannelID;
	TimeStamp timestamp;
	bool positionpresent, timestamppresent, orientationpresent, speedpresent;

	ChannelID = pdu->E_GetEventGeoChannelID();

	timestamppresent = pdu->E_EventGeoTimeStampPresent();
	if (timestamppresent) timestamp = pdu->E_GetEventGeoTimeStamp();
	positionpresent = pdu->E_EventGeoPositionPresent();
	if (positionpresent)
	{
		CPDU::TypeOfGeoPosition positiontype = pdu->E_GetEventGeoPositionType();
		switch (positiontype)
		{
		case CPDU::TypeOfGeoPosition::geographicPoint_chosen:
			latitude = pdu->E_GetEventGeoPositionAsGeographicPointLatitude();
			longitude = pdu->E_GetEventGeoPositionAsGeographicPointLongitude();
			if (pdu->E_EventGeoPositionAsGeographicPointElevationPresent())
				elevation = pdu->E_GetEventGeoPositionAsGeographicPointElevation();
			if (pdu->E_EventGeoPositionAsGeographicPointElevationOffsetPresent())
				elevationoffset = pdu->E_GetEventGeoPositionAsGeographicPointElevationOffset();
			if (pdu->E_EventGeoPositionAsGeographicPointGeoPointAccuracyPresent())
				geopointaccuracy = pdu->E_GetEventGeoPositionAsGeographicPointGeoPointAccuracy();
			if (pdu->E_EventGeoPositionAsGeographicPointElevationAccuracyPresent())
				elevationaccuracy = pdu->E_GetEventGeoPositionAsGeographicPointElevationAccuracy();
			if (pdu->E_EventGeoPositionAsGeographicPointElevationOffsetAccuracyPresent())
				offsetaccuracy = pdu->E_GetEventGeoPositionAsGeographicPointElevationOffsetAccuracy();
			if (pdu->E_EventGeoPositionAsGeographicPointDatumPresent())
				datum = pdu->E_SetEventGeoPositionAsGeographicPointDatum();
			break;
		case CPDU::TypeOfGeoPosition::locationDescription_chosen:
			locationdescription = pdu->E_GetEventGeoPositionLocationDescription();
			break;
		case CPDU::TypeOfGeoPosition::relativeLocation_chosen:
			if (pdu->E_EventGeoPositionAsRelativeLocationAzimuthPresent())
				azimuth = pdu->E_GetEventGeoPositionAsRelativeLocationAzimuth();
			if (pdu->E_EventGeoPositionAsRelativeLocationInclinationPresent())
				inclination = pdu->E_GetEventGeoPositionAsRelativeLocationInclination();
			if (pdu->E_EventGeoPositionAsRelativeLocationDistancePresent())
				distance = pdu->E_GetEventGeoPositionAsRelativeLocationDistance();
			if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsDescriptionPresent())
				origindescription = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsDescription();
			if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointPresent())
			{
				originlatitude = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointLatidude();
				originlongitude = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointLongitude();

				if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationPresent())
					relativeelevation = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevation();
				if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffsetPresent())
					relativeelevationoffset = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffset();
				if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointGeoPointAccuracyPresent())
					relativegeopointaccuracy = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointGeoPointAccuracy();
				if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationAccuracyPresent())
					relativeelevationaccuracy = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationAccuracy();
				if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffsetAccuracyPresent())
					relativeelevationoffsetaccuracy = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffsetAccuracy();
				if (pdu->E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointDatumPresent())
					relativedatum = pdu->E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointDatum();
			}
			break;
		}
	}
	orientationpresent = pdu->E_EventGeoOrientationPresent();
	if (orientationpresent)
	{
		if (pdu->E_EventGeoOrientationAzimuthPresent())
			orientationazimuth = pdu->E_GetEventGeoOrientationAzimuth();
		if (pdu->E_EventGeoOrientationInclinationPresent())
			orientationinclination = pdu->E_GetEventGeoOrientationInclination();
		if (pdu->E_EventGeoOrientationRollPresent())
			orientationroll = pdu->E_GetEventGeoOrientationRoll();
	}
	speedpresent = pdu->E_EventGeoSpeedPresent();
	if (speedpresent)
	{
		speed = pdu->E_GetEventGeoSpeed();
	}
}

void ReadEventMeasurements(CPDU *pdu)
{
	unsigned int nMeasurements;
	unsigned int ChannelID;
	TimeStamp timestamp;
	bool accuracypresent;
	REAL32 Value32, Accuracy32;
	REAL64 Value64, Accuracy64;
	int ValueInt, AccuracyInt;

	ChannelID = pdu->E_GetEventMeasurementsChannelID();
	if (pdu->E_EventMeasurementsTimestampPresent())
		timestamp = pdu->E_GetEventMeasurementsTimestamp();
	nMeasurements = pdu->E_GetEventMeasurementsNMeasurements();
	for (unsigned int _MeasurementID = 0; _MeasurementID < nMeasurements; _MeasurementID++)
	{
		accuracypresent = pdu->E_EventMeasurementAccuracyPresent(_MeasurementID);

		switch (pdu->E_GetEventMeasurementTypeOfNumericValue(_MeasurementID))
		{
		case CPDU::TypeOfNumeric::real32_chosen:
			pdu->E_GetEventMeasurementValueREAL32(_MeasurementID, Value32, Accuracy32);
			break;
		case CPDU::TypeOfNumeric::real64_chosen:
			pdu->E_GetEventMeasurementValueREAL64(_MeasurementID, Value64, Accuracy64);
			break;
		case CPDU::TypeOfNumeric::Numeric_int_chosen:
			pdu->E_GetEventMeasurementValueInt(_MeasurementID, ValueInt, AccuracyInt);
			break;
		}
	}
}

void ReadEventHistograms(CPDU *pdu)
{
	unsigned int ChannelID;
	TimeStamp timestamp;
	REAL32 *histo32;
	REAL64 *histo64;
	int *histoint;
	unsigned int *binnumbers;

	ChannelID = pdu->E_GetEventHistogramsChannelID();
	if (pdu->E_EventHistogramsTimestampPresent())
		timestamp = pdu->E_GetEventHistogramsTimestamp();


	unsigned int nHistograms = pdu->E_GetEventHistogramsNHistograms();
	for (unsigned int _HistogramID = 0; _HistogramID < nHistograms; _HistogramID++)
	{
		CPDU::TypeOfNumeric typeofhistogramnumeric = pdu->E_GetEventHistogramTypeOfNumeric( _HistogramID);
		unsigned int nBins = pdu->E_GetEventHistogramsNBins(_HistogramID);

		if (pdu->E_EventHistogramsAllBinsChosen( _HistogramID))
		{
			// all bins
			switch (typeofhistogramnumeric)
			{
			case CPDU::real32_chosen:
				histo32 = new REAL32[nBins];
				pdu->E_GetEventHistogramsAllBinsREAL32(_HistogramID, histo32);
				delete[]histo32;
				break;
			case CPDU::real64_chosen:
				histo64 = new REAL64[nBins];
				pdu->E_GetEventHistogramsAllBinsREAL64(_HistogramID, histo64);
				delete[]histo64;
				break;
			case CPDU::Numeric_int_chosen:
				histoint = new int[nBins];
				pdu->E_GetEventHistogramsAllBinsInt(_HistogramID, histoint);
				delete[]histoint;
				break;
			}
		}
		else
		{
			// bin list
			binnumbers = new unsigned int[nBins];
			switch (typeofhistogramnumeric)
			{
			case CPDU::real32_chosen:
				histo32 = new REAL32[nBins];
				pdu->E_GetEventHistogramsBinListREAL32( _HistogramID, binnumbers, histo32);
				delete[]histo32;
				break;
			case CPDU::real64_chosen:
				histo64 = new REAL64[nBins];
				pdu->E_GetEventHistogramsBinListREAL64(_HistogramID, binnumbers, histo64);
				delete[]histo64;
				break;
			case CPDU::Numeric_int_chosen:
				histoint = new int[nBins];
				pdu->E_GetEventHistogramsBinListInt(_HistogramID, binnumbers, histoint);
				delete[]histoint;
				break;
			}
			delete[]binnumbers;
		}

	}
}

void ReadEventMessage(CPDU *pdu)
{
	unsigned int ChannelID;
	TimeStamp timestamp;
	unsigned char *_Message = NULL;

	pdu->E_GetEventMessage(ChannelID, timestamp, &_Message);
}

void ReadEvents(CPDU *pdu)
{
	// Read some data from events
	char * eventsDataID = pdu->E_GetListmodeDataID();
	unsigned int id = pdu->E_GetEventsID();

	pdu->PointAtFirstEvent();
	CPDU::TypeOfEvent evtp;
	evtp = pdu->E_GetEventType();
	while (!pdu->EndOfEvents()) // (evtp != CPDU::TypeOfEvent::eventNone)
	{
		switch (evtp)
		{
		case CPDU::eventPulse_chosen:
			ReadEventPulse(pdu);
			break;
		case CPDU::eventWaveform_chosen:
			ReadEventWaveform(pdu);
			break;
		case CPDU::eventRollover_chosen:
			ReadEventRollover(pdu);
			break;
		case CPDU::eventTime_chosen:
			ReadEventTime(pdu);
			break;
		case CPDU::eventRTC_chosen:
			ReadEventRTC(pdu);
			break;
		case CPDU::eventLogic_chosen:
			ReadEventLogic(pdu);
			break;
		case CPDU::eventGeo_chosen:
			ReadEventGeo(pdu);
			break;
		case CPDU::eventMeasurements_chosen:
			ReadEventMeasurements(pdu);
			break;
		case CPDU::eventHistograms_chosen:
			ReadEventHistograms(pdu);
			break;
		case CPDU::eventMessage_chosen:
			ReadEventMessage(pdu);
			break;
		default:
			break;
		};
		pdu->PointAtNextEvent();
		evtp = pdu->E_GetEventType();
	}
}

void ReadFooter(CPDU *pdu)
{
	// Read some data from footer	
	char* st = pdu->F_GetStopDateTime();
	REAL64 stf = pdu->F_GetStopFractionalSeconds();
	unsigned int dt, lt;
	if (pdu->F_TotalDeadTimePresent())
	{
		dt = pdu->F_GetTotalDeadTime();
	}
	if (pdu->F_TotalLiveTimePresent())
	{
		lt = pdu->F_GetTotalLiveTime();
	}
}

void ReadListmodedataFile(const char *filename, CPDU::EncodingRules EncodingRule)
{
	// Read a listmode data file and decode the messages
	int retcode;
	unsigned int nMessages = 0;
	CCodec *codec = new CCodec(); // Declare and init an encoding object
	CPDU *pdu = new CPDU();

	// Read the file with encoded data
	// The file contains more than one pdu encoding.
	StartTiming();
	retcode = pdu->ReadCompleteEncodedDataFile(filename);
	StopTiming("Reading encoded data file");

	while (pdu->GetEncodedDataLength() > 0)
	{
		StartTiming();
		pdu->Decode(EncodingRule);
		StopTiming("Decoding");

		// process decoded pdu
		//printf("Message %u:\n", nMessages);

		pdu->PrintStructuredData(); // comment-out if printing is not wanted

		switch (pdu->GetListmodedataType())
		{
		case CPDU::header_chosen:
			StartTiming();
			ReadHeader(pdu);
			StopTiming("Reading header");
			break;
		case CPDU::events_chosen:
			StartTiming();
			ReadEvents(pdu);
			StopTiming("Reading events");
			break;
		case CPDU::footer_chosen:
			StartTiming();
			ReadFooter(pdu);
			StopTiming("Reading footer");
			break;
		}
		// Clear data
		StartTiming();
		pdu->ClearStructuredData();
		StopTiming("Clearing structureddata");

		// Next
		StartTiming();
		pdu->PrepareforNextDecoding();
		StopTiming("Preparing for next decoding");
		nMessages++;
	}

	// clean up
	delete pdu;
	delete codec;
}

void SetHeader(CPDU *pdu)
{
	// populates header elements

	pdu->SetListmodedataType(CPDU::header_chosen);

	pdu->H_SetListmodeDataID("Listmode data ID=123456");

	pdu->H_SetStart("2010-11-15T13:05:00Z", 0.5);
	pdu->H_SetDelay(155155);

	pdu->H_AddDevices(2);

	pdu->H_SetDeviceName(0, "DeviceID=0 Name");
	pdu->H_SetDeviceManuf(0, "DeviceID=0 Manuf");
	pdu->H_SetDeviceModel(0, "DeviceID=0 Model");
	pdu->H_SetDeviceSerial(0, "DeviceID=0 Serial");

	pdu->H_SetDeviceName(1, "DeviceID=1 Name");
	pdu->H_SetDeviceManuf(1, "DeviceID=1 Manuf");
	pdu->H_SetDeviceModel(1, "DeviceID=1 Model");
	pdu->H_SetDeviceSerial(1, "DeviceID=1 Serial");

	pdu->H_AddChannels(4);
	pdu->H_SetChannelDeviceID(0, 0); // Channel 0 belongs to Device 0
	pdu->H_SetChannelDeviceID(1, 0); // Channel 1 belongs to Device 0
	pdu->H_SetChannelDeviceID(2, 1); // Channel 2 belongs to Device 1
	pdu->H_SetChannelDeviceID(3, 1); // Channel 3 belongs to Device 1
	pdu->H_SetChannelType(0, CPDU::physicaltype);
	pdu->H_SetChannelType(1, CPDU::virtualtype);
	pdu->H_SetChannelType(2, CPDU::physicaltype);
	pdu->H_SetChannelType(3, CPDU::virtualtype);
	pdu->H_SetPhysicalChannel(0, 100);
	pdu->H_SetPhysicalChannel(1, 101);
	pdu->H_SetPhysicalChannel(2, 102);
	pdu->H_SetPhysicalChannel(3, 103);
	pdu->H_ResetPhysicalChannel(3);

	pdu->H_SetChannelName(0, "ChannelID=0 Name");
	pdu->H_SetChannelName(1, "ChannelID=1 Name");
	pdu->H_SetChannelName(2, "ChannelID=2 Name");
	pdu->H_SetChannelName(3, "ChannelID=3 Name");
	pdu->H_ResetChannelName(3);

	pdu->H_SetChannelDescription(0, "ChannelID=0 Description");
	pdu->H_SetChannelDescription(1, "ChannelID=1 Description");
	pdu->H_SetChannelDescription(2, "ChannelID=2 Description");
	pdu->H_SetChannelDescription(3, "ChannelID=3 Description");
	pdu->H_ResetChannelDescription(2);

	pdu->H_SetChannelParameters(0, "ChannelID=0 Parameters");
	pdu->H_SetChannelParameters(1, "ChannelID=1 Parameters");
	pdu->H_SetChannelParameters(2, "ChannelID=2 Parameters");
	pdu->H_SetChannelParameters(3, "ChannelID=3 Parameters");
	pdu->H_ResetChannelDescription(1);

	pdu->H_SetChannelRefClock(0, "ChannelID=0 RefClock");
	pdu->H_SetChannelRefClock(1, "ChannelID=1 RefClock");
	pdu->H_SetChannelRefClock(2, "ChannelID=2 RefClock");
	pdu->H_SetChannelRefClock(3, "ChannelID=3 RefClock");
	pdu->H_ResetChannelDescription(2);

	pdu->H_SetChannelSyncStatus(0, CPDU::synchronised);
	pdu->H_SetChannelSyncStatus(1, CPDU::unsynchronised);
	pdu->H_SetChannelSyncStatus(2, CPDU::unsynchronised);
	pdu->H_ResetChannelSyncStatus(2);

	pdu->H_SetChannelADCSamplingRate(0, 100000);
	pdu->H_SetChannelADCSamplingRate(1, 110000);
	pdu->H_SetChannelADCSamplingRate(2, 120000);
	pdu->H_SetChannelADCSamplingRate(3, 130000);

	pdu->H_SetChannelADCBitResolution(0, 10);
	pdu->H_SetChannelADCBitResolution(1, 12);
	pdu->H_SetChannelADCBitResolution(2, 14);
	pdu->H_SetChannelADCBitResolution(3, 16);

	pdu->H_SetChannelTicksPerSecond(0, 1000000);
	pdu->H_SetChannelTicksPerSecond(1, 1000000);
	pdu->H_SetChannelTicksPerSecond(2, 2000000);
	pdu->H_SetChannelTicksPerSecond(3, 2000000);

	////////////////////////////////
	// channel 0 event properties //
	////////////////////////////////
	pdu->H_AddChannelEventProperties(0, 5); // adds 5 event properties to channel 0

	// channel 0 event property 0
	pdu->H_SetChannelEventPropertyType(0, 0, CPDU::eventpulse); // set type
	pdu->H_SetChannelEventPropertyDescription(0, 0, "ChannelID=0 EventPropertyID=0 Description");
	pdu->H_AddChannelEventPulsePropertyValues(0, 0, 2); // channel 0, event property 0 has 2 values
	pdu->H_SetChannelEventPulsePropertyValueDescription(0, 0, 0, "ChannelID=0 EventPropertyID=0 Description of value 0");
	pdu->H_SetChannelEventPulsePropertyValueDescription(0, 0, 1, "ChannelID=0 EventPropertyID=0 Description of value 1");
	pdu->H_AddChannelEventPulsePropertyFlags(0, 0, 4); // channel 0, event property 0 has 4 flags
	pdu->H_SetChannelEventPulsePropertyFlag(0, 0, 0, "ChannelID=0 EventPropertyID=0 Description of flag 0", CPDU::logicstate);
	pdu->H_SetChannelEventPulsePropertyFlag(0, 0, 1, "ChannelID=0 EventPropertyID=0 Description of flag 1", CPDU::logicstate);
	pdu->H_SetChannelEventPulsePropertyFlag(0, 0, 2, "ChannelID=0 EventPropertyID=0 Description of flag 2", CPDU::logictransition);
	pdu->H_SetChannelEventPulsePropertyFlag(0, 0, 3, "ChannelID=0 EventPropertyID=0 Description of flag 3", CPDU::logictransition);

	// channel 0 event property 1
	pdu->H_SetChannelEventPropertyType(0, 1, CPDU::eventwaveform); // set type
	pdu->H_SetChannelEventPropertyDescription(0, 1, "ChannelID=0 EventPropertyID=1 Description");
	pdu->H_AddChannelEventWaveformPropertyFlags(0, 1, 3); // channel 0, event property 1 has 3 flags
	pdu->H_SetChannelEventWaveformPropertyFlag(0, 1, 0, "ChannelID=0 EventPropertyID=1 Description of flag 0", CPDU::logictransition);
	pdu->H_SetChannelEventWaveformPropertyFlag(0, 1, 1, "ChannelID=0 EventPropertyID=1 Description of flag 1", CPDU::logicstate);
	pdu->H_SetChannelEventWaveformPropertyFlag(0, 1, 2, "ChannelID=0 EventPropertyID=1 Description of flag 2", CPDU::logicstate);
	pdu->H_AddChannelEventWaveformSignalProperties(0, 1, 3); // channel 0, event property 1 has 3 waveform signals
	pdu->H_ChannelEventWaveformSetSignalDescription(0, 1, 0, "ChannelID=0 EventPropertyID=1 SignalID=0 Description");
	pdu->H_ChannelEventWaveformSetSignalSamplingRate(0, 1, 0, 123123); // ChannelID=0 EventPropertyID=1 SignalID=0 sampling rate
	pdu->H_ChannelEventWaveformSetSignalUnit(0, 1, 0, "V"); // ChannelID=0 EventPropertyID=1 SignalID=0 unit
	pdu->H_ChannelEventWaveformSetSignalRangeREAL32(0, 1, 0, (REAL32)-1.0, (REAL32)+1.0); // ChannelID=0 EventPropertyID=1 SignalID=0 range
	pdu->H_ChannelEventWaveformSetSignalDescription(0, 1, 1, "ChannelID=0 EventPropertyID=1 SignalID=1 Description");
	pdu->H_ChannelEventWaveformSetSignalSamplingRate(0, 1, 1, 456456);// ChannelID=0 EventPropertyID=1 SignalID=1 sampling rate
	pdu->H_ChannelEventWaveformSetSignalUnit(0, 1, 1, "mV"); // ChannelID=0 EventPropertyID=1 SignalID=1 unit
	pdu->H_ChannelEventWaveformSetSignalRangeREAL64(0, 1, 1, (REAL64)-1.1, (REAL64)+1.1); // ChannelID=0 EventPropertyID=1 SignalID=1 range
	pdu->H_ChannelEventWaveformSetSignalDescription(0, 1, 2, "ChannelID=0 EventPropertyID=1 SignalID=2 Description");
	pdu->H_ChannelEventWaveformSetSignalSamplingRate(0, 1, 2, 789789);// ChannelID=0 EventPropertyID=1 SignalID=1 sampling rate
	pdu->H_ChannelEventWaveformSetSignalUnit(0, 1, 2, "kV"); // ChannelID=0 EventPropertyID=1 SignalID=1 unit
	pdu->H_ChannelEventWaveformSetSignalRangeInt(0, 1, 2, (int)-2, (int)2); // ChannelID=0 EventPropertyID=1 SignalID=2 range

	// channel 0 event property 2
	pdu->H_SetChannelEventPropertyType(0, 2, CPDU::eventtime); // set type
	pdu->H_SetChannelEventPropertyDescription(0, 2, "ChannelID=0 EventPropertyID=2 Description");

	// channel 0 event property 3
	pdu->H_SetChannelEventPropertyType(0, 3, CPDU::eventrollover); // set type
	pdu->H_SetChannelEventPropertyDescription(0, 3, "ChannelID=0 EventPropertyID=3 Description");
	pdu->H_ChannelEventRolloverSetRolloverticks(0, 3, 1024);

	// channel 0 event property 4
	pdu->H_SetChannelEventPropertyType(0, 4, CPDU::eventRTC); // set type
	pdu->H_SetChannelEventPropertyDescription(0, 4, "ChannelID=0 EventPropertyID=4 Description");

	////////////////////////////////
	// channel 1 event properties //
	////////////////////////////////
	pdu->H_AddChannelEventProperties(1, 4); // adds 4 event properties to channel 1

	// channel 1 event property 0
	pdu->H_SetChannelEventPropertyType(1, 0, CPDU::eventlogic); // set type
	pdu->H_SetChannelEventPropertyDescription(1, 0, "ChannelID=1 EventPropertyID=0 Description");
	pdu->H_AddChannelEventLogicPropertyFlags(1, 0, 3); // add 3 flags
	pdu->H_SetChannelEventLogicPropertyFlag(1, 0, 0, "ChannelID=1 EventPropertyID=0 FlagID=0 Description", CPDU::logictransition);
	pdu->H_SetChannelEventLogicPropertyFlag(1, 0, 1, "ChannelID=1 EventPropertyID=0 FlagID=1 Description", CPDU::logictransition);
	pdu->H_SetChannelEventLogicPropertyFlag(1, 0, 2, "ChannelID=1 EventPropertyID=0 FlagID=2 Description", CPDU::logicstate);

	// channel 1 event property 1
	pdu->H_SetChannelEventPropertyType(1, 1, CPDU::eventgeo); // set type
	pdu->H_SetChannelEventPropertyDescription(1, 1, "ChannelID=1 EventPropertyID=1 Description");

	// channel 1 event property 2
	pdu->H_SetChannelEventPropertyType(1, 2, CPDU::eventmessage); // set type
	pdu->H_SetChannelEventPropertyDescription(1, 2, "ChannelID=1 EventPropertyID=2 Description");

	// channel 1 event property 3
	pdu->H_SetChannelEventPropertyType(1, 3, CPDU::eventmeasurements); // set type
	pdu->H_SetChannelEventPropertyDescription(1, 3, "ChannelID=1 EventPropertyID=3 Description");
	pdu->H_AddChannelEventMeasurementProperties(1, 3, 3); // adds 3 measurements
	pdu->H_ChannelEventMeasurementSetMeasurementDescription(1, 3, 0, "ChannelID=1 EventPropertyID=3 MeasurementID=0 Description");
	pdu->H_ChannelEventMeasurementSetMeasurementDescription(1, 3, 1, "ChannelID=1 EventPropertyID=3 MeasurementID=1 Description");
	pdu->H_ChannelEventMeasurementSetMeasurementDescription(1, 3, 2, "ChannelID=1 EventPropertyID=3 MeasurementID=2 Description");
	pdu->H_ChannelEventMeasurementSetMeasurementQuantity(1, 3, 0, "ChannelID=1 EventPropertyID=3 MeasurementID=0 Quantity");
	pdu->H_ChannelEventMeasurementSetMeasurementUnit(1, 3, 0, "ChannelID=1 EventPropertyID=3 MeasurementID=0 Unit");
	pdu->H_SetChannelEventMeasurementAccuracyREAL32(1, 3, 0, 0.23);
	pdu->H_SetChannelEventMeasurementAccuracyREAL64(1, 3, 1, 0.23456);
	pdu->H_SetChannelEventMeasurementAccuracyInt(1, 3, 2, 56);
	pdu->H_SetChannelEventMeasurementAccuracyType(1, 3, 1, CPDU::absolute);

	////////////////////////////////
	// channel 2 event properties //
	////////////////////////////////
	pdu->H_AddChannelEventProperties(2, 1); // adds 1 event properties to channel 2
	// channel 2 event property 0
	pdu->H_SetChannelEventPropertyType(2, 0, CPDU::eventhistograms); // set type
	pdu->H_SetChannelEventPropertyDescription(2, 0, "ChannelID=2 EventPropertyID=0 Description");
	pdu->H_AddChannelEventHistogramProperties(2, 0, 3); // 3 histograms
	pdu->H_ChannelEventHistogramSetHistogramDescription(2, 0, 0, "ChannelID=2 EventPropertyID=0 HistogramID=0 Description");
	pdu->H_ChannelEventHistogramSetHistogramDescription(2, 0, 1, "ChannelID=2 EventPropertyID=0 HistogramID=1 Description");
	pdu->H_ChannelEventHistogramSetHistogramDescription(2, 0, 2, "ChannelID=2 EventPropertyID=0 HistogramID=1 Description");
	pdu->H_ChannelEventHistogramSetHistogramQuantityX(2, 0, 0, "ChannelID=2 EventPropertyID=0 HistogramID=0 Quantity X");
	pdu->H_ChannelEventHistogramSetHistogramQuantityY(2, 0, 1, "ChannelID=2 EventPropertyID=0 HistogramID=1 Quantity Y");
	pdu->H_ChannelEventHistogramSetHistogramUnitX(2, 0, 2, "UnitX");
	pdu->H_ChannelEventHistogramSetHistogramUnitY(2, 0, 2, "UnitY");
	pdu->H_ChannelEventHistogramSetHistogramType(2, 0, 2, CPDU::multiChannelScaling);
	pdu->H_ChannelEventHistogramSetHistogramAccumulation(2, 0, 2, CPDU::partial);
	pdu->H_ChannelEventHistogramSetHistogramnBins(2, 0, 2, 4096);
	pdu->H_ChannelEventHistogramSetRangeXInt(2, 0, 2, -6, 4);
}

void SetEventPulse(CPDU *pdu)
{
	// Add a first pulse
	pdu->E_AddEventPulse(1, 111);
	pdu->E_AddEventPulseValueREAL32(32.32);// Add a REAL32 value

	// Add a second pulse
	pdu->E_AddEventPulse(1, 222);
	static const unsigned char _PulseFlags1[] = { 0x40 };
	pdu->E_SetEventPulseFlags(2, _PulseFlags1); // Add 2 flags
	pdu->E_AddEventPulseValueREAL32(32.32); // Add a REAL32 value
	pdu->E_AddEventPulseValueREAL64(64.6464); // Add a REAL64 value
	pdu->E_AddEventPulseValueREAL64(64.646464); // Add a REAL64 value
	pdu->E_AddEventPulseValueInt(3232); // Add a int value
	pdu->E_AddEventPulseValueREAL64(64.64646464); // Add a REAL64 value


	// Add a third pulse
	pdu->E_AddEventPulse(1, 333);
	static const unsigned char _PulseFlags2[] = { 0x60 };
	pdu->E_SetEventPulseFlags(4, _PulseFlags2); // Add 4 flags

	// Add a fourth pulse
	pdu->E_AddEventPulse(4, 444);
	pdu->E_AddEventPulseValueREAL32(32.32);// Add a REAL32 value
	pdu->E_AddEventPulseValueInt(3232); // Add a int value
	pdu->E_AddEventPulseValueREAL32(32.32);// Add a REAL32 value
	pdu->E_AddEventPulseValueInt(3232); // Add a int value

}

void SetEventWaveform(CPDU *pdu)
{
	// Add a first waveform
	pdu->E_AddEventWaveform(5, 555);
	static const unsigned char _PulseFlags3[] = { 0xC0 };
	pdu->E_SetEventWaveformFlags(2, _PulseFlags3);
	pdu->E_AddEventWaveformSignal(); // Adds first signal
	REAL64 _Samples64[] = { 1.1, 2.2, 3.3, 4.4, 5.5 };
	pdu->E_SetEventWaveformSignalSamplesREAL64(5, _Samples64);

	// Add a second waveform
	pdu->E_AddEventWaveform(1, 0);
	pdu->E_AddEventWaveformSignal(); // Adds first signal
	pdu->E_SetEventWaveformSignalSamplingRate(125000);
	REAL32 _Samples32[] = { 10.1, 20.2, 30.3, 40.4, 50.5, 60.6, 70.7 };
	pdu->E_SetEventWaveformSignalSamplesREAL32(7, _Samples32);
	pdu->E_AddEventWaveformSignal(); // Adds second signal
	pdu->E_SetEventWaveformSignalRangeInt(-10, 20);
	int _Samplesint[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
	pdu->E_SetEventWaveformSignalSamplesInt(9, _Samplesint);

	// Add a third waveform
	pdu->E_AddEventWaveform(0, 152);
	pdu->E_AddEventWaveformSignal(); // Adds first signal
	pdu->E_SetEventWaveformSignalSamplingRate(100000);
	pdu->E_SetEventWaveformSignalRangeREAL64(-10.10, 20.20);
	pdu->E_SetEventWaveformSignalSamplesREAL32(3, _Samples32);
	
	// Add a fourth waveform
	pdu->E_AddEventWaveform(0, 0);
	static const unsigned char _PulseFlags4[] = { 0xC0 };
	pdu->E_SetEventWaveformFlags(2, _PulseFlags4);
	pdu->E_AddEventWaveformSignal(); // Adds first signal
	pdu->E_SetEventWaveformSignalSamplesREAL64(4, _Samples64);
}

void SetEventRollover(CPDU *pdu)
{
	pdu->E_AddEventRollover(0, 1);
	pdu->E_AddEventRollover(1, 22);
}

void SetEventTime(CPDU *pdu)
{
	pdu->E_AddEventTime(15, 1254, true, 998877, 999888, false, 0, 0);
	pdu->E_AddEventTime(0, 1254, true, 998877, 999888, false, 0, 0);
	pdu->E_AddEventTime(18, 0, true, 998877, 999888, false, 0, 0);
	pdu->E_AddEventTime(15, 55454, false, 0, 0, true, 5455454, 545454);
	pdu->E_AddEventTime(0, 2215, true, 5464, 564564, true, 5455454, 545454);

}

void SetEventRTC(CPDU *pdu)
{
	pdu->E_AddEventRTCrealtimeclock(2, 2222233, "2016-10-12T14:45:05Z", 0.9954);
	pdu->E_AddEventRTCrealtimeclock(0, 1212233, "2016-10-12T14:45:05Z", 0.5499);
	pdu->E_AddEventRTCsyncstatus(0, 212157, CPDU::synchronised);
	pdu->E_AddEventRTCsyncstatus(0, 455545, CPDU::unsynchronised);
	pdu->E_AddEventRTCsyncstatus(0, 812157, CPDU::synchronised);
	pdu->E_AddEventRTCsyncstatus(0, 955545, CPDU::unsynchronised);
}

void SetEventLogic(CPDU *pdu)
{
	static const unsigned char _Flags[] = { 0xC0 };
	pdu->E_AddEventLogic(99, 8887, 2, _Flags);
	pdu->E_AddEventLogic(0, 0, 2, _Flags);
}

void SetEventGeo(CPDU *pdu)
{
	pdu->E_AddEventGeo(47, 47474747);

	pdu->E_SetEventGeoPositionAsRelativeLocation();
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsDescription("Description of origin");
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPoint(66.6, 77.7);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevation(15);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevationOffset(47);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetGeoPointAccuracy(75);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevationAccuracy(547);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevationOffsetAccuracy(124.5);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetDatum("new datum");
	pdu->E_SetEventGeoPositionAsRelativeLocationSetDistance(54.254);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetInclination(14.2);
	pdu->E_SetEventGeoPositionAsRelativeLocationSetAzimuth(54.7);

	pdu->E_AddEventGeo(0, 0);
	pdu->E_SetEventGeoOrientation();
	pdu->E_SetEventGeoOrientationSetAzimth(90.87);
	pdu->E_SetEventGeoOrientationSetInclination(87.9);
	pdu->E_SetEventGeoOrientationSetRoll(65.9);
	pdu->E_SetEventGeoSpeed(23.9);

	pdu->E_AddEventGeo(55, 1111);
	pdu->E_SetEventGeoPositionAsLocationDescription("55-1111");

}

void SetEventMeasurements(CPDU *pdu)
{
	// Add first event with measurements
	pdu->E_AddEventMeasurements(1, 0);
	pdu->E_AddEventMeasurementValueREAL32(32.5); // only value
	pdu->E_AddEventMeasurementValueREAL32(18.5, 0.3); // value with accuracy

	// Add second event with measurements
	pdu->E_AddEventMeasurements(2, 11212);
	pdu->E_AddEventMeasurementValueREAL32(52.5);
	pdu->E_AddEventMeasurementValueREAL32(54.8);
	pdu->E_AddEventMeasurementValueREAL64(3.8);
	pdu->E_AddEventMeasurementValueREAL64(1.8, 0.2);
	pdu->E_AddEventMeasurementValueInt(1258, 30);
	pdu->E_AddEventMeasurementValueInt(125);

	// Add third event with measurements
	pdu->E_AddEventMeasurements(0, 0);
	pdu->E_AddEventMeasurementValueInt(15);
	pdu->E_AddEventMeasurementValueInt(85);

	// Add fourth event with measurements
	pdu->E_AddEventMeasurements(0, 2254);
	pdu->E_AddEventMeasurementValueREAL64(58.6, 0.6);

}

void SetEventHistograms(CPDU *pdu)
{
	pdu->E_AddEventHistograms(1, 555);
	REAL32 _BinContent32[] = { 10.1, 20.2, 30.3, 40.4, 50.5, 60.6, 70.7 };
	pdu->E_AddEventHistogramsAllBinsREAL32(7, _BinContent32);
	REAL32 _BinContent64[] = { 11.1, 22.2, 33.3, 44.4 };
	pdu->E_AddEventHistogramsAllBinsREAL64(4, _BinContent64);
	
	pdu->E_AddEventHistograms(0, 0);
	int _BinContentInt[] = { 1, 1, 2, 3, 5, 8 };
	pdu->E_AddEventHistogramsAllBinsInt(6, _BinContentInt);
	REAL32 _BinContents32[] = { 101.1, 202.2, 303.3, 404.4, 505.5, 606.6 };
	unsigned int _BinNumber[] = { 26, 25, 24, 23, 22, 21 };
	pdu->E_AddEventHistogramsBinListREAL32(6, _BinNumber, _BinContents32);
	
	pdu->E_AddEventHistograms(2, 0);
	REAL64 _BinContents64[] = { 64101.1, 64202.2, 64303.3, 64404.4, 64505.5, 64606.6 };
	pdu->E_AddEventHistogramsBinListREAL64(6, _BinNumber, _BinContents64);
	int _BinContentsInt[] = { 551, 552, 553, 554, 555, 556 };
	pdu->E_AddEventHistogramsBinListInt(6, _BinNumber, _BinContentsInt);
	
}

void SetEventMessage(CPDU *pdu)
{
	pdu->E_AddEventMessage(0, "Ch0 message 0");
	pdu->E_AddEventMessage(0, "Ch0 message 1", 125);
	pdu->E_AddEventMessage(0, "Ch0 message 2");
	pdu->E_AddEventMessage(1, "Ch1 message 0", 15487);
}

void SetEvents(CPDU *pdu)
{
	// populates events elements
	pdu->SetListmodedataType(CPDU::events_chosen);
	pdu->E_SetListmodeDataID("Listmode data ID=123456");
		
	// Add some events of different types
	SetEventPulse(pdu); 
	//SetEventWaveform(pdu);
	//SetEventRollover(pdu);
	//SetEventTime(pdu);
	//SetEventRTC(pdu);
	//SetEventLogic(pdu);
	//SetEventGeo(pdu);
	//SetEventMeasurements(pdu);
	//SetEventHistograms(pdu);
	//SetEventMessage(pdu);
}

void SetFooter(CPDU *pdu)
{
	// populates footer elements
	pdu->SetListmodedataType(CPDU::footer_chosen);
	pdu->F_SetListmodeDataID("Listmode data ID=123456");
	pdu->F_SetStop("2016-09-15T14:00:00Z", 0.0054);
	pdu->F_SetTotalDeadTime(995588);
	pdu->F_SetTotalLiveTime(1000102);
	pdu->F_ResetTotalLiveTime();
}

void WriteHistogramToFile(const char * filename, const char *title,  unsigned int nchannels, unsigned int *histo)
{
	std::ofstream outfile;
	outfile.open(filename, std::ios::trunc | std::ios::out);

	outfile << title << "\n";
	for (unsigned int i = 0; i < nchannels; i++)
	{
		outfile << histo[i] << "\n";
	}
	outfile.close();
}

void WriteExampleFull(const char *filename, CPDU::EncodingRules EncodingRule)
{
	int retcode;

	// Open file for binary output
	std::ofstream outfile;
	outfile.open(filename,std::ios::binary | std::ios::trunc  | std::ios::out); 
	if (!outfile.is_open() )
	{
		printf("Unable to open output file!");
		return;
	}
	CCodec *codec = new CCodec(); // Declare and init an encoding object
	CPDU *pdu_h = new CPDU(); // Declare a IEC63047 message pdu for header
	CPDU *pdu_e = new CPDU(); // Declare a IEC63047 message pdu for events
	CPDU *pdu_f = new CPDU(); // Declare a IEC63047 message pdu for footer
	
	SetHeader(pdu_h); // populates header elements
	SetEvents(pdu_e); // populates events elements
	SetFooter(pdu_f); // populates footer elements
		
	pdu_h->PrintStructuredData(); // print the header as text
	pdu_e->PrintStructuredData(); // print the events as text
	pdu_f->PrintStructuredData(); // print the footer as text
	
	// Encode the header
	// -----------------
	retcode = pdu_h->Encode(EncodingRule); // Encode the header;
	pdu_h->ClearStructuredData(); // Free the memory that was allocated for the message

	// Check if encoding was success
	if (retcode != 0) {
		pdu_h->PrintError();
	}
	else {
		// Encoding succeeded

		long len = pdu_h->GetEncodedDataLength(); 	// Get the length of encoding 
		const char *data = (const char*)pdu_h->GetEncodedData(); //Get pointer to encoded data 
			
		outfile.write(data, len); // write to file

		//pdu_h->PrintEncodeddata(); // Print the encoded data in hex 
		pdu_h->ClearEncodedData(); // Free memory had been allocated for the encoding

		// Encode the events
		// -----------------
		retcode = pdu_e->Encode(EncodingRule);
		pdu_e->ClearStructuredData(); // Free the memory that was allocated for the message

		// Check if encoding was success
		if (retcode != 0) {
			pdu_e->PrintError();
		}
		else {
			// Encoding succeeded
			long len = pdu_e->GetEncodedDataLength(); 	// Get the length of encoding 
			const char *data = (const char*)pdu_e->GetEncodedData(); //Get pointer to encoded data 

			outfile.write(data, len); // write to file

			//pdu_e->PrintEncodeddata(); // Print the encoded data in hex 
			pdu_e->ClearEncodedData(); // Free memory had been allocated for the encoding
			
			// Encode the footer
			// -----------------
			retcode = pdu_f->Encode(EncodingRule); // Encode the footer;
			pdu_f->ClearStructuredData(); // Free the memory that was allocated for the message

			// Check if encoding was success
			if (retcode != 0) {
				pdu_f->PrintError();
			}
			else {
				// Encoding succeeded

				long len = pdu_f->GetEncodedDataLength(); 	// Get the length of encoding 
				const char *data = (const char*)pdu_f->GetEncodedData(); //Get pointer to encoded data 

				outfile.write(data, len); // write to file

				//pdu_f->PrintEncodeddata(); // Print the encoded data in hex 
				pdu_f->ClearEncodedData(); // Free memory had been allocated for the encoding
			}
		}
	}
	// clean up
	delete pdu_h;
	delete pdu_e;
	delete pdu_f;
	delete codec;

	// close the file
	outfile.close();
}

void WriteExample1(const char *filename, CPDU::EncodingRules EncodingRule)
{
	/* 
	Example 1
	---------

	One device, one channel, providing events of the EventPulse type. Events have timestamp and pulse height.

	*/

	double lambda = 2500.0;	// average number of decays per second
	unsigned int tickspersecond = 100000000;// timebase = 10 ns
	unsigned int timedistributionupperbound = 100000; // in multiples of timebase
	const unsigned int timedistributionchannels = 1000; // number of channels

	unsigned int* energyhistogram;
	energyhistogram = new unsigned int[4096];
	for (unsigned int i = 0; i < 4096; i++)
	{
		energyhistogram[i] = 0;
	}
	
	unsigned int* timedistribution;
	timedistribution = new unsigned int[timedistributionchannels];
	for (unsigned int i = 0; i < timedistributionchannels; i++)
	{
		timedistribution[i] = 0;
	}

	int retcode;
	long len;
	const char *data;

	//std::random_device rd;	// non-deterministic generator
	//std::mt19937 gen(rd()); // to seed mersenne twister
	//std::normal_distribution<double> energy(3000.0, 25.0); // normal distribution for energy: channel 3000, sigma = 25 channels
	//std::uniform_real_distribution<double> unif(0.0,1.0); // uniform distribution between 0 and 1


	// Open file for binary output
	std::ofstream outfile;
	outfile.open(filename, std::ios::binary | std::ios::trunc | std::ios::out);
	if (!outfile.is_open())
	{
		printf("Unable to open output file!");
		return;
	}
	CCodec *codec = new CCodec(); // Declare and init an encoding object

	////////////
	// HEADER //
	////////////
	CPDU *pdu_h = new CPDU(); // Declare a IEC63047 message pdu for header

	// Set the header elements
	pdu_h->SetListmodedataType(CPDU::header_chosen);
	pdu_h->H_SetListmodeDataID("123456");
	pdu_h->H_SetMeasSetupID("Measurement setup ID=yyyy");
	pdu_h->H_SetMeasSetupDescription("Samples artificially generated to demonstrate codec dll.");
	pdu_h->H_SetIec62755single(); // There is no associated IEC62755 file
	pdu_h->H_SetRadSource("Radiation source is artificial. Data is simulated.");
	pdu_h->H_SetStart("2016-09-26T20:30:00Z", 0);
	pdu_h->H_SetStartAccuracy(0.0);

	pdu_h->H_AddDevices(1); // One device
	pdu_h->H_SetDeviceName(0, "DeviceID=0 Name");
	pdu_h->H_SetDeviceManuf(0, "DeviceID=0 Manuf");
	pdu_h->H_SetDeviceModel(0, "DeviceID=0 Model");
	pdu_h->H_SetDeviceSerial(0, "DeviceID=0 Serial");
	
	pdu_h->H_AddChannels(1); // One channel
	pdu_h->H_SetChannelDeviceID(0, 0); // Channel 0 belongs to Device 0
	pdu_h->H_SetChannelType(0, CPDU::virtualtype);
	pdu_h->H_SetChannelName(0, "ChannelID=0 Name");
	pdu_h->H_SetChannelDescription(0, "ChannelID=0 Virtual channel containing simulated data from single gamma emitting nuclide.");
	pdu_h->H_SetChannelParameters(0, "ChannelID=0 Parameters");
	pdu_h->H_SetChannelADCSamplingRate(0, 100000000); // 100 MS/s
	pdu_h->H_SetChannelADCBitResolution(0, 12); // 12  bit; 0 to 4095
	pdu_h->H_SetChannelTicksPerSecond(0, tickspersecond); 
	pdu_h->H_AddChannelEventProperties(0, 1); // adds 1 event property to channel 0

	pdu_h->H_SetChannelEventPropertyType(0, 0, CPDU::eventpulse); // set type
	pdu_h->H_SetChannelEventPropertyDescription(0, 0, "ChannelID=0 EventPropertyID=0 Pulse");
	pdu_h->H_AddChannelEventPulsePropertyValues(0, 0, 1); // channel 0, event property 0 has 1 value
	pdu_h->H_SetChannelEventPulsePropertyValueDescription(0, 0, 0, "ChannelID=0 EventPropertyID=0 Pulseheight");
	
	
	pdu_h->PrintStructuredData(); // print the header as text
	retcode = pdu_h->Encode(EncodingRule); // Encode the header;
	pdu_h->ClearStructuredData(); // Free the memory that was allocated for the message
	len = pdu_h->GetEncodedDataLength(); 	// Get the length of encoding 
	data = (const char*)pdu_h->GetEncodedData(); //Get pointer to encoded data 
	outfile.write(data, len); // write to file
	//pdu_h->PrintEncodeddata(); // Print the encoded data in hex 
	pdu_h->ClearEncodedData(); // Free memory had been allocated for the encoding
	delete pdu_h;// clean up
	

	////////////
	// EVENTS //
	////////////

	CPDU *pdu_e;
	unsigned int nTotalEvents = 100; // 100000
	unsigned int nEventsPerGroup = 100; // 100000

	unsigned int nEventsInGroup;
	unsigned int nEvents = 0;
	unsigned int dt; // time until next decay (in multiples of timebase)
	unsigned int tme = 0; // the time (in multiples of timebase)
	int pulseheight;

	while (nEvents < nTotalEvents)
	{
		pdu_e = new CPDU(); // Declare a IEC63047 message pdu for events
		pdu_e->SetListmodedataType(CPDU::events_chosen);
		pdu_e->E_SetListmodeDataID("123456");
		StartTiming();
		nEventsInGroup = 0;
		while ((nEventsInGroup < nEventsPerGroup) && (nEvents < nTotalEvents))
		{
			// A better example would be to pull random number for timestamp and pulse height here, and 
			pulseheight = 3000;//(int)energy(gen);
			dt = 42;//(unsigned int)(-std::log(unif(gen)) / lambda * (double)tickspersecond); 
			tme += dt;
			pdu_e->E_AddEventPulse(0, tme); // use event number as timestamp 
			pdu_e->E_AddEventPulseValueInt(pulseheight); // pulseheight
			pdu_e->E_AddEventPulseValueREAL32(1234.5678);
			if (dt < timedistributionupperbound)
				timedistribution[dt* timedistributionchannels / timedistributionupperbound]++;
			energyhistogram[pulseheight]++;
			nEventsInGroup++;
			nEvents++;
		}
		StopTiming("Adding events");

		pdu_e->PrintStructuredData(); // print the structured data as text
		
		StartTiming();
		retcode = pdu_e->Encode(EncodingRule); // Encode the header;
		StopTiming("Encoding");
		
		StartTiming();
		pdu_e->ClearStructuredData(); // Free the memory that was allocated for the message
		StopTiming("ClearStructuredData");

		len = pdu_e->GetEncodedDataLength(); 	// Get the length of encoding 
		data = (const char*)pdu_e->GetEncodedData(); //Get pointer to encoded data 

		StartTiming();
		outfile.write(data, len); // write to file
		StopTiming("Adding to file");

		//pdu_e->PrintEncodeddata(); // Print the encoded data in hex 

		StartTiming();
		pdu_e->ClearEncodedData(); // Free memory had been allocated for the encoding
		StopTiming("ClearEncodedData");
		
		delete pdu_e;// clean up
	}


	////////////
	// FOOTER //
	////////////
	CPDU *pdu_f = new CPDU(); // Declare a IEC63047 message pdu for footer

	// populates footer elements
	pdu_f->SetListmodedataType(CPDU::footer_chosen);
	pdu_f->F_SetListmodeDataID("123456");

	
	pdu_f->PrintStructuredData(); // print the footer as text
	retcode = pdu_f->Encode(EncodingRule); // Encode the footer;
	pdu_f->ClearStructuredData(); // Free the memory that was allocated for the message
	len = pdu_f->GetEncodedDataLength(); 	// Get the length of encoding 
	data = (const char*)pdu_f->GetEncodedData(); //Get pointer to encoded data 
	outfile.write(data, len); // write to file
	//pdu_f->PrintEncodeddata(); // Print the encoded data in hex 
	pdu_f->ClearEncodedData(); // Free memory had been allocated for the encoding
	delete pdu_f;// clean up

	delete codec;// clean up

	// close the file
	outfile.close();

	WriteHistogramToFile("Energyh.txt", "Energy histogram", 4096, energyhistogram);
	WriteHistogramToFile("Timeh.txt", "Time interval distribution", timedistributionchannels, timedistribution);
}

//int main()
//{
//	/* Select one or more functions to run the demo.
//	Comment-out those that you don't want.
//	See the functions for a description for what they do.
//
//	The first parameter is the filename generated. The second is the encoding rule: ER_COER or ER_PER_ALIGNED 
//	As an undecided convention, the extension corresponds to the encoding rule
//
//	*/
//
//	// WriteExampleFull("ExampleFull.coer", CPDU::ER_COER);
//	// ReadListmodedataFile("ExampleFull.coer", CPDU::ER_COER);
//	// WriteExampleFull("ExampleFull.per", CPDU::ER_PER_ALIGNED);
//	// ReadListmodedataFile("ExampleFull.per", CPDU::ER_PER_ALIGNED);
//	
//	WriteExample1("Example1.coer", CPDU::ER_COER);
//
//
//	ReadListmodedataFile("Example1.coer", CPDU::ER_COER);
//	
//
//	return 0;
//}

void CompareReadEventPulse(CPDU *pdu)
{
	// Read an event of the pulse type
	
	unsigned int channelid;
	TimeStamp timestamp;
	unsigned int _nFlags = 0, _nValues = 0, _ValueID;
	unsigned char *_FlagsBitstring = NULL;
	CPDU::TypeOfNumeric _typeofnumeric;
	REAL32 _real32Value;
	REAL64 _real64Value;
	int _intValue;

	channelid = pdu->E_GetEventPulseChannelID();
	if (pdu->E_EventPulseTimeStampPresent())
		timestamp = pdu->E_GetEventPulseTimeStamp();

	pdu->E_GetEventPulseFlags( _nFlags, &_FlagsBitstring);

	// KS DEBUG: output of pulse values
	printf("%d\t%d\t%lu", pdu->E_GetEventsID(), channelid, timestamp);
	_nValues = pdu->E_GetEventPulseNValues();
	for (_ValueID = 0; _ValueID < _nValues; _ValueID++)
	{
		_typeofnumeric = pdu->E_GetEventPulseTypeOfNumericValue( _ValueID);
		switch (_typeofnumeric)
		{
		case CPDU::TypeOfNumeric::real32_chosen:
			_real32Value = pdu->E_GetEventPulseValueREAL32( _ValueID);
			break;
		case CPDU::TypeOfNumeric::real64_chosen:
			_real64Value = pdu->E_GetEventPulseValueREAL64( _ValueID);
			break;
		case CPDU::TypeOfNumeric::Numeric_int_chosen:
			_intValue = pdu->E_GetEventPulseValueInt( _ValueID);
			printf("\t%d", _intValue);
			break;
		}
	} // for values
	printf("\n");
}
void CompareReadEvents(CPDU *pdu)
{
	// Read some data from events
	char * eventsDataID = pdu->E_GetListmodeDataID();
	unsigned int id = pdu->E_GetEventsID();

	pdu->PointAtFirstEvent();
	CPDU::TypeOfEvent evtp;
	evtp = pdu->E_GetEventType();
	while (!pdu->EndOfEvents()) // (evtp != CPDU::TypeOfEvent::eventNone)
	{
		switch (evtp)
		{
		case CPDU::eventPulse_chosen:
			CompareReadEventPulse(pdu);
			break;
		case CPDU::eventWaveform_chosen:
			ReadEventWaveform(pdu);
			break;
		case CPDU::eventRollover_chosen:
			ReadEventRollover(pdu);
			break;
		case CPDU::eventTime_chosen:
			ReadEventTime(pdu);
			break;
		case CPDU::eventRTC_chosen:
			ReadEventRTC(pdu);
			break;
		case CPDU::eventLogic_chosen:
			ReadEventLogic(pdu);
			break;
		case CPDU::eventGeo_chosen:
			ReadEventGeo(pdu);
			break;
		case CPDU::eventMeasurements_chosen:
			ReadEventMeasurements(pdu);
			break;
		case CPDU::eventHistograms_chosen:
			ReadEventHistograms(pdu);
			break;
		case CPDU::eventMessage_chosen:
			ReadEventMessage(pdu);
			break;
		default:
			break;
		};
		pdu->PointAtNextEvent();
		evtp = pdu->E_GetEventType();
	}
}
void CompareReadListmodedataFile(const char *filename, CPDU::EncodingRules EncodingRule)
{
	// Read a listmode data file and decode the messages
	int retcode;
	unsigned int nMessages = 0;
	CCodec *codec = new CCodec(); // Declare and init an encoding object
	CPDU *pdu = new CPDU();

	// Read the file with encoded data
	// The file contains more than one pdu encoding.
	StartTiming();
	retcode = pdu->ReadCompleteEncodedDataFile(filename);
	StopTiming("Reading encoded data file");
	printf("Event\tChannel\tTimestamp\tEnergy\tXIAPSA\tUserPSA\tPSA0\tPSA1\tPSA2\tPSA3\n");
	while (pdu->GetEncodedDataLength() > 0)
	{
		StartTiming();
		pdu->Decode(EncodingRule);
		StopTiming("Decoding");

		// process decoded pdu
		//printf("Message %u:\n", nMessages);

		//pdu->PrintStructuredData(); // comment-out if printing is not wanted

		switch (pdu->GetListmodedataType())
		{
		case CPDU::header_chosen:
			StartTiming();
			ReadHeader(pdu);
			StopTiming("Reading header");
			break;
		case CPDU::events_chosen:
			StartTiming();
			CompareReadEvents(pdu);
			StopTiming("Reading events");
			break;
		case CPDU::footer_chosen:
			StartTiming();
			ReadFooter(pdu);
			StopTiming("Reading footer");
			break;
		}
		// Clear data
		StartTiming();
		pdu->ClearStructuredData();
		StopTiming("Clearing structureddata");

		// Next
		StartTiming();
		pdu->PrepareforNextDecoding();
		StopTiming("Preparing for next decoding");
		nMessages++;
	}

	// clean up
	delete pdu;
	delete codec;
}


// a few more utility functions from the main XIA code

/****************************************************************
*	Pixie_fseek function:
*		This routine serves as a wrapper for a system dependent
*              fseek function, with Large-File Support.
*
*		Return Value:
*			 0 - success 
*		      
*
****************************************************************/

S32 Pixie_fseek (
				 FILE *stream,			// Pointer to FILE structure
				 S64 offset,             // Number of bytes from origin
				 S32 origin)				// Initial position
{
	S32 retval;


#ifdef WI64
	retval = _fseeki64(stream, offset, origin);
#else
	retval = fseek(stream, offset, origin);
#endif


	return(retval);
}

/****************************************************************
*	Pixie_ftell function:
*		This routine serves as a wrapper for a system dependent
*              ftell function, with Large-File Support.
*
*		Return Value:
*			 Current position of a file pointer 
*		      
*
****************************************************************/

S64 Pixie_ftell (
				 FILE *stream)			// Pointer to FILE structure

{
	S64 retval;

#ifdef WI64
	retval = _ftelli64(stream);
#else
	retval = ftell(stream);
#endif



	return(retval);
}

/****************************************************************
*	TstBit function:
*		Test Bit function (for 16-bit words only).
*
*		Return Value:
*			bit value
*
****************************************************************/

U16 TstBit(U16 bit, U16 value)
{
	return(   (value & (1<<bit)) >> bit);
}


/****************************************************************
*	Pixie_Print_MSG function:
*		This routine prints error message 
*
****************************************************************/

S32 Pixie_Print_MSG (
					 S8 *message, 	// message to be printed 
					 U32 enable )	// print it or not
{
	if(enable)
	{
		printf("%s\n",message);
	}	// end if enable

	return(0);
}



/****************************************************************
*	Finally MAIN
*
****************************************************************/
int main(int argc, char *argv[])
{
	// argv[1] - file name
	// argv[2] - max number of events to process (0=all)
	// argv[3] - options: if 1 print all messages

	U32 *UserDataList = NULL;
	U32 *UserDataEvent = NULL;
	U32 i, j, k;
	U64 NumEvents = 0;
	U64 ProcEvents = 0;

	FILE *fpEncoded = NULL;
	FILE *fpRaw = NULL;
	
	U16 runHeader[RUN_HEAD_LENGTH];
	U16 ModNum, RunType, BoardRev, BoardSN;
	char sBoardRev[8];
	char sBoardSN[8];
	char sDevName[128];
	int ADCSamplingRate, ADCBitResolution;

	CPDU::EncodingRules EncodingRule;
	CCodec *codec = new CCodec(); // Declare and init an encoding object	
	CPDU *pdu_h, *pdu_e, *pdu_f;
	int retcode, maxevents, verbose;
	long len;
	const char *data;
	unsigned int chan;
	char chanName[64];

	unsigned long long TimeStamp = 0;
	unsigned short Energy, XIAPSA, UserPSA, ExtendedPSA0, ExtendedPSA1, ExtendedPSA2, ExtendedPSA3, TraceLen;
	unsigned char EventStatus[] = {0x00,0x00,0x00,0x00};
	int *Trace = NULL;

#ifdef XIA_READER
	char filename[128];
	char filenameEncoded[128];

	if (argc < 2) {
		printf("Provide file name as an argument\n");
		return(-1);
	}
	
	strcpy(filename, argv[1]);
	fpRaw = fopen(filename, "rb");
	if (!fpRaw) {
		printf("Cannot open finary file %s!\n", filename);
		return(-2);
	}
	
	if (argc >= 3) {
		maxevents = atoi(argv[2]);
		if(maxevents<0) maxevents = 0;
	} else {
		maxevents = 0;
	}

	if (argc == 4) {
		verbose = atoi(argv[3]);
	} else {
		verbose = 0;
	}


	fread(runHeader, sizeof(U16), RUN_HEAD_LENGTH, fpRaw);
	fclose(fpRaw);

	ModNum = runHeader[1];
	RunType = runHeader[2];
	BoardRev = runHeader[7];
	BoardSN = runHeader[12];

	if (RunType != 0x400) {
		printf("Only RunType=0x400 encoding implemented!\n");
		return(-3);
	}
	sprintf(sBoardRev, "0x%04X", BoardRev);
	sprintf(sBoardSN, "0x%04X", BoardSN);
	sprintf(sDevName, "Pixie4e-%d", ModNum);

	switch (BoardRev & 0x0FF0) {
		case MODULETYPE_P500e:
		case MODULETYPE_P4e_14_500:
			ADCBitResolution = 14;
			ADCSamplingRate = 500000000;
			break;
		case MODULETYPE_P4e_16_125:
			ADCBitResolution = 16;
			ADCSamplingRate = 125000000;
			break;
		case MODULETYPE_P4e_14_125:
			ADCBitResolution = 14;
			ADCSamplingRate = 125000000;
			break;
		case MODULETYPE_P4e_12_125:
			ADCBitResolution = 12;
			ADCSamplingRate = 125000000;
			break;
		case MODULETYPE_P4e_16_250:
			ADCBitResolution = 16;
			ADCSamplingRate = 250000000;
			break;
		case MODULETYPE_P4e_14_250:
			ADCBitResolution = 14;
			ADCSamplingRate = 250000000;
			break;
		case MODULETYPE_P4e_12_250:
			ADCBitResolution = 12;
			ADCSamplingRate = 250000000;
			break;                                  
		case MODULETYPE_P4e_16_500:
			ADCBitResolution = 16;
			ADCSamplingRate = 500000000;
			break;
		case MODULETYPE_P4e_12_500:
			ADCBitResolution = 12;
			ADCSamplingRate = 500000000;
			break;
		default:
			printf("Unsupported module version\n");
			return(-4);
			break;
	}

	// To control debug messages printout and genration of ref txt file
	if (verbose==1) {
		PrintDebugMsg_other = 1;
		PrintDebugMsg_QCdetail = 1;
		PrintDebugMsg_QCerror = 1;
		AutoProcessLMData = 3; 
	} else {
		PrintDebugMsg_other = 0;
		PrintDebugMsg_QCdetail = 0;
		PrintDebugMsg_QCerror = 0;
		AutoProcessLMData = 0; 
	}

	/* Allocate memory for the user array.
	 * Depending on the task the user array must have an appropriate size.
	 * In case of 0x7001 the size is 17 for the maximum number of mudules in the system */
	UserDataList = (U32*)calloc(PRESET_MAX_MODULES + 1, sizeof(U32));
	if (UserDataList == NULL) {
		printf("Cannot allocate memory for 0x7001\n");
	}

	// Get number of events

	Pixie_List_Mode_Parser((S8*)filename, UserDataList, 0x7001); // parse headers
	NumEvents = UserDataList[0];
	if( (maxevents>UserDataList[0]) || (maxevents==0) )
		ProcEvents = UserDataList[0];
	else
		ProcEvents = maxevents;
	printf("0x7001 done, events found %d, processing %d\n", UserDataList[0], maxevents);
	if (UserDataList != NULL) free(UserDataList);


	EncodingRule = CPDU::ER_COER;
	strcpy(filenameEncoded, filename);
	strcat(filenameEncoded, ".coer"); 
	fpEncoded = fopen(filenameEncoded, "wb");
	if (!fpEncoded) {
		printf("Cannot open output file\n");
		return(-1);
	}

	////////////
	// HEADER //
	////////////
	pdu_h = new CPDU(); // Declare a IEC63047 message pdu for header

	// Read Run Header
	// NB: would be nice to record the whole .set file (and .ifm?). At least filter/threshold settings! Parameters?
	// Binary file name.
	// RunType.
	// Device Serial Number.
	// Check with multiple channels.
	// Statistics? -- FTDT, RT, LT, #triggers.
	// Coincidences?
	// Start/Stop date/time?
	// Encode header
	// Set the header elements
	// FIXME: should be automated
	pdu_h->SetListmodedataType(CPDU::header_chosen);
	pdu_h->H_SetListmodeDataID("123456");
	pdu_h->H_SetMeasSetupID("Measurement setup ID=yyyy");
	pdu_h->H_SetMeasSetupDescription(filename);
	pdu_h->H_SetIec62755single(); // There is no associated IEC62755 file
	pdu_h->H_SetRadSource("Do we keep any notes with experiments?");
	pdu_h->H_SetStart("2016-09-26T20:30:00Z", 0); // from IFM?
	pdu_h->H_SetStartAccuracy(0.0);

	pdu_h->H_AddDevices(1); // One device
	pdu_h->H_SetDeviceName(0, sDevName); // with module number
	pdu_h->H_SetDeviceManuf(0, "XIA LLC");
	pdu_h->H_SetDeviceModel(0, sBoardRev);
	pdu_h->H_SetDeviceSerial(0, sBoardSN);

	// TODO: Hardcode 4 channels?
	pdu_h->H_AddChannels(NUMBER_OF_CHANNELS);
	for (chan = 0; chan < NUMBER_OF_CHANNELS; chan++) {
		sprintf(chanName, "Ch%d", chan);
		pdu_h->H_SetChannelDeviceID(chan, 0); // Multiple modules: channels continue to increment!
		pdu_h->H_SetChannelType(chan, CPDU::physicaltype);
		pdu_h->H_SetChannelName(chan, chanName);
		pdu_h->H_SetChannelDescription(chan, "Channel description");
		pdu_h->H_SetChannelParameters(chan, "Channel Parameters"); // rise, flattop... Here?
		pdu_h->H_SetChannelADCSamplingRate(chan, ADCSamplingRate); // 500 MS/s
		pdu_h->H_SetChannelADCBitResolution(chan, ADCBitResolution);
		pdu_h->H_SetChannelTicksPerSecond(chan, 500000000); // clock for timestamp, fixed to 2 ns
		// Event properties for each channel: pulse or waveform
		// Pulse property has multiple values: height, user/XIA/extended PSA. 
		// Also flags (Hit pattern, event status) (up to 32. not all needed?)
		// Waveform property--probably with just one value (which is...)
		// For dead time see 6.10.4.
		pdu_h->H_AddChannelEventProperties(chan, 2); // property 0 pulse, property 1 waveform

		// TODO: Think what other names would be useful predefined
		pdu_h->H_SetChannelEventPropertyType(chan, 0, CPDU::eventpulse);
		pdu_h->H_SetChannelEventPropertyDescription(chan, 0, "Pulse");
		pdu_h->H_AddChannelEventPulsePropertyValues(chan, 0, 7); // event property 0 has 7 values
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 0, "*HEIGHT");
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 1, "XIAPSA");	
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 2, "UserPSA");
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 3, "ExtendedPSA0");
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 4, "ExtendedPSA1");	
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 5, "ExtendedPSA2");
		pdu_h->H_SetChannelEventPulsePropertyValueDescription(chan, 0, 6, "ExtendedPSA3");
		pdu_h->H_AddChannelEventPulsePropertyFlags(chan, 0, 32);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  0, "EventPattern 0 Ch0 recorded", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  1, "EventPattern 1 Ch1 recorded", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  2, "EventPattern 2 Ch2 recorded", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  3, "EventPattern 3 Ch3 recorded", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  4, "EventPattern 4 FRONT panel input", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  5, "EventPattern 5 LOCAL acceptance", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  6, "EventPattern 6 backplane STATUS", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  7, "EventPattern 7 backplane TOKEN", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  8, "EventPattern 8  Ch0 hit", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0,  9, "EventPattern 9  Ch1 hit", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 10, "EventPattern 10 Ch2 hit", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 11, "EventPattern 11 Ch3 hit", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 12, "EventPattern 12 GATE Ch0", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 13, "EventPattern 13 GATE Ch1", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 14, "EventPattern 14 GATE Ch2", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 15, "EventPattern 15 GATE Ch3", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 16, "EventInfo 0 Coincidence", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 17, "EventInfo 1 backplane VETO", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 18, "EventInfo 2 *PILEUP", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 19, "EventInfo 3 waveform FIFO full", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 20, "EventInfo 4 current channel hit", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 21, "EventInfo 5 current channel GATE", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 22, "EventInfo 6 *OUT_OF_RANGE", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 23, "EventInfo 7", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 24, "EventInfo 8", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 25, "EventInfo 9", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 26, "EventInfo 10", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 27, "EventInfo 11", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 28, "EventInfo 12", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 29, "EventInfo 13", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 30, "EventInfo 14", CPDU::logicstate);
		pdu_h->H_SetChannelEventPulsePropertyFlag(chan, 0, 31, "EventInfo 15 event ERROR", CPDU::logicstate);

		pdu_h->H_SetChannelEventPropertyType(chan, 1, CPDU::eventwaveform);
		pdu_h->H_SetChannelEventPropertyDescription(chan, 1, "Trace");
		pdu_h->H_AddChannelEventWaveformSignalProperties(chan, 1, 1); // one signal
		pdu_h->H_ChannelEventWaveformSetSignalDescription(chan, 1, 0, "channel EventPropertyID=1 SignalID=0 Description");

	}
	pdu_h->PrintStructuredData(); // print the header as text
	retcode = pdu_h->Encode(EncodingRule); // Encode the header;
	pdu_h->ClearStructuredData(); // Free the memory that was allocated for the message
	len = pdu_h->GetEncodedDataLength(); 	// Get the length of encoding 
	data = (const char*)pdu_h->GetEncodedData(); //Get pointer to encoded data 
	fwrite(data, len, 1, fpEncoded); // write to file
	//pdu_h->PrintEncodeddata(); // Print the encoded data in hex 
	pdu_h->ClearEncodedData(); // Free memory had been allocated for the encoding
	delete pdu_h;// clean up



	UserDataList = (U32*)calloc(NumEvents * 3, sizeof(U32)); // 3 entries per event

	if (UserDataList == NULL) {
		printf("Cannot allocate memory for 0x7007\n");
	}
	Pixie_List_Mode_Parser((S8*)filename, UserDataList, 0x7007); // event positions in binary file
	
	////////////
	// EVENTS //
	////////////


	printf("Starting to encode %lu events\n...", ProcEvents);
	// Now processing each event
	for (i = 0; i < ProcEvents; i++) {


		//printf("Event %ld, pos=%ld, pos1=%ld, len=%ld\n", i, UserDataList[3*i+0], UserDataList[3*i+1], UserDataList[3*i+2]);

		// NB Length: 0x7007 reports EventLen = 32 (blocksize)* (1 (header) + TraceLength) * 4 (num. chan)
		// Pixie_Event_Browser returns 52 header words + non-zero tracelength waveforms.
		// So, allocating (EventLen - 32*1(header)*4(ch))+52: mode than needed, but we don't know which length is will be.
		// UserDataEvent has room for 4 channel headers + 4 channel traces, but only one header and trace will be filled.
		UserDataEvent = (U32*)calloc(UserDataList[3*i+2]-32*1*4 + 52, sizeof(U16)); 
		//printf("Allocating event size %d...", UserDataList[3*i+2]-32*1*4 + 52); getchar();
		if (UserDataEvent == NULL) {
			printf("Cannot allocate memory for event %ld\n", i);
		}
		UserDataEvent[0] = UserDataList[3*i+0]; // position
		UserDataEvent[2] = UserDataList[3*i+2]; // length. Actually, length is max possible: 32(blocksize)*(1(header)+traces) words
		UserDataEvent[3] = 65536; // time window, larger than 64K to disable search for neighbors

		// Extract data for event from binary file
		// Pixie_Event_Browser() takes UserData[0,2,3] as inputs, returns:
		// structure of UserData from Pixie_Event_Browser():
		//UserData[1]   = *P500E->ADCrate;
		//UserData[7+1] = *P500E->ModNum;
		//UserData[7+2] = *P500E->RunType;
		//UserData[7+3] = ChanHeader[6];								// Equivalent to Buffer Start Time High Word  
		//UserData[7+4] = ChanHeader[5];								// Equivalent to Buffer Start Time Middle Word  
		//UserData[7+5] = ChanHeader[4];								// Equivalent to Buffer Start Time Low Word 
		//UserData[7+BHL] = ChanHeader[0] + 65536 * ChanHeader[1]; ;	// Event status 
		//UserData[7+BHL+2] = ChanHeader[4];							// Time low 
		//UserData[7+BHL+1] = ChanHeader[5];							// Time medium 
		//UserData[3+CurrentChanNum] = TraceSizeR;				
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+1] = 65536 * (U32)ChanHeader[5] + (U32)ChanHeader[4]; /* Time medium and low*/
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+2] = ChanHeader[8]; /* Energy */
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+3] = ChanHeader[11]; /* XIA PSA */
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+4] = ChanHeader[10]; /* User PSA */
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+5] = ChanHeader[12]; /* Extended Uretval */
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+6] = ChanHeader[13]; /* Extended Uretval */
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+7] = ChanHeader[14]; /* Extended Uretval */
		//UserData[7+BHL+EHL+CHL*CurrentChanNum+8] = ChanHeader[15]; /* Extended Uretval */
		// Trace data for channels with non-zero TraceSizeR (actually, only current channel)

		Pixie_Event_Browser((S8*)filename, UserDataEvent);

		///////////////////////////////////////////////////////////////////
		//// DEBUG

		//// headers
		////for (j = 0; j < 52; j++) printf("\t%d 0x%08X\n", j, UserDataEvent[j]);

		//printf("%lu\t", i);
		//printf("EvtStatus:\t0x%08X\t", UserDataEvent[7+BUFFER_HEAD_LENGTH]);
		//TimeStamp =  (unsigned long long)((double)UserDataEvent[7+5] + 65536.0 * (double)UserDataEvent[7+4] + 4294967296.0 * (double)UserDataEvent[7+3]);
		//printf("TimeStamp:\t%llu\n", TimeStamp);
		//printf("TraceLen :");
		//for (j = 0; j < 4; j++) printf("\t%d", UserDataEvent[3+j]);
		//printf("\n");
		//printf("Energy   :");
		//for (j = 0; j < 4; j++) printf("\t%d", UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*j+2]);
		//printf("\n");
		//for (j = 0; j < 4; j++) {
		//	if (UserDataEvent[3+j]>=0) {
		//		for (k=0; k < UserDataEvent[3+j]; k++) {
		//			//printf("ch%d\t%d 0x%08X\n", j, k, UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*4+k]);
		//		} // for trace
		//	}
		//} // for channels
		////////////////////// end DEBUG

		// Encode each event
		// FIXME: timestamp looks like 32-bit, overflows
		TimeStamp =  (unsigned long long)((double)UserDataEvent[7+5] + 65536.0 * (double)UserDataEvent[7+4] + 4294967296.0 * (double)UserDataEvent[7+3]);
		EventStatus[0] = (UserDataEvent[7+BUFFER_HEAD_LENGTH] & 0x000000FF);
		EventStatus[1] = (UserDataEvent[7+BUFFER_HEAD_LENGTH] & 0x0000FF00) >> 8;
		EventStatus[2] = (UserDataEvent[7+BUFFER_HEAD_LENGTH] & 0x00FF0000) >> 16;
		EventStatus[3] = (UserDataEvent[7+BUFFER_HEAD_LENGTH] & 0xFF000000) >> 24;

		switch (EventStatus[0] & 0x0F) { // channel recorded pattern
			case 1:
				chan = 0;
				break;
			case 2:
				chan = 1;
				break;
			case 4:
				chan = 2;
				break;
			case 8:
				chan = 3;
				break;
			default:
				break;
		}
		Energy       = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+2];
		XIAPSA       = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+3];
		UserPSA      = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+4];
		ExtendedPSA0 = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+5];
		ExtendedPSA1 = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+6];
		ExtendedPSA2 = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+7];
		ExtendedPSA3 = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*chan+8];

		TraceLen = 0;
		TraceLen = UserDataEvent[3+chan];
		//printf("Chan %d TraceLen=%d\n", chan, TraceLen); getchar();
		if (TraceLen>0) Trace = (int *)calloc(TraceLen, sizeof(int));
		for (k = 0; k < TraceLen; k++) {
			Trace[k] = UserDataEvent[7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+P4_MAX_CHAN_HEAD_LENGTH*NUMBER_OF_CHANNELS+k];
			//printf("%d 0x%08X\n", k, Trace[k]);
		}

		if (TimeStamp) {
			pdu_e = new CPDU(); // Declare a IEC63047 message pdu for events
			pdu_e->SetListmodedataType(CPDU::events_chosen);
			pdu_e->E_SetListmodeDataID("123456");

			pdu_e->E_AddEventPulse(chan, TimeStamp);
			pdu_e->E_AddEventPulseValueInt(Energy);
			pdu_e->E_AddEventPulseValueInt(XIAPSA);
			pdu_e->E_AddEventPulseValueInt(UserPSA);
			pdu_e->E_AddEventPulseValueInt(ExtendedPSA0);
			pdu_e->E_AddEventPulseValueInt(ExtendedPSA1);
			pdu_e->E_AddEventPulseValueInt(ExtendedPSA2);
			pdu_e->E_AddEventPulseValueInt(ExtendedPSA3);

			pdu_e->E_SetEventPulseFlags(32, EventStatus);

			if (TraceLen) {
				pdu_e->E_AddEventWaveform(chan, TimeStamp);
				pdu_e->E_AddEventWaveformSignal();
				pdu_e->E_SetEventWaveformSignalSamplesInt(TraceLen, Trace);
			}
			//pdu_e->PrintStructuredData(); // print the structured data as text

			retcode = pdu_e->Encode(EncodingRule); // Encode the event;
			pdu_e->ClearStructuredData(); // Free the memory that was allocated for the message
			len = pdu_e->GetEncodedDataLength(); 	// Get the length of encoding 
			data = (const char*)pdu_e->GetEncodedData(); //Get pointer to encoded data 
			fwrite(data, len, 1, fpEncoded);
			pdu_e->ClearEncodedData(); // Free memory had been allocated for the encoding
			delete pdu_e;// clean up
		}
		// Done with this event
		if (UserDataEvent != NULL) {
			free(UserDataEvent);
			UserDataEvent = NULL;
		}
		if (Trace != NULL) {
			free(Trace);
			Trace = NULL;
		}
	} // for events

	printf("Done with all events\n");
	if (UserDataList != NULL) free(UserDataList);

	////////////
	// FOOTER //
	////////////
	pdu_f = new CPDU(); // Declare a IEC63047 message pdu for footer
	// populates footer elements
	pdu_f->SetListmodedataType(CPDU::footer_chosen);
	pdu_f->F_SetListmodeDataID("123456");
	pdu_f->PrintStructuredData(); // print the footer as text
	retcode = pdu_f->Encode(EncodingRule); // Encode the footer;
	pdu_f->ClearStructuredData(); // Free the memory that was allocated for the message
	len = pdu_f->GetEncodedDataLength(); 	// Get the length of encoding 
	data = (const char*)pdu_f->GetEncodedData(); //Get pointer to encoded data 
	fwrite(data, len, 1, fpEncoded); // write to file
	//pdu_f->PrintEncodeddata(); // Print the encoded data in hex 
	pdu_f->ClearEncodedData(); // Free memory had been allocated for the encoding
	delete pdu_f;// clean up

	delete codec;// clean up

	// close the file
	fclose(fpEncoded);

	// Read back encoded data, dump info something like dt3 file for comparison
	if (verbose==1) {
		CompareReadListmodedataFile(filenameEncoded, CPDU::ER_COER);
	}
#else
ReadListmodedataFile("filenameEncoded", CPDU::ER_COER);

//WriteExampleFull("ExampleFull.coer", CPDU::ER_COER);
	//WriteExample1("Example1.coer", CPDU::ER_COER);
	//getchar();
	//ReadListmodedataFile("Example1.coer", CPDU::ER_COER);
#endif
	return(0);
}


