// Header file to be inlcuded in applications that use the IEC63047 codec dll.
// 
// Copyright, 2016, European Atomic Energy Community
// 
// Author: Jan Paepen, jan.paepen@ec.europa.eu


#ifdef CODECDLL_EXPORTS
#define CODECDLL_API __declspec(dllexport)
#else
#define CODECDLL_API __declspec(dllimport)
#endif

#define single_chosen 1
#define coexisting_chosen 2
#define embedded_chosen 3
#define included_chosen 4

typedef double REAL64; // IEEE 754 encoding double precision real
typedef double REAL32; // IEEE 754 encoding single precision real

typedef unsigned int    TimeStamp;

#include <iostream>
#include <fstream>


// Class for codec
// exported from the CodecDLL.dll
class CODECDLL_API CCodec {
public:
	CCodec(); // constructor
	~CCodec(); // destructor
	bool Err(); // true if lastretcode is not 0
	int ErrNo(); // returns lastretcode
private:
	class impl;
	impl *pimpl;
	int lastretcode; // last return code
};



// All messages are objects of the CPDU class.
// 
// The class is constructed using the pimpl idiom. The implementation class is hidden to the user.
//
// Important to know:
// There are two private members of the implementation class.
// - LISTMODE DATA
//		Contains structured data. 
//		The class's member functions can be used to setup the data structure, and to read the elements
// - ENCODED DATA
//		Is just a number of bytes that contain encoded data
//
// To encode, you first populate the LISTMODE DATA using the class's functions. 
// Calling the ENCODE function will encode the LISTMODE DATA to the ENCODED DATA
// Then, LISTMODE DATA may be freed.
//
// To decode, you first copy the single encoded message, or a sequence of messages to ENCODED DATA 
// Then, by calling the DECODE function, the LISTMODE DATA structures are constructed and populated.
// After decoding, the ENCODED DATA may be freed.
//
class CODECDLL_API CPDU {
public:
	CPDU(); // constructor
	~CPDU(); // desturctor
	void PrintStructuredData(); // Print listmode data message as text
	void PrintEncodeddata(); // Print encoded data message in hex
	void PrintError(); // Print the last error

	int ReadCompleteEncodedDataFile(const char *filename); // Reads a binary file containing encoded data. Data can be one or more encodings.
	int ReadEncodedData(const unsigned char *data, long length); // passes pointers to encoded data, e.g. received 

	void PointAtFirstEvent(); // Set event pointer to first event
	void PointAtNextEvent(); // Advance event pointer to the next event
	bool EndOfEvents(); // Returns tue if event pointer is at the last event

	typedef enum  {
		ER_BER = 0,
		ER_PER_ALIGNED = 1,
		ER_PER_UNALIGNED = 2,
		ER_DER = 3,
		ER_ECN = 4,
		ER_XER = 5,
		ER_CXER = 6,
		ER_CER = 7,
		ER_EXER = 8,
		ER_OER = 9,
		ER_COER = 10,
		ER_JSON = 11
	} EncodingRules;

	int Encode(EncodingRules EncodingRule); // encode the listmode from 
	int Decode(EncodingRules EncodingRule); // decode

	void *GetListmodedata();// returns pointer to listmode data message
	unsigned char *GetEncodedData(); // return pointer to encoded data

	long GetEncodedDataLength(); // returns length of encoded data
	void PrepareforNextDecoding(); // prepare the emcoded data for the next decoding. To call after decoding a pdu.
	void ClearStructuredData();  // Free LISTMODE DATA
	void ClearEncodedData(); // Free ENCODED DATA

	typedef struct {
		long           length;
		unsigned char *value;
	} DataBuf;

	enum TypeOfListmodedata
	{
		header_chosen  = 1,
		events_chosen  = 2,
		footer_chosen  = 3
	};

	enum TypeOfEvent
	{
		eventNone = 0,
		eventPulse_chosen = 1,
		eventWaveform_chosen = 2,
		eventRollover_chosen = 3,
		eventTime_chosen = 4,
		eventRTC_chosen = 5,
		eventLogic_chosen = 6,
		eventGeo_chosen = 7,
		eventMeasurements_chosen = 8,
		eventHistograms_chosen = 9,
		eventMessage_chosen = 10
	};

	enum ChannelType {
		physicaltype = 0,
		virtualtype = 1
	};

	enum ChannelSyncStatus {
		synchronised = 0,
		unsynchronised = 1
	};

	enum ChannelEventPropertyType {
		eventpulse = 1,
		eventwaveform = 2,
		eventrollover = 3,
		eventtime = 4,
		eventRTC = 5,
		eventlogic = 6,
		eventgeo = 7,
		eventmeasurements = 8,
		eventhistograms = 9,
		eventmessage = 10
	};

	enum FlagType {
		logicstate = 0,
		logictransition = 1
	};

	enum RangeType {
		rangeOfReals32_chosen = 1,
		rangeOfReals64_chosen = 2,
		rangeOfInts_chosen = 3
	};

	enum TypeOfAccuracy{
		absolute = 0,
		relative = 1
	};

	enum TypeOfNumeric {
		real32_chosen = 1,
		real64_chosen = 2,
		Numeric_int_chosen = 3
	};

	enum TypeOfHistogram{
		pulseHeightAnalysis = 0,
		multiChannelScaling = 1,
		timeDistribution = 2
	};

	enum TypeOfAccumulation{
		full = 0,
		partial = 1
	};
	
	enum TypeOfGeoPosition {
		geographicPoint_chosen = 1,
		locationDescription_chosen = 2,
		relativeLocation_chosen = 3
	};

	void SetListmodedataType(TypeOfListmodedata _TypeOfListmodedata); // Sets the type of listmode data: header, events or footer
	TypeOfListmodedata GetListmodedataType(); // returns the type of listmode data: header, events or footer

	// Note: the functions with "reset" in their name are normally not required. 

	// Note: there is limited error checking.
	// For example, specifying parameters that are out of range could result in a crash
	// There is also no check against the requirements in the standard. 
	// E.g. the presence of a corresponding eventProperty type is not verified.

	////////////
	// Header // 
	////////////

	// StandardID
	char* H_GetStandardID(); // returns the standard ID. Note that there is no function to set the standard ID: it is fixed.

	// ListmodeDataID
	void H_SetListmodeDataID(const char _listmodedataid[]);
	char* H_GetListmodeDataID();
	void H_ResetListmodeDataID(); 
	
	// MeasSetupID
	void H_SetMeasSetupID(const char _meassetupid[]);
	char* H_GetMeasSetupID();
	void H_ResetMeasSetupID();

	// MeasSetupDescription
	void H_SetMeasSetupDescription(const char _meassetupdescription[]);
	char* H_GetMeasSetupDescription();
	void H_ResetMeasSetupDescription();

	// iec62755 is set by calling one of the four following functions.
	void H_SetIec62755single();
	void H_SetIec62755coexisting(const char _coexisting[]);
	void H_SetIec62755embedded(const char _embedded[]);
	void H_SetIec62755included(const char _includedvalue[], unsigned int _includedlength);
	void H_GetIec62755(unsigned short &relation, unsigned char **data, unsigned int &length);
	
	// radSource
	void H_SetRadSource(const char _radSource[]);
	char* H_GetRadSource();
	void H_ResetRadSource();

	// start
	void H_SetStart(const char _Start[], REAL64 _FractionalSeconds);
	char* H_GetStartDateTime();
	REAL64 H_GetStartFractionalSeconds();
	
	// startAccuracy
	void H_SetStartAccuracy(REAL64 _StartAccuracy);
	void H_ResetStartAccuracy();
	bool H_StartAccuracyPresent(); // Returns true if startaccuracy is present (it is an optional element)
	REAL64 H_GetStartAccuracy();
	
	// delay
	void H_SetDelay(int _Delay);
	void H_ResetDelay();
	bool H_DelayPresent();
	int H_GetDelay();

	// devices
	// note: there is no function to remove a device
	// Devices are identified by their DeviceID, which starts at 0 and increments by one.
	void H_AddDevices(unsigned int _nDevices); // Only call once. _nDevices > 0. No error trapping!
	unsigned int H_GetNDevices(); // returns the number of devices
	void H_SetDeviceName(unsigned int _DeviceID, const char _DeviceName[]);
	void H_SetDeviceManuf(unsigned int _DeviceID, const char _DeviceManuf[]);
	void H_SetDeviceModel(unsigned int _DeviceID, const char _DeviceModel[]);
	void H_SetDeviceSerial(unsigned int _DeviceID, const char _DeviceSerial[]);
	void H_ResetDeviceName(unsigned int _DeviceID);
	void H_ResetDeviceManuf(unsigned int _DeviceID);
	void H_ResetDeviceModel(unsigned int _DeviceID);
	void H_ResetDeviceSerial(unsigned int _DeviceID);
	char* H_GetDeviceName(unsigned int _DeviceID);
	char* H_GetDeviceManuf(unsigned int _DeviceID);
	char* H_GetDeviceModel(unsigned int _DeviceID);
	char* H_GetDeviceSerial(unsigned int _DeviceID);

	// channels
	// note: there is no function to remove a channel
	// Channels are identified by their ChannelID, which starts at 0 and increments by one, irrespective of the DeviceID.
	void H_AddChannels(unsigned int _nChannels); // Only call once. _nChannels > 0 and _nChannels >= _nDevices. No error trapping!
	unsigned int H_GetNChannels(); // returns the number of channels
	void H_SetChannelDeviceID(unsigned int _ChannelID, unsigned int _DeviceID); // Assigns the channel to a device.
	unsigned int H_GetChannelDeviceID(unsigned int _ChannelID); // Get the device where the channel is assigen to
	void H_SetChannelType(unsigned int _ChannelID, ChannelType _ChannelType); // Set type of channel: physical, virtual
	ChannelType H_GetChannelType(unsigned int _ChannelID); // Get type of channel: physical, virtual
	void H_SetPhysicalChannel(unsigned int _ChannelID, unsigned int _PhysicalChannel); // Set the number of the physical channel on the device
	void H_ResetPhysicalChannel(unsigned int _ChannelID); 
	unsigned int  H_GetPhysicalChannel(unsigned int _ChannelID); // Returns physical channel number. Check first if present with H_PhysicalChannelPresent
	bool H_PhysicalChannelPresent(unsigned int _ChannelID); // returns true if a physical channel number has been assigned
	void H_SetChannelName(unsigned int _ChannelID, const char _ChannelName[]); // set the name of the channel
	void H_ResetChannelName(unsigned int _ChannelID); // clear the name of the channel
	char* H_GetChannelName(unsigned int _ChannelID); // returns NULL if channel name is not present
	void H_SetChannelDescription(unsigned int _ChannelID, const char _ChannelDescription[]);
	void H_ResetChannelDescription(unsigned int _ChannelID);
	char* H_GetChannelDescription(unsigned int _ChannelID); // returns NULL if not present
	void H_SetChannelParameters(unsigned int _ChannelID, const char _ChannelParameters[]);
	void H_ResetChannelParameters(unsigned int _ChannelID);
	char* H_GetChannelParameters(unsigned int _ChannelID);// returns NULL if not present
	void H_SetChannelRefClock(unsigned int _ChannelID, const char _ChannelRefClock[]);
	void H_ResetChannelRefClock(unsigned int _ChannelID);
	char* H_GetChannelRefClock(unsigned int _ChannelID);// returns NULL if not present
	void H_SetChannelSyncStatus(unsigned int _ChannelID, ChannelSyncStatus _SyncStatus);
	void H_ResetChannelSyncStatus(unsigned int _ChannelID);
	ChannelSyncStatus H_GetChannelSyncStatus(unsigned int _ChannelID);
	bool H_ChannelSyncStatusPresent(unsigned int _ChannelID);
	void H_SetChannelADCSamplingRate(unsigned int _ChannelID, unsigned int _ADCSamplingRate);
	unsigned int H_GetChannelADCSamplingRate(unsigned int _ChannelID);
	void H_SetChannelADCBitResolution(unsigned int _ChannelID, unsigned int _ADCBitResolution);
	unsigned int H_GetChannelADCBitResolution(unsigned int _ChannelID);
	void H_SetChannelTicksPerSecond(unsigned int _ChannelID, unsigned int _TicksPerSecond);
	unsigned int H_GetChannelTicksPerSecond(unsigned int _ChannelID);

	// Header - Channel - EventProperties
	void H_AddChannelEventProperties(unsigned int _ChannelID, unsigned int _nEventProperties); // Add a number of event properties to the channel
	void H_SetChannelEventPropertyType(unsigned int _ChannelID, unsigned int _eventPropertyID, ChannelEventPropertyType _eventPropertyType); // Set the type of event property
	ChannelEventPropertyType H_GetChannelEventPropertyType(unsigned int _ChannelID, unsigned int _eventPropertyID);
	void H_SetChannelEventPropertyDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, const char _Description[]);
	char* H_GetChannelEventPropertyDescription(unsigned int _ChannelID, unsigned int _eventPropertyID);
	void H_ResetChannelEventPropertyDescription(unsigned int _ChannelID, unsigned int _eventPropertyID);
	unsigned int H_GetChannelNEventProperties(unsigned int _ChannelID); // Get number of event properties in the channel

	// Header - Channel - EventProperties - EventPulseProperty - Values
	void H_AddChannelEventPulsePropertyValues(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nValues); 
	void H_SetChannelEventPulsePropertyValueDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _ValueID, const char _Description[]);
	char* H_GetChannelEventPulsePropertyValueDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _ValueID);
	void H_ResetChannelEventPulsePropertyValues(unsigned int _ChannelID, unsigned int _eventPropertyID);
	unsigned int H_GetChannelEventPulsePropertyNValues(unsigned int _ChannelID, unsigned int _eventPropertyID); // returns number of values
	
	// Header - Channel - EventProperties - EventPulseProperty - Flags
	void H_AddChannelEventPulsePropertyFlags(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nFlags);
	void H_SetChannelEventPulsePropertyFlag(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID, const char _Description[], FlagType _flagtype);
	char* H_GetChannelEventPulsePropertyFlagDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID);
	FlagType H_GetChannelEventPulsePropertyFlagType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID);
	void H_ResetChannelEventPulsePropertyFlags(unsigned int _ChannelID, unsigned int _eventPropertyID);
	unsigned int H_GetChannelEventPulsePropertyNFlags(unsigned int _ChannelID, unsigned int _eventPropertyID); // returns number of flags
	
	// Header - Channel - EventProperties - EventWaveformProperty - Flags
	void H_AddChannelEventWaveformPropertyFlags(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nFlags);
	void H_SetChannelEventWaveformPropertyFlag(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID, const char _Description[], FlagType _flagtype);
	char* H_GetChannelEventWaveformPropertyFlagDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID);
	FlagType H_GetChannelEventWaveformPropertyFlagType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID);
	void H_ResetChannelEventWaveformPropertyFlags(unsigned int _ChannelID, unsigned int _eventPropertyID);
	unsigned int H_GetChannelEventWaveformPropertyNFlags(unsigned int _ChannelID, unsigned int _eventPropertyID); // returns number of flags

	// Header - Channel - EventProperties - EventWaveformProperty - Waveform signal properties
	void H_AddChannelEventWaveformSignalProperties(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nSignals);
	unsigned int H_GetChannelEventWaveformNSignals(unsigned int _ChannelID, unsigned int _eventPropertyID); // number of signals in the waveform
	void H_ChannelEventWaveformSetSignalDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal, const char _Description[]);
	char* H_ChannelEventWaveformGetSignalDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	void H_ChannelEventWaveformResetSignalDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	void H_ChannelEventWaveformSetSignalSamplingRate(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal, unsigned int _SamplingRate);
	bool H_ChannelEventWaveformSignalSamplingRatePresent(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	unsigned int H_ChannelEventWaveformGetSignalSamplingRate(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	void H_ChannelEventWaveformResetSignalSamplingRate(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	void H_ChannelEventWaveformSetSignalUnit(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal, const char _Unit[]);
	char* H_ChannelEventWaveformGetSignalUnit(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	void H_ChannelEventWaveformResetSignalUnit(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	void H_ChannelEventWaveformSetSignalRangeREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal, REAL32 _Lowerbound, REAL32 _Upperbound);
	void H_ChannelEventWaveformSetSignalRangeREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal, REAL64 _Lowerbound, REAL64 _Upperbound);
	void H_ChannelEventWaveformSetSignalRangeInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal, int _Lowerbound, int _Upperbound);
	void H_ChannelEventWaveformResetSignalRange(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	bool H_ChannelEventWaveformSignalRangePresent(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	RangeType H_ChannelEventWaveformGetSignalRangeType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	REAL32 H_ChannelEventWaveformGetSignalRangeLowerboundREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	REAL32 H_ChannelEventWaveformGetSignalRangeUpperboundREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	REAL64 H_ChannelEventWaveformGetSignalRangeLowerboundREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	REAL64 H_ChannelEventWaveformGetSignalRangeUpperboundREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	int H_ChannelEventWaveformGetSignalRangeLowerboundInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);
	int H_ChannelEventWaveformGetSignalRangeUpperboundInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Signal);

	// Header - Channel - EventProperties - EventRolloverProperty 
	void H_ChannelEventRolloverSetRolloverticks(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _RolloverTicks);
	unsigned int H_ChannelEventRolloverGetRolloverticks(unsigned int _ChannelID, unsigned int _eventPropertyID);
	
	// Header - Channel - EventProperties - EventLogic - Flags
	void H_AddChannelEventLogicPropertyFlags(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nFlags);
	void H_SetChannelEventLogicPropertyFlag(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID, const char _Description[], FlagType _flagtype);
	char* H_GetChannelEventLogicPropertyFlagDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID);
	FlagType H_GetChannelEventLogicPropertyFlagType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _FlagID);
	void H_ResetChannelEventLogicPropertyFlags(unsigned int _ChannelID, unsigned int _eventPropertyID);
	unsigned int H_GetChannelEventLogicPropertyNFlags(unsigned int _ChannelID, unsigned int _eventPropertyID); // returns number of flags
	
	// Header - Channel - EventProperties - EventMeasurementProperty - Properties
	void H_AddChannelEventMeasurementProperties(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nMeasurements);
	void H_ChannelEventMeasurementSetMeasurementDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, const char _Description[]);
	char* H_ChannelEventMeasurementGetMeasurementDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_ChannelEventMeasurementResetMeasurementDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_ChannelEventMeasurementSetMeasurementQuantity(int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, const char _Quantity[]);
	char* H_ChannelEventMeasurementGetMeasurementQuantity(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_ChannelEventMeasurementResetMeasurementQuantity(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_ChannelEventMeasurementSetMeasurementUnit(int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, const char _Unit[]);
	char* H_ChannelEventMeasurementGetMeasurementUnit(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_ChannelEventMeasurementResetMeasurementUnit(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_SetChannelEventMeasurementAccuracyREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, REAL32 _accuracy);
	void H_SetChannelEventMeasurementAccuracyREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, REAL64 _accuracy);
	void H_SetChannelEventMeasurementAccuracyInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, int _accuracy);
	REAL32 H_GetChannelEventMeasurementAccuracyREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	REAL64 H_GetChannelEventMeasurementAccuracyREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	int H_GetChannelEventMeasurementAccuracyInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	TypeOfNumeric H_GetChannelEventMeasurementTypeOfNumeric(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_ResetChannelEventMeasurementAccuracy(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	bool H_ChannelEventMeasurementAccuracyPresent(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	void H_SetChannelEventMeasurementAccuracyType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID, TypeOfAccuracy _AccuracyType);
	void H_ResetChannelEventMeasurementAccuracyType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	TypeOfAccuracy H_GetChannelEventMeasurementAccuracyType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	bool H_ChannelEventMeasurementAccuracyTypePresent(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _MeasurementID);
	unsigned int H_ChannelEventMeasurementGetNMeasurements(unsigned int _ChannelID, unsigned int _eventPropertyID); // returns number of measurements


	// Header - Channel - EventProperties - EventHistogramsProperty - Properties
	void H_AddChannelEventHistogramProperties(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _nHistograms);
	unsigned int H_ChannelEventHistogramGetNHistograms(unsigned int _ChannelID, unsigned int _eventPropertyID);
	void H_ChannelEventHistogramSetHistogramDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram, const char _Description[]);
	char* H_ChannelEventHistogramGetHistogramDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramResetHistogramDescription(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramSetHistogramQuantityX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram, const char _QuantityX[]);
	char* H_ChannelEventHistogramGetHistogramQuantityX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramResetHistogramQuantityX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramSetHistogramQuantityY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram, const char _QuantityY[]);
	char* H_ChannelEventHistogramGetHistogramQuantityY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramResetHistogramQuantityY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramSetHistogramUnitX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram, const char _UnitX[]);
	char* H_ChannelEventHistogramGetHistogramUnitX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramResetHistogramUnitX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramSetHistogramUnitY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram, const char _UnitY[]);
	char* H_ChannelEventHistogramGetHistogramUnitY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramResetHistogramUnitY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _Histogram);
	void H_ChannelEventHistogramSetHistogramType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, TypeOfHistogram _TypeOfHistogram);
	TypeOfHistogram H_ChannelEventHistogramGetHistogramType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	void H_ChannelEventHistogramSetHistogramAccumulation(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, TypeOfAccumulation _TypeOfAccumulation);
	TypeOfAccumulation H_ChannelEventHistogramGetHistogramAccumulation(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	void H_ChannelEventHistogramSetHistogramnBins(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, unsigned int _nBins);
	unsigned int H_ChannelEventHistogramGetHistogramnBins(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	void H_ChannelEventHistogramSetRangeXREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, REAL32 _Lowerbound, REAL32 _Upperbound);
	void H_ChannelEventHistogramSetRangeXREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, REAL64 _Lowerbound, REAL64 _Upperbound);
	void H_ChannelEventHistogramSetRangeXInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, int _Lowerbound, int _Upperbound);
	void H_ChannelEventHistogramResetRangeX(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	bool H_ChannelEventHistogramRangeXPresent(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	RangeType H_ChannelEventHistogramGetRangeXType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL32 H_ChannelEventHistogramGetRangeXLowerboundREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL64 H_ChannelEventHistogramGetRangeXLowerboundREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	int H_ChannelEventHistogramGetRangeXLowerboundInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL32 H_ChannelEventHistogramGetRangeXUpperboundREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL64 H_ChannelEventHistogramGetRangeXUpperboundREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	int H_ChannelEventHistogramGetRangeXUpperboundInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	void H_ChannelEventHistogramSetRangeYREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, REAL32 _Lowerbound, REAL32 _Upperbound);
	void H_ChannelEventHistogramSetRangeYREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, REAL64 _Lowerbound, REAL64 _Upperbound);
	void H_ChannelEventHistogramSetRangeYInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID, int _Lowerbound, int _Upperbound);
	void H_ChannelEventHistogramResetRangeY(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	bool H_ChannelEventHistogramRangeYPresent(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	RangeType H_ChannelEventHistogramGetRangeYType(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL32 H_ChannelEventHistogramGetRangeYLowerboundREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL64 H_ChannelEventHistogramGetRangeYLowerboundREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	int H_ChannelEventHistogramGetRangeYLowerboundInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL32 H_ChannelEventHistogramGetRangeYUpperboundREAL32(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	REAL64 H_ChannelEventHistogramGetRangeYUpperboundREAL64(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	int H_ChannelEventHistogramGetRangeYUpperboundInt(unsigned int _ChannelID, unsigned int _eventPropertyID, unsigned int _HistogramID);
	
	////////////
	// Events //
	////////////

	// Events - ListmodeDataID
	void E_SetListmodeDataID(const char _listmodedataid[]);
	char* E_GetListmodeDataID();
	void E_ResetListmodeDataID();
	
	// Events - id
	unsigned int E_GetEventsID();

	TypeOfEvent E_GetEventType(unsigned int _EventID); // get the type of event
	TypeOfEvent E_GetEventType(); // get the type of event of event pointed at by pEventToRead

	// Events - EventPulse
	void E_AddEventPulse(unsigned int _ChannelID, TimeStamp _TimeStamp); // Add an event of the pulse type. Set _ChannelID and/or _TimeStamp to 0 to disable encoding.
	void E_AddEventPulseValueREAL32(REAL32 _Value); // Add one REAL32 value to the last event pulse. Repeat calling this function for all values.
	void E_AddEventPulseValueREAL64(REAL64 _Value); // Add one REAL64 value to the last event pulse. Repeat calling this function for all values.
	void E_AddEventPulseValueInt(int _Value); // Add one Int value to the last event pulse. Repeat calling this function for all values.
	void E_SetEventPulseFlags(unsigned int _nFlags, const unsigned char _FlagsBitstring[]); // Set the flags of last event pulse
	unsigned int E_GetEventPulseChannelID(); // Returns channel ID, or 0 if channel ID is not encoded
	bool E_EventPulseTimeStampPresent();
	TimeStamp E_GetEventPulseTimeStamp(); // Returns timestamp 
	void E_GetEventPulseFlags( unsigned int &_nFlags, unsigned char **_FlagsBitstring); // returns flags. nFlags = 0 when no flags
	unsigned int E_GetEventPulseNValues(); // returns number of values in the pulse
	TypeOfNumeric E_GetEventPulseTypeOfNumericValue( unsigned int _ValueID); // returns the type of numeric value
	REAL32 E_GetEventPulseValueREAL32( unsigned int _ValueID); // Get the REAL32 value identified by _EventID and _ValueID
	REAL64 E_GetEventPulseValueREAL64( unsigned int _ValueID);// Get the REAL64 value identified by _EventID and _ValueID
	int E_GetEventPulseValueInt( unsigned int _ValueID);// Get the int value identified by _EventID and _ValueID
	
	// Events - EventWaveform
	void E_AddEventWaveform(unsigned int _ChannelID, TimeStamp _TimeStamp); // Add an event of the waveform type. Set _ChannelID and/or _TimeStamp to 0 to disable encoding.
	void E_SetEventWaveformFlags(unsigned int _nFlags, const unsigned char _FlagsBitstring[]); // Set the flags of last event waveform
	void E_GetEventWaveformFlags(unsigned int &_nFlags, unsigned char **_FlagsBitstring); // returns flags. nFlags = 0 when no flags
	unsigned int E_GetEventWaveformChannelID(); // Returns channel ID, or 0 if channel ID is not encoded
	bool E_EventWaveformTimeStampPresent();
	TimeStamp E_GetEventWaveformTimeStamp(); // Returns timestamp 
	unsigned int E_GetEventWaveformNSignals(); // returns number of signals in the waveform
	void E_AddEventWaveformSignal(); // Add a signal
	void E_SetEventWaveformSignalSamplingRate(unsigned int _SamplingRate); // Sets the signal sampling rate of the last signal
	bool E_EventWaveformSignalSamplingRatePresent(unsigned int _SignalID); // Check if sampling rate is present
	unsigned int E_GetEventWaveformSignalSamplingRate(unsigned int _SignalID); // Get the sampling rate fo the signal
	void E_SetEventWaveformSignalRangeREAL32(REAL32 _Lowerbound, REAL32 _Upperbound); // Sets the signal range of the last signal
	void E_SetEventWaveformSignalRangeREAL64(REAL64 _Lowerbound, REAL64 _Upperbound); // Sets the signal range of the last signal
	void E_SetEventWaveformSignalRangeInt(int _Lowerbound, int _Upperbound); // Sets the signal range of the last signal
	bool E_EventWaveformSignalRangePresent(unsigned int _SignalID); // Check if signal range is present
	RangeType E_GetEventWaveformSignalRangeType(unsigned int _SignalID); // Get the type of signal range
	void E_GetEventWaveformSignalRangeREAL32(unsigned int _SignalID, REAL32 &_Lowerbound, REAL32 &_Upperbound); // Get the signal range 
	void E_GetEventWaveformSignalRangeREAL64(unsigned int _SignalID, REAL64 &_Lowerbound, REAL64 &_Upperbound); // Get the signal range 
	void E_GetEventWaveformSignalRangeInt(unsigned int _SignalID, int &_Lowerbound, int &_Upperbound); // Get the signal range 
	void E_SetEventWaveformSignalSamplesREAL32(unsigned int _nSamples, REAL32 _Samples[]); // Set the samples of the last signal
	void E_SetEventWaveformSignalSamplesREAL64(unsigned int _nSamples, REAL64 _Samples[]); // Set the samples of the last signal
	void E_SetEventWaveformSignalSamplesInt(unsigned int _nSamples, int _Samples[]); // Set the samples of the last signal
	TypeOfNumeric E_GetEventWaveformSignalSampleType(unsigned int _SignalID); // Get the type of signal samples
	unsigned int E_GetEventWaveformSignalNSamples(unsigned int _SignalID); // Get number of samples in the signal
	void E_GetEventWaveformSignalSamplesREAL32(unsigned int _SignalID, REAL32 *_Samples); // Get the samples 
	void E_GetEventWaveformSignalSamplesREAL64(unsigned int _SignalID, REAL64 *_Samples); // Get the samples 
	void E_GetEventWaveformSignalSamplesInt(unsigned int _SignalID, int *_Samples); // Get the samples 


	// Events - EventTime
	// Set _SinceStart = true and set _TotalDeadTimeSinceStart and _TotalLiveTimeSinceStart
	// and/or, set _SincePrevious = true and set _TotalDeadTimeSincePrevious and _TotalLiveTimeSincePrevious
	// At least one of the two should be set. Set unused parameters 0.
	void E_AddEventTime(unsigned int _ChannelID, TimeStamp _TimeStamp, 
		bool _SinceStart, unsigned int _TotalDeadTimeSinceStart, unsigned int _TotalLiveTimeSinceStart,
		bool _SincePrevious, unsigned int _TotalDeadTimeSincePrevious, unsigned int _TotalLiveTimeSincePrevious);
	void E_GetEventTime(unsigned int &_ChannelID, TimeStamp &_TimeStamp,
		bool &_SinceStart, unsigned int &_TotalDeadTimeSinceStart, unsigned int &_TotalLiveTimeSinceStart,
		bool &_SincePrevious, unsigned int &_TotalDeadTimeSincePrevious, unsigned int &_TotalLiveTimeSincePrevious);

	// Events - EventRollover
	void E_AddEventRollover(unsigned int _ChannelID, unsigned int _rollovers);
	void E_GetEventRollover(unsigned int &_channelID, unsigned int &_rollovers);

	// Events - EventRTC
	void E_AddEventRTCrealtimeclock(unsigned int _ChannelID, TimeStamp _TimeStamp, const char _RealTimeClock[], REAL64 _FractionalSeconds);
	void E_AddEventRTCsyncstatus(unsigned int _ChannelID, TimeStamp _TimeStamp, ChannelSyncStatus _SyncStatus);
	void E_GetEventRTC(unsigned int &_ChannelID, TimeStamp &_TimeStamp, char **_RealTimeClock, REAL64 &_FractionalSeconds, ChannelSyncStatus &_SyncStatus);

	// Events - EventLogic
	// Bitstring: 1 byte is consumed for up to 8 bits (flags)
	// 1 flag  value = 0: 0x00
	// 1 flag  value = 1: 0x80
	// 2 flags value = 00: 0x00
	// 2 flags value = 01: 0x40
	// 2 flags value = 10: 0x80
	// 2 flags value = 11: 0xC0 etc
	void E_AddEventLogic(unsigned int _ChannelID, TimeStamp _TimeStamp, unsigned int _nFlags, const unsigned char _FlagsBitstring[]);
	void E_GetEventLogic(unsigned int &_ChannelID, TimeStamp &_TimeStamp, unsigned int &_nFlags, unsigned char **_FlagsBitstring);
	
	// Events - EventGeo
	void E_AddEventGeo(unsigned int _ChannelID, TimeStamp _TimeStamp); // Adds a event geo to list
	unsigned int E_GetEventGeoChannelID();
	bool E_EventGeoTimeStampPresent();
	TimeStamp E_GetEventGeoTimeStamp();
	
	// Events - EventGeo - Position - Geographic point
	void E_SetEventGeoPositionAsGeographicPoint(REAL64 _Latitude, REAL64 _Longitude);
	void E_SetEventGeoPositionAsGeographicPointSetElevation(REAL64 _Elevation); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	void E_SetEventGeoPositionAsGeographicPointSetElevationOffset(REAL64 _ElevationOffset); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	void E_SetEventGeoPositionAsGeographicPointSetGeoPointAccuracy(REAL64 _GeoPointAccuracy); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	void E_SetEventGeoPositionAsGeographicPointSetElevationAccuracy(REAL64 _ElevationAccuracy); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	void E_SetEventGeoPositionAsGeographicPointSetElevationOffsetAccuracy(REAL64 _ElevationOffsetAccuracy);// optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	void E_SetEventGeoPositionAsGeographicPointSetDatum(const char _Datum[]);// optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionPresent();
	TypeOfGeoPosition E_GetEventGeoPositionType( );
	REAL64 E_GetEventGeoPositionAsGeographicPointLatitude( );
	REAL64 E_GetEventGeoPositionAsGeographicPointLongitude( );
	bool E_EventGeoPositionAsGeographicPointElevationPresent( );
	REAL64 E_GetEventGeoPositionAsGeographicPointElevation( );
	bool E_EventGeoPositionAsGeographicPointElevationOffsetPresent( );
	REAL64 E_GetEventGeoPositionAsGeographicPointElevationOffset( );
	bool E_EventGeoPositionAsGeographicPointGeoPointAccuracyPresent( );
	REAL64 E_GetEventGeoPositionAsGeographicPointGeoPointAccuracy( );
	bool E_EventGeoPositionAsGeographicPointElevationAccuracyPresent( );
	REAL64 E_GetEventGeoPositionAsGeographicPointElevationAccuracy( );
	bool E_EventGeoPositionAsGeographicPointElevationOffsetAccuracyPresent( );
	REAL64 E_GetEventGeoPositionAsGeographicPointElevationOffsetAccuracy( );
	bool E_EventGeoPositionAsGeographicPointDatumPresent( );
	unsigned char * E_SetEventGeoPositionAsGeographicPointDatum( );

	// Events - EventGeo - Position - Description
	void E_SetEventGeoPositionAsLocationDescription(const char _LocationDescription[]);
	unsigned char *E_GetEventGeoPositionLocationDescription();
	// Events - EventGeo - Position - Relative location
	void E_SetEventGeoPositionAsRelativeLocation();
	void E_SetEventGeoPositionAsRelativeLocationSetAzimuth(REAL64 _Azimuth);
	bool E_EventGeoPositionAsRelativeLocationAzimuthPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationAzimuth();
	void E_SetEventGeoPositionAsRelativeLocationSetInclination(REAL64 _Inclination);
	bool E_EventGeoPositionAsRelativeLocationInclinationPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationInclination();
	void E_SetEventGeoPositionAsRelativeLocationSetDistance(REAL64 _Distance);
	bool E_EventGeoPositionAsRelativeLocationDistancePresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationDistance();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsDescription(const char _OriginDescription[]);
	bool E_EventGeoPositionAsRelativeLocationOriginAsDescriptionPresent();
	unsigned char *E_GetEventGeoPositionAsRelativeLocationOriginAsDescription();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPoint(REAL64 _Latitude, REAL64 _Longitude);
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointLatidude();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointLongitude();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevation(REAL64 _Elevation); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevation();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevationOffset(REAL64 _ElevationOffset); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffsetPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffset();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetGeoPointAccuracy(REAL64 _GeoPointAccuracy); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointGeoPointAccuracyPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointGeoPointAccuracy();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevationAccuracy(REAL64 _ElevationAccuracy); // optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationAccuracyPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationAccuracy();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetElevationOffsetAccuracy(REAL64 _ElevationOffsetAccuracy);// optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffsetAccuracyPresent();
	REAL64 E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointElevationOffsetAccuracy();
	void E_SetEventGeoPositionAsRelativeLocationSetOriginAsGeographicPointSetDatum(const char _Datum[]);// optionally, to call after E_SetEventGeoPositionAsGeographicPoint
	bool E_EventGeoPositionAsRelativeLocationOriginAsGeographicPointDatumPresent();
	unsigned char * E_GetEventGeoPositionAsRelativeLocationOriginAsGeographicPointDatum();
	// Events - EventGeo - Orientation
	void E_SetEventGeoOrientation();
	void E_SetEventGeoOrientationSetAzimth(REAL64 _Azimuth);
	void E_SetEventGeoOrientationSetInclination(REAL64 _Inclination);
	void E_SetEventGeoOrientationSetRoll(REAL64 _Roll);
	bool E_EventGeoOrientationPresent();
	bool E_EventGeoOrientationAzimuthPresent();
	REAL64 E_GetEventGeoOrientationAzimuth();
	bool   E_EventGeoOrientationInclinationPresent();
	REAL64 E_GetEventGeoOrientationInclination();
	bool   E_EventGeoOrientationRollPresent();
	REAL64 E_GetEventGeoOrientationRoll();
	// Events - EventGeo - Speed
	void E_SetEventGeoSpeed(REAL64 _Speed);
	bool E_EventGeoSpeedPresent();
	REAL64 E_GetEventGeoSpeed();


	// Events - EventMeasurements
	void E_AddEventMeasurements(unsigned int _ChannelID, TimeStamp _TimeStamp); // Adds an event of the Measurements type. Se parameters 0 for default (not encoded). Call this first and then call E_AddEventMeasurementValuexxx to add individual measurements
	void E_AddEventMeasurementValueREAL32(REAL32 _Value); // Adds a measurement value, without accuracy
	void E_AddEventMeasurementValueREAL32(REAL32 _Value, REAL32 _Accuracy); // Adds a measurement value, with accuracy
	void E_AddEventMeasurementValueREAL64(REAL64 _Value); // Adds a measurement value, without accuracy
	void E_AddEventMeasurementValueREAL64(REAL64 _Value, REAL64 _Accuracy); // Adds a measurement value, with accuracy
	void E_AddEventMeasurementValueInt(int _Value); // Adds a measurement value, without accuracy
	void E_AddEventMeasurementValueInt(int _Value, int _Accuracy); // Adds a measurement value, with accuracy
	unsigned int E_GetEventMeasurementsChannelID(); // Get the channel ID of the measurements event
	bool E_EventMeasurementsTimestampPresent(); // returns true if there is a timestamp
	TimeStamp E_GetEventMeasurementsTimestamp(); // returns timestamp
	unsigned int E_GetEventMeasurementsNMeasurements(); // returns number of measurements
	TypeOfNumeric E_GetEventMeasurementTypeOfNumericValue(unsigned int _MeasurementID); // returns the type of numeric value
	bool E_EventMeasurementAccuracyPresent(unsigned int _MeasurementID); // returns true if the measurement has an accuracy
	void E_GetEventMeasurementValueREAL32(unsigned int _MeasurementID, REAL32 &_Value, REAL32 &_Accuracy); // returns measurement and accuracy. Accuracy is invalid if not encoded. Check with E_EventMeasurementAccuracyPresent
	void E_GetEventMeasurementValueREAL64(unsigned int _MeasurementID, REAL64 &_Value, REAL64 &_Accuracy); // returns measurement and accuracy. Accuracy is invalid if not encoded. Check with E_EventMeasurementAccuracyPresent
	void E_GetEventMeasurementValueInt(unsigned int _MeasurementID, int &_Value, int &_Accuracy); // returns measurement and accuracy. Accuracy is invalid if not encoded. Check with E_EventMeasurementAccuracyPresent

	// Events - Histograms
	void E_AddEventHistograms(unsigned int _ChannelID, TimeStamp _TimeStamp); // Adds an event of the Histograms type. Se parameters 0 for default (not encoded). 
	unsigned int E_GetEventHistogramsChannelID(); // Get the channel ID of the  event
	bool E_EventHistogramsTimestampPresent(); // returns true if there is a timestamp
	TimeStamp E_GetEventHistogramsTimestamp(); // returns timestamp
	void E_AddEventHistogramsAllBinsREAL32(unsigned int _nBins, REAL32 _BinContent[]); // Add a histogram to the Histograms event's list of histograms
	void E_AddEventHistogramsAllBinsREAL64(unsigned int _nBins, REAL64 _BinContent[]); // Add a histogram to the Histograms event's list of histograms
	void E_AddEventHistogramsAllBinsInt(unsigned int _nBins, int _BinContent[]); // Add a histogram to the Histograms event's list of histograms
	void E_AddEventHistogramsBinListREAL32(unsigned int _nBins, unsigned int _BinNumber[], REAL32 _BinContent[]);// Add a histogram to the Histograms event's list of histograms. Specify content of individual bins
	void E_AddEventHistogramsBinListREAL64(unsigned int _nBins, unsigned int _BinNumber[], REAL64 _BinContent[]);// Add a histogram to the Histograms event's list of histograms. Specify content of individual bins
	void E_AddEventHistogramsBinListInt(unsigned int _nBins, unsigned int _BinNumber[], int _BinContent[]);// Add a histogram to the Histograms event's list of histograms. Specify content of individual bins
	TypeOfNumeric E_GetEventHistogramTypeOfNumeric(unsigned int _HistogramID); // Get the numeric type of the bin content of histogram with id _HistogramID
	bool E_EventHistogramsAllBinsChosen(unsigned int _HistogramID); // Returns true if histogram contains all bins, false if histogram is specified as sequence of (binnumber, bincontent)
	unsigned int E_GetEventHistogramsNHistograms(); // returns number of histograms
	unsigned int E_GetEventHistogramsNBins(unsigned int _HistogramID); // returns number of bins in the histogram (specified with allbins or binlist)
	void E_GetEventHistogramsAllBinsREAL32(unsigned int _HistogramID, REAL32 *_BinContent); // Return a histogram 
	void E_GetEventHistogramsAllBinsREAL64(unsigned int _HistogramID, REAL64 *_BinContent); // Return a histogram 
	void E_GetEventHistogramsAllBinsInt(unsigned int _HistogramID, int *_BinContent); // Return a histogram 
	void E_GetEventHistogramsBinListREAL32(unsigned int _HistogramID, unsigned int *_BinNumber, REAL32 *_BinContent); // Return a histogram, specified by bin list
	void E_GetEventHistogramsBinListREAL64(unsigned int _HistogramID, unsigned int *_BinNumber, REAL64 *_BinContent); // Return a histogram, specified by bin list
	void E_GetEventHistogramsBinListInt(unsigned int _HistogramID, unsigned int *_BinNumber, int *_BinContent); // Return a histogram, specified by bin list



	// Events - EventMessage
	void E_AddEventMessage(unsigned int _ChannelID, const char _message[], TimeStamp _timestamp = 0);
	void E_GetEventMessage(unsigned int &_channelID, TimeStamp &_timestamp, unsigned char **_message); // timestamp = 0 when not encoded
	


	////////////
	// Footer // 
	////////////

	// ListmodeDataID
	void F_SetListmodeDataID(const char _listmodedataid[]);
	char* F_GetListmodeDataID();
	void F_ResetListmodeDataID();

	// Footer - Stop
	void F_SetStop(const char _Start[], REAL64 _FractionalSeconds);
	char* F_GetStopDateTime();
	REAL64 F_GetStopFractionalSeconds();
	void F_ResetStop();

	// Footer - Total dead time
	void F_SetTotalDeadTime(unsigned int _totaldeadtime);
	void F_ResetTotalDeadTime();
	bool F_TotalDeadTimePresent();
	unsigned int F_GetTotalDeadTime();

	// Footer - Total live time
	void F_SetTotalLiveTime(unsigned int _totallivetime);
	void F_ResetTotalLiveTime();
	bool F_TotalLiveTimePresent();
	unsigned int F_GetTotalLiveTime();
	
private:
	class impl;
	impl *pimpl;
	int retcode;
};


