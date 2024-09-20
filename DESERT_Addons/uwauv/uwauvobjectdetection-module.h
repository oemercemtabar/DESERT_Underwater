//
// Copyright (c) 2017 Regents of the SIGNET lab, University of Padova.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University of Padova (SIGNET lab) nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
* @file uwauv-module.h
* @author Federico Chiariotti Filippo Campagnaro Omer Cem Tabar
* @version 1.0.0
*
* \brief Provides the definition of the class <i>UWAUVErrorObjectDetection</i> with Object Detection Module.
*
* Provides the definition of the class <i>UWAUVErrorObjectDetection</i> with Object Detection, based on <i>UwCbr</i>.
* <i>UWAUVErrorObjectDetection</i> can manage no more than 2^16 packets. If a module generates more
* than 2^16 packets, they will be dropped, according with <i>UwCbr</i>.
* <i>UWAUVErrorObjectDetection</i> sends periodically monitoring packets containing information about
* the current position and acknowledges the last control packet received.
* Each control packet contains the next waypoint that has to be reach.
*/

#ifndef UWAUVObjectDetection_MODULE_H
#define UWAUVObjectDetection_MODULE_H
#include <uwcbr-module.h>
#include <uwauv-packet.h>
#include "uwsmposition.h"
#include <queue>
#include <fstream>
#define UWAUVObjectDetection_DROP_REASON_UNKNOWN_TYPE "UKT" /**< Reason for a drop in a <i>UWAUV</i> module. */
#define UWAUVObjectDetection_DROP_REASON_OUT_OF_SEQUENCE "OOS" /**< Reason for a drop in a <i>UWAUV</i> module. */
#define UWAUVObjectDetection_DROP_REASON_DUPLICATED_PACKET "DPK" /**< Reason for a drop in a <i>UWAUV</i> module. */




class UwAUVODModule;

/**
* UwSendTimer class is used to handle the scheduling period of <i>UWAUV</i> packets.
*/
class UwAUVODSendTimer : public UwSendTimer {
public:

    /**
   * Conscructor of UwSendTimer class
   * @param UwAUVCtrModule *m pointer to an object of type UwAUVCtrModule
   */
    UwAUVODSendTimer(UwAUVODModule *m) : UwSendTimer((UwCbrModule*)(m)){
    };
};

/**
* UwAUVModule class is used to manage <i>UWAUV</i> packets and to collect statistics about them.
*/
class UwAUVODModule : public UwCbrModule {
public:

    /**
    * Default Constructor of UwAUVModule class.
    */
    UwAUVODModule();

    /**
    * Constructor with position setting of UwAUVModule class.
    *
    * @param UWSMPosition* p Pointer to the AUV position
    */
    UwAUVODModule(UWSMPosition* p);

    /**
    * Destructor of UwAUVModule class.
    */
    virtual ~UwAUVODModule();

    /**
   * TCL command interpreter. It implements the following OTcl methods:
   *
   * @param argc Number of arguments in <i>argv</i>.
   * @param argv Array of strings which are the command parameters (Note that <i>argv[0]</i> is the name of the object).
   * @return TCL_OK or TCL_ERROR whether the command has been dispatched successfully or not.
   *
   **/
    virtual int command(int argc, const char*const* argv);

    /**
   * Initializes a monitoring data packet passed as argument with the default values.
   *
   * @param Packet* Pointer to a packet already allocated to fill with the right values.
   */
    virtual void initPkt(Packet* p) ;

    /**
    * Performs the reception of packets from upper and lower layers.
    *
    * @param Packet* Pointer to the packet will be received.
    */
    virtual void recv(Packet*);

    /**
    * Performs the reception of packets from upper and lower layers.
    *
    * @param Packet* Pointer to the packet will be received.
    * @param Handler* Handler.
    */
    virtual void recv(Packet* p, Handler* h);

    /**
    * Reset retransmissions
    */
    inline void reset_retx() {p=NULL; sendTmr_.force_cancel();}


    /**
    * Creates and transmits a packet.
    *
    * @see UwCbrModule::sendPkt()
    */
    virtual void transmit();

    /**
    * Sets the position of the AUV
    *
    * @param UWSMPosition * p Pointer to the AUV position
    */
    virtual void setPosition(UWSMPosition* p);

    /**
    * Returns the position of the AUV
    *
    * @return the current AUV position
    */
    inline UWSMPosition* getPosition() { return posit; }

    /**
    * Returns the size in byte of a <i>hdr_uwAUV_monitoring</i> packet header.
    *
    * @return The size of a <i>hdr_uwAUV_monitoring</i> packet header.
    */
    static inline int getAUVMonHeaderSize() { return sizeof(hdr_uwAUV_monitoring); }

    /**
    * Returns the size in byte of a <i>hdr_uwAUV_ctr</i> packet header.
    *
    * @return The size of a <i>hdr_uwAUV_ctr</i> packet header.
    */
    static inline int getAUVCTRHeaderSize() { return sizeof(hdr_uwAUV_ctr); }

    /**
    * Returns the size in byte of a <i>hdr_uwAUV_error</i> packet header.
    *
    * @return The size of a <i>hdr_uwAUV_error</i> packet header.
    */
    static inline int getAUVErrorHeaderSize() { return sizeof(hdr_uwAUV_error); }
	/**
    * Returns the size in byte of a <i>hdr_uwAUV_od</i> packet header.
    *
    * @return The size of a <i>hdr_uwAUV_od</i> packet header.
    */
    static inline int getAUVODHeaderSize() { return sizeof(hdr_uwAUV_od); }

protected:

    UWSMWPPosition* posit; /**< AUV position.*/
	int last_sn_confirmed;/**< Sequence number of the last command Packete received.*/
	int sn;
	int drop_old_waypoints; /** < Flag set to 1 to drop waypoints with sequence number
								lower or equal than last_sn_confirmed.*/
	int log_on_file; /**< Flag to enable log file writing.*/
	Packet* p; /**< Pointer to the packet that will be received*/
	int period;

	std::ofstream out_file_stats; /**< Output stream for the textual file of debug */
	std::ofstream maybe_object_detection_log;
	std::ofstream true_object_detection_log;
    float x_od; /**< x coordinate object detected*/
    float y_od; /**< y coordinate object detected*/
    int error_m; /**< error measurement indicator*/
    int alarm_mode; /**< object detection status
					 * 0 no object in presence
					 * 1 maybe there is an object
					 * 2 object in presence
					*/


private:
    double speed; /**< speed of the AUV*/
};


#endif //UWAUVObjectDetection_MODULE_H
