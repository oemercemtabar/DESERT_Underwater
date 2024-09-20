//
// Copyright (c) 2023 Regents of the SIGNET lab, University of Padova.
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
* @file uwauvoderror-module.cc
* @author Omer Cem Tabar
* @version 1.0.0
*
* \brief Provides the <i>UWAUVODError</i> class implementation.
*
*/

#include "uwauvobjectdetection-module.h"
#include <iostream>
#include <rng.h>
#include <random>
#include <stdint.h>
extern packet_t PT_UWCBR;
extern packet_t PT_UWAUV;
extern packet_t PT_UWAUV_CTR;
extern packet_t PT_UWAUV_ERROR;
extern packet_t PT_UWAUV_OD;
int hdr_uwAUV_od::offset_; /**< Offset used to access in
									<i>hdr_uwAUV_od</i> packets header. */
/**
* Adds the header for <i>hdr_uwAUVError</i> packets in ns2.
*/

/**
 * Class that represents the binding with the tcl configuration script
 */
static class UwAUVODModule : public TclClass {
public:

    /**
    * Constructor of the class
    */
    UwAUVODModule() : TclClass("Module/UW/AUV/OD") {
    }

    /**
    * Creates the TCL object needed for the tcl language interpretation
    * @return Pointer to an TclObject
    */
    TclObject* create(int, const char*const*) {
        return (new UwAUVODModule());
    }
} class_module_uwAUV;



UwAUVErrorModule::UwAUVODModule()
        : UwCbrModule()
        , last_sn_confirmed(0)
        , sn(0)
        , drop_old_waypoints(1)
        , log_on_file(0)
        , period(60)
        , error_m(0)
        , alarm_mode(0)
        , speed(0.5)

{

    posit= new UWSMWPPosition();
    bind("drop_old_waypoints_", (int*) &drop_old_waypoints);
    bind("log_on_file_", (int*) &log_on_file );
    bind("period_", (int*) &period );
}

UwAUVErrorModule::UwAUVODModule(UWSMWPPosition* p)
        : UwCbrModule()
        , last_sn_confirmed(0)
        , sn(0)
        , drop_old_waypoints(1)
        , log_on_file(0)
        , period(60)
        , error_m(0)
        , alarm_mode(0)
        , speed(0.5)


{
    posit = p;
    bind("drop_old_waypoints_", (int*) &drop_old_waypoints);
    bind("log_on_file_", (int*) &log_on_file );
    bind("period_", (int*) &period );

}

UwAUVODModule::~UwAUVODModule() {}

void UwAUVODModule::setPosition(UWSMWPPosition* p){
    posit = p;
}

int UwAUVODModule::command(int argc, const char*const* argv) {
    Tcl& tcl = Tcl::instance();
    if(argc == 2){
        if (strcasecmp(argv[1], "getAUVMonheadersize") == 0) {
            tcl.resultf("%d", getAUVMonHeaderSize());
            return TCL_OK;
        } else if(strcasecmp(argv[1], "getAUVctrheadersize") == 0) {
            tcl.resultf("%d", getAUVCTRHeaderSize());
            return TCL_OK;
        } else if(strcasecmp(argv[1], "getAUVErrorheadersize") == 0) {
            tcl.resultf("%d", getAUVErrorHeaderSize());
            return TCL_OK;
        } else if(strcasecmp(argv[1], "getAUVODheadersize") == 0) {
            tcl.resultf("%d", getAUVErrorHeaderSize());
            return TCL_OK;
        }else if(strcasecmp(argv[1], "getX") == 0) {
            tcl.resultf("%f", posit->getX());
            return TCL_OK;
        } else if(strcasecmp(argv[1], "getY") == 0) {
            tcl.resultf("%f", posit->getY());
            return TCL_OK;
        } else if(strcasecmp(argv[1], "getZ") == 0) {
            tcl.resultf("%f", posit->getZ());
            return TCL_OK;
        }
    } else if(argc == 3){
        if (strcasecmp(argv[1], "setPosition") == 0) {
            UWSMWPPosition* p = dynamic_cast<UWSMWPPosition*> (tcl.lookup(argv[2]));
            if(p){
                posit=p;
                tcl.resultf("%s", "position Setted\n");
                return TCL_OK;
            } else {
                tcl.resultf("%s", "Invalid position\n");
                return TCL_ERROR;
            }
        }
    } else if(argc == 5){
        if (strcasecmp(argv[1], "setDest") == 0) {
            posit->setDest(atof(argv[2]),atof(argv[3]),atof(argv[4]));
            return TCL_OK;
        } else if (strcasecmp(argv[1], "addDest") == 0) {
            posit->addDest(atof(argv[2]),atof(argv[3]),atof(argv[4]));
            return TCL_OK;
        }
    } else if(argc == 6){
        if (strcasecmp(argv[1], "setDest") == 0) {
            posit->setDest(atof(argv[2]),atof(argv[3]),atof(argv[4]),atof(argv[5]));
            return TCL_OK;
        } else if (strcasecmp(argv[1], "addDest") == 0) {
            posit->addDest(atof(argv[2]),atof(argv[3]),atof(argv[4]),atof(argv[5]));
            return TCL_OK;
        }

    }
    return UwCbrModule::command(argc,argv);
}

void UwAUVODModule::transmit() {
    sendPkt();

    if (debug_) {
        std::cout << NOW << " UwAUV_OD_Module::Sending pkt with period: " <<
                  period << std::endl;
    }
    sendTmr_.resched(period);
}

void UwAUVODModule::initPkt(Packet* p) {
    hdr_uwcbr *uwcbrh = HDR_UWCBR(p);
    hdr_uwAUV_od* uwAUVh = hdr_uwAUV_od::access(p);

    // Reset the error
    uwAUVh->error() = 0;

    static int line_position = 0; // Static variable to remember position

    // Open the file and read from the last position
    std::ifstream infile("objectdetection-inputs.txt");
    std::string line;

    // Skip lines until the last read position
    for (int i = 0; i < line_position && std::getline(infile, line); ++i);

    if (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string token;
        int frame_number, object_detected, edge_count;
        double timestamp;
        std::string frame_size, byte_size;

        // Parsing the text file
        std::getline(ss, token, '/'); frame_number = std::stoi(token);
        std::getline(ss, token, ':'); timestamp = std::stod(token);
        std::getline(ss, token, ':'); // ignoring further timestamp components
        std::getline(ss, token, '/'); frame_size = token;
        std::getline(ss, token, '/'); byte_size = token;
        std::getline(ss, token, '/'); object_detected = std::stoi(token);
        std::getline(ss, token); edge_count = std::stoi(token);
		if (alarm_mode != 2 ){ //if not in alarm mode

			if (alarm_mode == 1){ //gray zone

				error_m = getObjectDetectionMeasure(edge_count,object_detected);

				if (debug_) {
					std::cout << NOW << " UwAUVErroModule::initPkt(Packet *p) "
						<< "Gray Zone error, new measure: "<< error_m << std::endl;
				}

			} else {

				error_m = getObjectDetectionMeasure(edge_count,object_detected);

				if (debug_) {
					std::cout << NOW << " UwAUVObjectDetectionModule::initPkt(Packet *p) "
						<< "New error, measure: "<< error_m << std::endl;
				}

				if(alarm_mode == 1){ // start error timer

					if (log_on_file == 1) {

						err_log.open("log/error_log_t.csv",std::ios_base::app);
						err_log << "G,"<< NOW << "," << posit->getX() <<","<<
							posit->getY() <<", ON"<< std::endl;
						err_log.close();

						err_log.open("log/error_log.csv",std::ios_base::app);
						err_log << "ON,"<< NOW << "," << x_e <<","<< y_e << std::endl;
						err_log.close();
					}

				}
			}

			if (alarm_mode == 1){

				x_e = posit->getX(); // Save error position
				y_e = posit->getY();

				//STOP
				posit->setDest(posit->getXdest(),posit->getYdest(),posit->getZdest(),0);
				posit->setAlarm(true);

				uwAUVh->x() = x_e;
				uwAUVh->y() = y_e;
				uwAUVh->error() = error_m;
				this->p = p;

				if (debug_) {
					std::cout << NOW << " UwAUVObjectDetectionModule::initPkt(Packet *p) "
						<< "ERROR ("<< x_e <<","<< y_e << "," << error_m <<"),"
						<<"alarm_mode = "<< alarm_mode<< std::endl;
				}

			} else {

				if (debug_) {
					std::cout << NOW << " UwAUVObjectDetectionModule::initPkt(Packet *p) "
						<< "no error "<<" alarm_mode = " << alarm_mode << std::endl;
				}

			}

			uwAUVh->sn() = ++sn;

		} else {

			uwAUVh->x() = x_e;
			uwAUVh->y() = y_e;
			uwAUVh->error() = error_m;
			this->p = p;
			uwAUVh->sn() = ++sn;
		}

		UwCbrModule::initPkt(p);

		if (log_on_file == 1) {
				out_file_stats.open("log/position_log_a.csv",std::ios_base::app);
				out_file_stats << NOW << "," << posit->getX() << ","<< posit->getY()
					<< "," << posit->getZ() << ", " << posit->getSpeed()<< std::endl;
				out_file_stats.close();
		}
    }

    // Update the line position
    line_position++;
}



void UwAUVODModule::recv(Packet* p) {
    hdr_uwAUV_error* uwAUVh = hdr_uwAUV_od::access(p);

	//obsolete packets
	if (drop_old_waypoints == 1 && uwAUVh->sn() <= last_sn_confirmed) {
			if (debug_) {
				std::cout << NOW << " UwAUVObjectDetectionModule::old error with sn "
					<< uwAUVh->sn() << " dropped " << std::endl;
			}

	} else { //packet in order

		/**
		 * error > 0 tx more data
		 * error > 1 stop tx, Ctr is coming
		 * error < 0 stop tx, there is no error
		*/

		//Valid pkt refering to my error
		if (alarm_mode && uwAUVh->x() == x_e && uwAUVh->y() == y_e){

			if (uwAUVh->error() < 0 ){

				posit->setAlarm(false);
				posit->setDest(posit->getXdest(),posit->getYdest(),posit->getZdest(),speed);

				sendTmr_.force_cancel();
				sendTmr_.resched(period);

				if (log_on_file == 1) {

					err_log.open("log/error_log_t.csv",std::ios_base::app);
					err_log << "W,"<< NOW << "," << x_e <<","<< y_e <<
						",ON"<< std::endl;
					err_log.close();

					err_log.open("log/error_log.csv",std::ios_base::app);
					err_log << "OFF,"<< NOW << "," << x_e <<","<< y_e
						<< std::endl;
					err_log.close();

					if (uwAUVh->error() == -1){

						if(t_e <= th_ne){

							t_err_log.open("log/true_error_log.csv",std::ios_base::app);
							t_err_log << NOW << "," << x_e <<","<< y_e <<",tn"
								<< std::endl;
							t_err_log.close();

						} else {

							t_err_log.open("log/true_error_log.csv",std::ios_base::app);
							t_err_log << NOW << "," << x_e <<","<< y_e <<",fn"
								<< std::endl;
							t_err_log.close();

						}
					}

				}

				alarm_mode = 0;


				if (debug_) {
					std::cout << NOW << " UwAUVObjectDetectionModule::recv(Packet *p) error("
						<< x_e <<","<< y_e <<") solved AUV can move again"
						<< "with speed=" << posit->getSpeed()<< std::endl;
				}

			} else if (uwAUVh->error() >= 1 ){

				alarm_mode = 2;

				std::cout << NOW << " UwAUVErrModule::recv(Packet *p) for"
						<< "SURE there is an error ("<< x_e <<","<< y_e <<")"
						<< "STOP until ctr arrival"<< std::endl;

				if (debug_)
					std::cout << NOW << " UwAUVErrModule::recv(Packet *p) for"
						<< "SURE there is an error ("<< x_e <<","<< y_e <<")"
						<< "STOP until ctr arrival"<< std::endl;

			} else {// ->error=0

				alarm_mode = 1;

				if (debug_)
					std::cout << NOW << " UwAUVErrModule::recv(Packet *p)"
						<< "MAYBE there is an error ("<< x_e <<","<< y_e <<")"<<
						"continue tx"<< std::endl;

			}
		}

		last_sn_confirmed = uwAUVh->sn();
	}

	UwCbrModule::recv(p);

	if (log_on_file == 1) {
		out_file_stats.open("log/position_log_a.csv",std::ios_base::app);
		out_file_stats << NOW << "," << posit->getX() << ","<< posit->getY()
			<< "," << posit->getZ() << ',' << posit->getSpeed() << std::endl;
		out_file_stats.close();
	}
}

int UwAUVODModule::getObjectDetectionMeasure(edge_count,object_detected){
	// Set the alarm mode based on edge_count and object_detected

     if (edge_count > 10000 && edge_count <= 30000) {
         m = 0.5;
         alarm_mode = 1;  // Gray zone, check object detection
         if (object_detected == 1) {
             if (log_on_file == 1) {
                 err_log.open("log/error_log.csv", std::ios_base::app);
                 err_log << "Alarm mode 2 triggered by object detection, edge_count: "
                         << edge_count << std::endl;
                 err_log.close();
             }
         }
     }
     else{
       if (edge_count > 3000 && edge_count <= 10000) {
         if (object_detected == 1) {
           m = 1.5;
             if (log_on_file == 1) {
                 err_log.open("log/error_log.csv", std::ios_base::app);
                 err_log << "Alarm mode 2 triggered by object detection, edge_count: "
                         << edge_count << std::endl;
                 err_log.close();
             }
         }
       }
       else{
         m = 0.0;
       }
     }
	return m;


}


